#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os, os.path

createdbhisto = 'CreateDB_Histo.py'
patharr = os.environ["PATH"].split(":")

fileFound = False
for path in patharr:
  file = os.path.join( path, createdbhisto )
  if os.path.exists( file ):
    createdbhisto = file
    fileFound = True
    break

if not fileFound: 
  print createdbhisto + " not found. Exit."
  sys.exit(1)

if len(sys.argv)>=2:
  sys.argv[1] = "createdb"
else: sys.argv.append("createdb")

execfile(createdbhisto)

