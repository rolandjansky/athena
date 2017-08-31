# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# file to parse log files for rate compilation
# instructions can be found in compileRPVLLRates.py
# receives two arguments: output file + input directory containing log files to be parsed

from os import listdir
from os.path import isfile, join
import re
import sys

text_file = open(sys.argv[1], "w")
mypath = sys.argv[2]
# get list of log files w/in directory passed as argument
onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]

print 'About to loop through files!'
for f in onlyfiles:
  badName = False
  #sometimes ls files from mounted eos gives funny results. This should fix that!
  #double check that any files that had "sys" in the name are not duplicates of files without - in some runs, they all are! there should only be a handful.
  #if they're all duplicates, can just skip files with 'sys' in f instead.
  if 'sys' in f:
    badName = True
    openFile = open(mypath+'.'.join(f.split('.')[3:16]))
  else:
    openFile = open(mypath+f)
  print f
  for line in openFile:
    # changed from 'RAWtoESD' to 'RAWtoALL' in R21
    if re.match('RAWtoALL.*RPVLL.*Events',line) or re.match('RAWtoALL.*BSESOutputSvcStreamDRAW_RPVLL.*events',line):
      if badName: 
	text_file.write('.'.join(f.split('.')[3:16]) + ':' + line)
      else: 	
	text_file.write(f + ':' + line)
text_file.close()
