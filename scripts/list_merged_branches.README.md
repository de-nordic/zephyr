# List Merged Branches

## Overview

The `list_merged_branches.py` script helps you identify branches in your local repository that have already been merged to the upstream zephyrproject-rtos/zephyr repository. This is useful for:

- Finding branches that can be safely deleted
- Tracking which of your contributions have been accepted upstream
- Maintaining a clean local branch list

## Prerequisites

Before using this script, you need to add the upstream zephyrproject remote to your repository:

```bash
git remote add zephyrproject https://github.com/zephyrproject-rtos/zephyr.git
git fetch zephyrproject
```

## Usage

### Basic Usage

Check if the current branch has been merged:

```bash
python3 scripts/list_merged_branches.py
```

Or use the shell wrapper:

```bash
./scripts/list-merged-branches.sh
```

### Check All Local Branches

To check all local branches instead of just the current one:

```bash
python3 scripts/list_merged_branches.py --all
```

### Fetch Before Checking

To fetch the latest changes from upstream before checking:

```bash
python3 scripts/list_merged_branches.py --all --fetch
```

### Custom Remote or Branch

If you're using a different remote name or want to check against a different upstream branch:

```bash
python3 scripts/list_merged_branches.py --remote upstream --branch develop --all
```

## Options

- `--remote REMOTE`: Name of the upstream remote (default: `zephyrproject`)
- `--branch BRANCH`: Upstream branch to check against (default: `main`)
- `--fetch`: Fetch from remote before checking
- `--all`: Check all local branches instead of just the current one
- `--help`: Show help message

## Examples

### Example 1: Check Current Branch

```bash
$ python3 scripts/list_merged_branches.py
Current branch 'feature-xyz' has NOT been merged to zephyrproject/main
```

### Example 2: Find All Merged Branches

```bash
$ python3 scripts/list_merged_branches.py --all
Branches merged to zephyrproject/main:
  bugfix-123
  feature-abc
  hotfix-456

Total: 3 branch(es)
```

### Example 3: Check with Fresh Data

```bash
$ python3 scripts/list_merged_branches.py --all --fetch
Fetching from zephyrproject...
Branches merged to zephyrproject/main:
  bugfix-123
  feature-abc

Total: 2 branch(es)
```

## How It Works

The script uses Git's built-in merge detection (`git branch --merged`) to identify branches whose commits have all been incorporated into the upstream branch. This means:

- If all commits from your branch are present in upstream (possibly cherry-picked or rebased), it will be detected as merged
- The branch doesn't need to have been merged via a pull request; any way of getting the commits into upstream counts
- The check is based on commit content, not commit SHAs, so rebased commits are still detected

## Cleanup

Once you've identified merged branches, you can delete them locally:

```bash
# Delete a single branch
git branch -d branch-name

# Delete multiple branches
git branch -d branch1 branch2 branch3
```

If you also want to delete the remote branches:

```bash
git push origin --delete branch-name
```

## Troubleshooting

### "Remote 'zephyrproject' does not exist"

You need to add the upstream remote first:

```bash
git remote add zephyrproject https://github.com/zephyrproject-rtos/zephyr.git
git fetch zephyrproject
```

### Script Shows No Merged Branches But I Know Some Are Merged

Try fetching the latest upstream changes first:

```bash
git fetch zephyrproject
python3 scripts/list_merged_branches.py --all
```

### False Positives

The `git branch --merged` command can sometimes show branches as merged even if they haven't been. This typically happens when:
- The branch was created from an old upstream commit
- The commits have been significantly modified during merge

Always double-check before deleting branches!
