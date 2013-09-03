# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        Mar 20, 2007                            #
#                                                      #
# Description: Retrieve thresholds from the config     #
#              file passed as an argument.             #
########################################################

#! /usr/bin/env python

import sys

def SplitLines (line):
    line = line.strip()
    new_list = line.split()

    return new_list
            

file = sys.argv[1]

f = open(file,'r')
fw = open('temp_thresholds','w')

lines = f.readlines()
list = []
used = []
arg2 = sys.argv[2]
pos = int(arg2.strip())

for line in lines:
    list.append(SplitLines(line))

for tup in list:
    if len(tup) >= pos+3:
        if used.count(tup[1]) == 0:
            fw.write(tup[1]+' = Algorithm( \"'+tup[1]+'\" )\n')
            used.append(tup[1])
        fw.write(tup[1]+'.'+tup[2]+' = '+tup[pos+2]+'\n')

f.close()
fw.close()
