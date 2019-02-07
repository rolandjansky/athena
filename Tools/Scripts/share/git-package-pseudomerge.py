#!/usr/bin/env python


# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
# @file:    git-package-pseudomerge.py
# @author:  Tim Martin
#

"""
Merge individual Athena packages between git branches. Commit the resulting diff as a cherry-pick.
"""

__author__  = "Tim Martin"
__version__ = "$Revision: 1.0 $"
__doc__     = "Merge individual Athena packages between git branches. Commit the resulting diff as a cherry-pick."

import argparse, time

parser = argparse.ArgumentParser(description='\
  Perform a pseudo-merge (a selective merge followed by cherry-pick) of select packages between branches in Athena. \
  This performs a proper merge, using the full history in both the source and target branches but restricting to select packages (paths). \
  The result of this operation, after conflicts are resolved, is isolated as a cherry-pick which is used to apply the changes to the official code base.')
parser.add_argument('--packages', type=str, nargs='+', required=True,
                   help='List of package paths to migrate to the target branch, separated by spaces "Path/To/Package1 Path/To/Package2" etc.')
parser.add_argument('--stage', type=int,
                   help='Set initially to 1 to setup the merge and present conflicts, set to 2 to create a branch which will apply the changes of the pseudo-merge')
parser.add_argument('--source', type=str,
                   default='upstream/21.0', help='source branch (default: upstream/21.0)')
parser.add_argument('--target', type=str,
                   default='upstream/master', help='target branch (default: upstream/master)')
parser.add_argument('--reset', action='store_true',
                   help='Remove branches created by this script. Use to start again or recover from any issues')
parser.add_argument('--debug', action='store_true',
                   help='Extra printing')

args = parser.parse_args()

def sortBySplitLen(element):
    return len(element.split('/'))

args.packages = sorted(args.packages, key=sortBySplitLen)

print(args)

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

try:
  from subprocess import call, STDOUT, check_output
  import os
except:
  print(bcolors.FAIL  + 'Cannot import subprocess or os, please make sure you have an OKish Python version. "asetup Athena,master,latest && lsetup git" will set you up.' + bcolors.ENDC)
  exit()

if call(["git", "branch"], stderr=STDOUT, stdout=open(os.devnull, 'w')) != 0:
  print(bcolors.FAIL  + 'Must be called from within a clone of Athena. If you are in your athena directory then please double check that you have run "lsetup git".' + bcolors.ENDC)
  exit()

import time
userBranch = os.environ['USER'] + "_" + time.strftime('%d_%b') + "_" + args.packages[0].rstrip('/').split('/')[-1] + "_to_" + args.target.split('/')[-1]
userTempBranch = userBranch + "_TEMP_BRANCH_DO_NOT_MERGE_TO_OFFICIAL_REPOSITORY"

if call(["git", "branch"], stderr=STDOUT, stdout=open(os.devnull, 'w')) != 0:
  print(bcolors.FAIL  + 'Must be called from within a clone of Athena' + bcolors.ENDC)
  exit()

if args.reset:
  print(bcolors.HEADER + 'Removing branches created by this script.' + bcolors.ENDC)
  print(bcolors.HEADER + 'Checking out ' + args.target + bcolors.ENDC)
  call(["rm", "-f", ".git/index.lock"])
  print(bcolors.OKBLUE + 'git checkout --force ' + args.target + bcolors.ENDC)
  call(["git", "checkout", "--force", args.target])
  time.sleep(0.1)
  print(bcolors.OKBLUE + 'git reset --hard HEAD' + bcolors.ENDC)
  call(["git", "reset", "--hard", "HEAD"])
  time.sleep(0.1)
  print(bcolors.OKBLUE + 'git clean -fdx' + bcolors.ENDC)
  call(["git", "clean", "-fdx"])
  time.sleep(0.1)
  #
  print(bcolors.HEADER + 'Delete local branch ' + userBranch + bcolors.ENDC)
  print(bcolors.OKBLUE + 'git branch -D ' + userBranch + bcolors.ENDC)
  call(["git", "branch", "-D", userBranch])
  time.sleep(0.1)
  #
  print(bcolors.HEADER + 'Delete remote branch ' + userBranch + bcolors.ENDC)
  print(bcolors.OKBLUE + 'git push origin --delete ' + userBranch + bcolors.ENDC)
  call(["git", "push", "origin", "--delete", userBranch])
  time.sleep(0.1)
  #
  print(bcolors.HEADER + 'Delete local temporary branch ' + userTempBranch + bcolors.ENDC)
  print(bcolors.OKBLUE + 'git branch -D ' + userBranch + bcolors.ENDC)
  call(["git", "branch", "-D", userTempBranch])
  time.sleep(0.1)
  #
  print(bcolors.HEADER + 'Reset done. Start again by specifying "--stage 1"' + bcolors.ENDC)
  exit()

