#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys
import commands
import string
import re

fname = 'chain_combs.txt'

f = open(fname, 'r')

clid_to_clsname = {}
features_in_chain = []
featureset_to_chain = {}
chain_name = ''

for line in f.readlines():
    i = line.find('Feature')
    if i > 0:
        if len(features_in_chain)>0:
            features_in_chain.sort()
            tmp = string.join(features_in_chain)
            if tmp in featureset_to_chain.keys():
                featureset_to_chain[tmp].append(chain_name)
            else:
                featureset_to_chain[tmp] = [chain_name]
            #for a in features_in_chain:
            #    print '  %s' % a
        features_in_chain = []
        chain_name = line[line.find(':')+2:-1]
        #print line[i:-1]
    i = line.find('  Comb:')
    if i > 0:
        clids = line[i+7:]
        #print 'clids: ', clids
        s = '  Comb:'
        for c in clids.split():
            #print ' c = ', c
            c = int(c)
            clsname = '???'
            if c in clid_to_clsname.keys():
                clsname = clid_to_clsname[c]
            else:
                output = commands.getoutput('clid %d' % c)
                clsname = output.split()[1]
                clid_to_clsname[c] = clsname
            #print 'output: ', output
            s += ' ' + clsname
            if clsname not in features_in_chain:
                features_in_chain.append(clsname)
            #print s
        # print s

re_single_obj_trigger = re.compile('[L2|EF]_([a-z]+)(\d+)(\w?)(_(loose|medium|tight)[a-z\d]?)?')
re_single_obj_trigger1 = re.compile('[L2|EF]_([a-z]+)(\d+)([a-z]?)$')
re_single_obj_trigger2 = re.compile('[L2|EF]_([a-z]+)(\d+)([a-z]?)_[^_]+$')

i = 0
for (fs, chains) in featureset_to_chain.iteritems():
    single_obj_trigger = False
    for c in chains:
        mg = re_single_obj_trigger1.search(c)
        if mg:
            print 'SINGLE OBJECT TRIGGER: %s (%s)' % (c, mg.groups())
            single_obj_trigger = True
        mg = re_single_obj_trigger2.search(c)
        if mg:
            print 'SINGLE OBJECT TRIGGER: %s (%s)' % (c, mg.groups())
            single_obj_trigger = True
    if single_obj_trigger:
        print '#----------------------------------------------------------'
        print '# Feature set %d' % i
        print '#----------------------------------------------------------'
        print '# Used by chains: ', chains
        for f in fs.split():
            print '  %s' % f
        i += 1
    
