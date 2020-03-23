#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import sys 
import os
sys.path.append(os.path.abspath("../python"))
import MuonsCPContent as mcc

print (mcc.MuonsCPContent)

def makeContent(l):
    ct = 'MuonsCPContent = [\n'
    for i in l:
        ct += '"'+i+'",\n'
    ct += ']'
    return ct

def mergeList(l1, l2, excludeList=[]):
    l1a = []
    for i in l1:
        x = i.split('.')
        tag=x[0]
        found = False
        for ji in range(len(l2)):
            j = l2[ji]
#         for j in l2:
            y = j.split('.')
            if tag!=y[0]: continue
            found = True
            for m in x[1:]:
                if m in y[1:]: continue
                print (m, 'is not found for', tag)
                l2[ji] += '.'+m
        if (not found) and (not (tag in excludeList)):
            print (tag, 'is not found')
            l1a.append(i)
    return l2+l1a

ifile=sys.argv[1]
list1 = []
with open(ifile) as if1:
    for line in if1.readlines():
        list1.append(line.rstrip())
print (makeContent(list1))
print (makeContent(mcc.MuonsCPContent))

kk = mergeList(list1, mcc.MuonsCPContent, ['EventInfo', 'EventInfoAux']) 
nc = makeContent(kk)
with open('new_MuonsCPContent.py','w') as fout1:
    fout1.write(nc)
