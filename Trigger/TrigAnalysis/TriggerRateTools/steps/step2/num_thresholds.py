# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        Mar 20, 2007                            #
#                                                      #
# Description: Counts the number of sets of thresholds #
#              that are defined in the file passed as  #
#              the first argument.                     #
########################################################

#! /usr/bin/env python

import sys

file = sys.argv[1]

f = open(file,'r')

lines = f.readlines()
max_len = 0

l = []

for line in lines:
#    line = line.strip()
    l = line.split()
    if len(l) > max_len:
        if l[0] != 'None':
            max_len = len(l)

print max_len-3

f.close()
