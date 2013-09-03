# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################
# Author:      Marc-Andre Dufour                       #
# Date:        Mar 20, 2007                            #
#                                                      #
# Description: Retrieve a list of signatures to study  #
#              from the file passed as an argument.    #
########################################################

#! /usr/bin/env python

import sys

# Separate the a line in tokens
def SplitLines (line):
    line = line.strip()
    new_list = line.split()

    return new_list

file = sys.argv[1]

f_sig = open(file,'r') # file containing the signatures
fw = open('temp_sigs','w') # file containing the signatures

lines = f_sig.readlines()
sig_count = 0
i = 0

temp = []
l = [] # List of single signatures

for line in lines:
    if len(line) > 1:
        temp.append(SplitLines(line))


for tup in temp:
    if len(tup) == 4:
        l.append(tup[3])
        sig_count = sig_count+1
while i < sig_count:
    fw.write('\"'+l[i]+'\",\n')
    i = i+1

fw.write('\"'+l[i-1]+'\"]\n')


f_sig.close()

