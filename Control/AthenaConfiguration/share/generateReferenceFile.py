#!/usr/bin/env python

#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

import re
import glob
import json

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

with open("referenceDuplicates.txt", "w") as f:
    f.write(json.dumps(duplicates))