try:
  args.stage
except NameError:
  print(bcolors.FAIL  + 'Must specify "--stage 1" or "--stage 2"' + bcolors.ENDC)
  exit()

if args.stage == 1:
  print(bcolors.HEADER + 'Updating from upstream' + bcolors.ENDC)
  print(bcolors.OKBLUE + 'git fetch upstream ' + bcolors.ENDC)
  call(["git", "fetch", "upstream"])

  # Check we're not going to trample on work
  localChanges = check_output(["git", "status", "--porcelain"])
  if localChanges != "":
    print("Local changes: " + localChanges)
    prompt = raw_input(bcolors.WARNING + 'This will discard all local changes in the repository which are not already committed, please confirm this is OK! (y/n): ' + bcolors.ENDC)
    if (prompt != "Y" and prompt != "y"):
      print(bcolors.HEADER + 'Exiting' + bcolors.ENDC)
      exit()

  print(bcolors.HEADER + 'Checking out ' + args.target + ' and discarding any un-committed changes' + bcolors.ENDC)

  # Try and cover all cases - start in a good place
  call(["rm", "-f", ".git/index.lock"])
  print(bcolors.OKBLUE + 'git checkout --force ' + args.target + bcolors.ENDC)
  call(["git", "checkout", "--force", args.target])
  time.sleep(0.1)
  #
  print(bcolors.OKBLUE + 'git clean -fdx' + bcolors.ENDC)
  call(["git", "clean", "-fdx"])
  time.sleep(0.1)

  print(bcolors.HEADER + 'Creating new temporary branch: ' + userTempBranch + bcolors.ENDC)

  # Make the new branch
  print(bcolors.OKBLUE + 'git branch ' + userTempBranch + ' ' + args.target + bcolors.ENDC)
  call(["git", "branch", userTempBranch, args.target])
  time.sleep(0.1)
  print(bcolors.OKBLUE + 'git checkout ' + userTempBranch + bcolors.ENDC)
  call(["git", "checkout", userTempBranch])
  time.sleep(0.1)

  print(bcolors.HEADER + 'Performing merge (this may take a while...)' + bcolors.ENDC)
  print(bcolors.OKBLUE + 'git merge --no-commit ' + args.source + bcolors.ENDC)
  call(["git", "merge", "--no-commit", args.source])
  time.sleep(0.1)

  print(bcolors.HEADER + 'Performing reset ' + bcolors.ENDC)

  result = check_output(["git", "status", "--porcelain"])
  toReset = []
  responsibleRule = ""

  for line in result.splitlines():
    fileTuple = line.split() # Git status flag and path
    fileSplit = fileTuple[1].split('/') # Exploded path
    level = 0 # Distance into directory tree
    resetPath = ''
    while (True):
      if (args.debug): print("Evaluate " + fileTuple[1] + " level=" + str(level))
      if level == len(fileSplit): # Reached beyond bottom of directory tree
        print(bcolors.FAIL + "ERR" + bcolors.ENDC)
        break

      doKeep = False
      doProgress = False
      for path in args.packages: # For each package in the list of merge-packages
        pathSplit = path.rstrip('/').split('/') # Explode path
        if level == len(pathSplit) and path == responsibleRule:
          doKeep = True  # The previous operation matched the final path segment. Keep this file
          if (args.debug): print("  doKeep=True (" + path + " = " + responsibleRule + ") as level is equal to size of " + path + ", " + str(len(pathSplit)) )
        elif doKeep == False and level < len(pathSplit) and fileSplit[level] == pathSplit[level]:
          doProgress = True # We match the current level - check next level of directory
          responsibleRule = path # Record which package rule is allowing us to progress
          if (args.debug): print("  doProgress=True (" + fileSplit[level] + " = " + pathSplit[level] + ") as level is less than the size of " + path + ", " + str(len(pathSplit)) )

      if doKeep: # At least one match to keep this
        if (args.debug): print("    Keeping  "+path)
        break
      elif doProgress: # At least one partial match - check next level
        resetPath += fileSplit[level] + '/'
        if (args.debug): print("    Progressing  "+resetPath)
        level += 1
      else: # No matches - reset this path
        resetPath += fileSplit[level]
        if (args.debug): print("    Resetting  "+resetPath)
        if not resetPath in toReset:
          print("To reset:  "+resetPath + " level = " + str(level))
          toReset.append(resetPath)
        break

  for pathToReset in toReset:
    print("Resetting: " + pathToReset)
    print(bcolors.OKBLUE + 'git reset HEAD -- ' + pathToReset + bcolors.ENDC)
    call(["git", "reset", "HEAD", "--", pathToReset], stdout=open(os.devnull, 'w'))
    time.sleep(0.05)
    print(bcolors.OKBLUE + 'git checkout HEAD -- ' + pathToReset + bcolors.ENDC)
    call(["git", "checkout", "HEAD", "--", pathToReset], stdout=open(os.devnull, 'w'))
    time.sleep(0.05)

  print(bcolors.HEADER + 'Removing remaining untracked' + bcolors.ENDC)
  print(bcolors.OKBLUE + 'git clean -fdx' + bcolors.ENDC)
  call(["git", "clean", "-fdx"])
  time.sleep(0.1)

  print(bcolors.HEADER + 'Printing git status' + bcolors.ENDC)
  print(bcolors.OKBLUE + 'git status' + bcolors.ENDC)
  call(["git", "status"])

  print(bcolors.HEADER + 'Please fix any merge conflicts which show up in red in the above "git status" output.' + bcolors.ENDC)
  print(bcolors.HEADER + 'Please then test recompilation and runtime tests of these (and any dependent) packages against the most recent ' + args.target + ' nightly' + bcolors.ENDC)
  print(bcolors.HEADER + 'When happy, run this command again with "--stage 2" to prepare the branch which may be merged to the official repository' + bcolors.ENDC)
  print(bcolors.FAIL + 'Do NOT commit anything yourself, "--stage 2" will handle committing the changes.' + bcolors.ENDC)

