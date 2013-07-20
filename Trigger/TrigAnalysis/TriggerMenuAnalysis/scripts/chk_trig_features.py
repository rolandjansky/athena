#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys
import re
import commands
import string

re_chain = re.compile('Inspecting chain: (\w+)')
re_feature = re.compile('Feature \[(\d+)\] clid=(\d+) index\.subType=(\d+) index\.begin=(\d+) index\.end=(\d+)')

def add_comb(combs, features):
    if len(features) == 0: return
    combs.append(features)
##     def get_clid(feature):
##         # feature = [ clid, n ]
##         return feature[0]
##     clids0 = map(get_clid, features)
##     found_same_type_of_comb = False
##     for comb in combs:
##         clids_tmp = map(get_clid, comb)
##         overlap=False
##         for clid in clids0:
##             if clids0 in clids_tmp:
##                 overlap = True
##                 break
##         if overlap:
##             found_same_type_of_comb = True
##             for feature in features:
##                 clid = feature[0]
##                 n = feature[1]
##                 for tmp in comb:
##                     if clid == tmp[0]:
##                         n_tmp = tmp[1]
##                         if n > tmp[1]: tmp[1] = n
##             break
##     if not found_same_type_of_comb:
##         combs.append(features)

#--------------------------------------------------------------------------
chain=''
combs = []
features=[]

chain_combs = {
    }

fname = 'chain_inspection.txt'
if len(sys.argv)>1:
    fname = sys.argv[1]

f = open(fname, 'r')
for line in f.readlines():
    mg = re_chain.search(line)
    if mg:
        #print 'found chain', mg.group(0)
        add_comb(combs, features)
        if chain != '' and len(combs) > 0:
            chain_combs[chain] = combs
        chain = mg.group(1)
        if chain in chain_combs.keys():
            combs = chain_combs[chain]
            break
        else:
            combs = []
        features = []
    mg = re_feature.search(line)
    if mg:
        #print 'found feature, line=', mg.group(0)
        ifeature = int(mg.group(1))
        clid = int(mg.group(2))
        subtype = int(mg.group(3))
        begin = int(mg.group(4))
        end = int(mg.group(5))
        if ifeature == 0:
            if len(features) > 0: add_comb(combs, features)
            features = []
        n = end - begin
        features.append([clid, n])

def build_clid_map(chain_combs0):
    clid_map = {}
    for combs in chain_combs0.values():
        for comb in combs:
            for f in comb:
                if f[0] not in clid_map.keys():
                    output = commands.getoutput('clid %d' % f[0])
                    clid_map[f[0]] = output.split()[1]
    return clid_map

clid_map = build_clid_map(chain_combs)

for (c, combs) in chain_combs.iteritems():
    print 'chain ', c
    scombs = []
    for comb in combs:
        s = []
        for f in comb:
            s.append(clid_map[f[0]])
        s.sort()
        s = string.join(s, ' ')
        if s not in scombs:
            scombs.append(s)
        print '  ' + s
    
