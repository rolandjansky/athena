#!/usr/bin/env python

#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

import re
import glob
import json
import argparse

parser = argparse.ArgumentParser(description='Find duplicate config for athena modules in the current and all subfolders. Scans files ending in *onfig.py or *onfigNew.py. Ignores duplicates listed in given reference file.')
parser.add_argument('referenceFile', metavar='referenceFile', type=str,
                    help='Reference file with duplicates to ignore.')
args = parser.parse_args()


functionDict = {}
files = glob.glob('**/*onfig.py', recursive=True)
files = files + glob.glob('**/*onfigNew.py', recursive=True)
for file in files:
    for line in open(file, 'r', encoding="utf-8"):
        match = re.search(r"CompFactory\.(\w+\.?\w+) ?\(", line)
        if match:
            fun = match.group(1)
            if fun in functionDict:
                functionDict[fun].add(file)
            else:
                functionDict[fun] = {file}

duplicates = {}
for item in functionDict.items():
    if len(item[1]) > 1:
        duplicates[item[0]]=list(item[1])

with open(args.referenceFile) as f:
  ref = json.loads(f.read())
  addedDuplicates = {}
  removedDuplicates = {}
  for refItem in ref.items():
    if refItem[0] not in duplicates.keys():
      removedDuplicates[refItem[0]]=refItem[1]
      continue
    newRemoved = list(set(refItem[1]) - set(duplicates[refItem[0]]))
    if len(newRemoved) > 0:
      removedDuplicates[refItem[0]]= newRemoved
    newAdded = list(set(duplicates[refItem[0]]) - set(refItem[1]))
    if len(newAdded) > 0:
      addedDuplicates[refItem[0]] = newAdded
    del duplicates[refItem[0]]
  if len(duplicates) > 0:
    addedDuplicates.update(duplicates)

  if len(addedDuplicates) > 0:
    print("New module instance created which is also created in other configuration file(s), please consider using an existing instance (or update the reference file using generateReferenceFile.py):")
    print(addedDuplicates)
  if len(removedDuplicates) > 0:
    print("Please update the reference file, expected duplicates are not found:")
    print(removedDuplicates)