elif args.stage == 2:

  conflicted = check_output(["git", "diff", "--name-only", "--diff-filter=U"])
  if conflicted != "":
    print(bcolors.FAIL + "Conflicting paths remain, please resolve and use 'git add' on:\n" + conflicted  + bcolors.ENDC)
    print(bcolors.HEADER + 'Use "git status" to check the state of the merge' + bcolors.ENDC)
    print(bcolors.HEADER + 'Exiting' + bcolors.ENDC)
    exit()

  print(bcolors.HEADER + 'Stashing changes' + bcolors.ENDC)
  print(bcolors.OKBLUE + 'git stash' + bcolors.ENDC)
  call(["git", "stash"])
  time.sleep(0.1)

  print(bcolors.HEADER + 'Creating new branch: ' + userBranch + bcolors.ENDC)
  print(bcolors.OKBLUE + 'git checkout -b ' + userBranch + ' ' + args.target + bcolors.ENDC)
  call(["git", "checkout", "-b", userBranch, args.target])
  time.sleep(0.1)

  print(bcolors.HEADER + 'Popping stash' + bcolors.ENDC)
  print(bcolors.OKBLUE + 'git stash pop' + bcolors.ENDC)
  call(["git", "stash", "pop"])
  time.sleep(0.1)

  print(bcolors.HEADER + 'Committing' + bcolors.ENDC)
  allPackages = ""
  for path in args.packages:
    allPackages += path.rstrip('/').split('/')[-1] + " "

  commitMessage = "Update packages:" + allPackages + " from " + args.source + " to " + args.target + " via pseudo-merge"
  print(bcolors.OKBLUE + 'git commit -am "' + commitMessage + '"' + bcolors.ENDC)
  call(["git", "commit", "-am", commitMessage])
  time.sleep(0.1)

  print(bcolors.HEADER + 'Push to user fork and cleanup' + bcolors.ENDC)
  print(bcolors.OKBLUE + 'git push --set-upstream origin ' + userBranch + bcolors.ENDC)
  call(["git", "push", "--set-upstream", "origin", userBranch])
  time.sleep(0.1)

  print(bcolors.OKBLUE + 'git branch -D ' + userTempBranch + bcolors.ENDC)
  call(["git", "branch", "-D", userTempBranch])

  print(bcolors.HEADER + 'Please open a merge request for ' + userBranch + " to " + args.target + bcolors.ENDC)
  print(bcolors.HEADER + 'https://gitlab.cern.ch/' + os.environ['USER'] + '/athena/merge_requests/new' + bcolors.ENDC)
  print(bcolors.HEADER + 'Exiting' + bcolors.ENDC)
