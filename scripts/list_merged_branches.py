#!/usr/bin/env python3

# Copyright (c) 2026 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0

'''
List local branches that have already been merged to upstream.

This script identifies branches in the current repository that have been
merged to the zephyrproject-rtos/zephyr upstream repository. It's useful
for identifying branches that can be safely deleted since their changes
are already incorporated upstream.

Usage:
    python3 scripts/list_merged_branches.py [--remote REMOTE] [--branch BRANCH]

Options:
    --remote REMOTE     Name of the upstream remote (default: zephyrproject)
    --branch BRANCH     Upstream branch to check against (default: main)
    --fetch             Fetch from remote before checking (default: False)
    --all               Check all local branches, not just current (default: False)
'''

import argparse
import subprocess
import sys
from typing import List, Set


def run_git_command(args: List[str]) -> str:
    """Run a git command and return its output."""
    try:
        result = subprocess.run(
            ['git'] + args,
            capture_output=True,
            text=True,
            check=True
        )
        return result.stdout.strip()
    except subprocess.CalledProcessError as e:
        print(f"Error running git command: {e}", file=sys.stderr)
        print(f"stderr: {e.stderr}", file=sys.stderr)
        sys.exit(1)


def get_local_branches() -> List[str]:
    """Get list of all local branches."""
    output = run_git_command(['branch', '--format=%(refname:short)'])
    return [b.strip() for b in output.split('\n') if b.strip()]


def check_remote_exists(remote: str) -> bool:
    """Check if a remote exists."""
    try:
        output = run_git_command(['remote'])
        remotes = [r.strip() for r in output.split('\n') if r.strip()]
        return remote in remotes
    except Exception:
        return False


def get_merged_branches(remote: str, upstream_branch: str) -> Set[str]:
    """
    Get branches that have been merged to upstream.
    
    Uses git's merge detection to identify branches whose commits
    have all been incorporated into the upstream branch.
    """
    # Get the list of branches merged into the upstream branch
    try:
        output = run_git_command([
            'branch',
            '--merged',
            f'{remote}/{upstream_branch}',
            '--format=%(refname:short)'
        ])
        merged = set(b.strip() for b in output.split('\n') if b.strip())
        return merged
    except Exception:
        return set()


def get_current_branch() -> str:
    """Get the name of the current branch."""
    try:
        return run_git_command(['branch', '--show-current'])
    except Exception:
        return ""


def main():
    parser = argparse.ArgumentParser(
        description='List branches that have been merged to upstream',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    parser.add_argument(
        '--remote',
        default='zephyrproject',
        help='Name of the upstream remote (default: zephyrproject)'
    )
    parser.add_argument(
        '--branch',
        default='main',
        help='Upstream branch to check against (default: main)'
    )
    parser.add_argument(
        '--fetch',
        action='store_true',
        help='Fetch from remote before checking'
    )
    parser.add_argument(
        '--all',
        action='store_true',
        help='Check all local branches (default: only current branch)'
    )
    
    args = parser.parse_args()
    
    # Check if remote exists
    if not check_remote_exists(args.remote):
        print(f"Error: Remote '{args.remote}' does not exist.", file=sys.stderr)
        print(f"\nTo add the upstream remote, run:", file=sys.stderr)
        print(f"  git remote add {args.remote} https://github.com/zephyrproject-rtos/zephyr.git", file=sys.stderr)
        sys.exit(1)
    
    # Optionally fetch from remote
    if args.fetch:
        print(f"Fetching from {args.remote}...", file=sys.stderr)
        try:
            run_git_command(['fetch', args.remote])
        except Exception:
            print(f"Warning: Failed to fetch from {args.remote}", file=sys.stderr)
    
    # Get branches to check
    if args.all:
        branches_to_check = get_local_branches()
    else:
        current = get_current_branch()
        if not current:
            print("Error: Not on any branch and --all not specified", file=sys.stderr)
            sys.exit(1)
        branches_to_check = [current]
    
    # Get merged branches
    merged_branches = get_merged_branches(args.remote, args.branch)
    
    # Filter to only branches we're checking
    merged_branches = [b for b in branches_to_check if b in merged_branches]
    
    # Output results
    if not merged_branches:
        if args.all:
            print(f"No local branches have been merged to {args.remote}/{args.branch}")
        else:
            current = get_current_branch()
            print(f"Current branch '{current}' has NOT been merged to {args.remote}/{args.branch}")
    else:
        print(f"Branches merged to {args.remote}/{args.branch}:")
        for branch in sorted(merged_branches):
            print(f"  {branch}")
        
        if len(merged_branches) > 0:
            print(f"\nTotal: {len(merged_branches)} branch(es)")


if __name__ == '__main__':
    main()
