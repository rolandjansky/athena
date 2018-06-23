# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# file to parse log files from grid for rate compilation
# grid outputs logs as tarballs (.tgz)
# instructions in compileRPVLLRates.p
# receives two arguments: output file + input directory containing log files to be parsed

from os import listdir
from os.path import isfile, join, isdir
import re
import sys
import tarfile
import os

text_file = open(sys.argv[1], "w")
mypath = sys.argv[2]
# get list of grid log tgz tarballs w/in directory
tgzs = [t for t in listdir(mypath) if isfile(join(mypath, t))]
# cd into directory containing tarballs
os.chdir(mypath)

# loop through tarballs and untar
print 'About to loop through tarballs!'
for t in tgzs:
    tar = tarfile.open(t, "r:gz")
    tar.extractall()
    tar.close()

# get list of untarred tarballs
tarballs = [x for x in listdir(os.getcwd()) if isdir(x)]
# loop through untarred tarballs + cd into them
for x in tarballs:
    # loop through files in tarballs
    files = [f for f in listdir(x)]
    for f in files:
        # open log.RAWtoALL + extract filter info
        if "log.RAWtoALL" in f:
            openFile = open(x+'/'+f)
            for line in openFile:
                # find input data file for lumiblock info
                if re.match('.*Py:AthFile\s*INFO opening \[\S*\]...',line):
                    data_file = line[line.index("[")+1:line.rindex("]")]
                # find RPVLL filter information
                if re.match('.*RPVLL.*Events',line) or re.match('.*BSESOutputSvcStreamDRAW_RPVLL.*events',line):
                    # write input data file + RPVLL info to output file
                    text_file.write(data_file + ':' + line)
# close output text file
text_file.close()
