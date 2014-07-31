#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os, os.path

files=os.listdir(".")

for file in files:
  if not file.endswith("han.root"): continue
  splitArr = file.split("_")
  newfile = file.replace(splitArr[1],"")
  os.system("mv %s %s" % (file,newfile))

