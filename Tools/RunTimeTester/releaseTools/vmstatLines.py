# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os.path

root='/afs/cern.ch/atlas/project/RTT/prod/SLC4_32/logs/logfiles_20feb_08:38'
fn = os.path.join(root, 'RTT.log')
ilines = open(fn, 'r').readlines()
olines1 = []
olines2 = []
ind = 0
for i in ilines:
    if i.find(' active ')>-1:
        olines1.extend(ilines[ind-1:ind+2])
    if i.find(' cache ')>-1:
        olines2.extend(ilines[ind-1:ind+2])
    ind +=1

olines1 = [ i[:17]+i[43:] for i in olines1]
olines2 = [ i[:17]+i[43:] for i in olines2]
of = open('vmstatLines1.txt', 'w')
[of.write(l) for l in olines1]
of = open('vmstatLines2.txt', 'w')
[of.write(l) for l in olines2]
