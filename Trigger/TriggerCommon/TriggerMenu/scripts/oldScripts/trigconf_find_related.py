#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import xml.dom.minidom


def getSignatureNodes(chain_node):
    sigs = chain_node.getElementsByTagName('SIGNATURE')
    ret = []
    for s in sigs:
        ret.append(s)
    return ret

def getTENodes(sig_node):
    tes = sig_node.getElementsByTagName('TRIGGERELEMENT')
    ret = []
    for te in tes:
        ret.append(te)
    return ret

def findChainNode(doc, chain_name):
    chains = doc.getElementsByTagName('CHAIN')
    for c in chains:
        if c.getAttribute('chain_name') == chain_name:
            return c
    return None

def findSequenceNode(doc, te_name):
    seqs = doc.getElementsByTagName('SEQUENCE')
    for seq in seqs:
        if seq.getAttribute('output') == te_name:
            return seq
    return None

def readAllL1Items(doc):
    x = []
    items = doc.getElementsByTagName('TriggerItem')
    for item in items:
        x.append(item.getAttribute('name'))
    return x

def readAllChainsAtLevel(doc, level):
    x = []
    chains = doc.getElementsByTagName('CHAIN')
    for c in chains:
        if c.getAttribute('level') == level:
            x.append( (c.getAttribute('chain_name'),
                       c.getAttribute('lower_chain_name')))
    return x

def usage():
    print 'Usage: %s <LVL1_XML_file> <HLT_XML_file> <item/chain_name>' % sys.argv[0]
    print '------'
    print 'Examples: %s lvl1.xml hlt.xml EF_e10_loose' % sys.argv[0]
    print '          %s lvl1.xml hlt.xml L1_J10' % sys.argv[0]

def printTE(te_name, hltdoc, tes_created):
    te_out = te_name
    seq = findSequenceNode(hltdoc, te_out)
    if seq:
        tes_in = seq.getAttribute('input')
        if tes_in != '': tes_in = tes_in.split(' ')
        for te_in in tes_in:
            if te_in not in tes_created:
                printTE(te_in, hltdoc, tes_created)
                tes_created.append(te_in)
        #
        algos = seq.getAttribute('algorithm')
        if algos!='': algos = algos.split(' ')
        print '  TE %s <- %s' % (te_out, tes_in)
        for algo in algos:
            print '    %s' % algo
        tes_created.append(te_name)

def whichLevel(name, l1items, l2chains, efchains):
    r = ''
    for x in l1items:
        if name == x:
            r = 'L1'
            break
    for x in l2chains:
        if name == x[0]:
            r = 'L2'
            break
    for x in efchains:
        if name == x[0]:
            r = 'EF'
            break
    return r

def findRelatedForL1(name, l1items, l2chains, efchains):
    print ''
    print 'L1 item: %s' % name
    used_at_l2 = []
    used_at_ef = []
    for x in l2chains:
        if x[1] == name:
            used_at_l2.append(x[0])
    for x in efchains:
        if x[1] in used_at_l2:
            used_at_ef.append(x[0])
    print '* Used at L2 by: '
    for x in used_at_l2: print x
    print '* Used at EF by: '
    for x in used_at_ef: print x
    print ''

def findRelatedForL2(name, l1items, l2chains, efchains):
    print ''
    print 'L2 chain: %s' % name
    uses_at_l1 = []
    used_at_ef = []
    lower_name = ''
    for x in l2chains:
        if x[0] == name:
            lower_name = x[1]
            uses_at_l1.append(lower_name)
            break
    for x in efchains:
        if x[1] == name:
            used_at_ef.append(x[0])
    print '* Uses L1 item: '
    for x in uses_at_l1: print x
    print '* Used at EF by: '
    for x in used_at_ef: print x
    print ''

def findRelatedForEF(name, l1items, l2chains, efchains):
    print ''
    print 'EF chain: %s' % name
    uses_at_l1 = []
    uses_at_l2 = []
    lower_name = ''
    for x in efchains:
        if x[0] == name:
            lower_name = x[1]
            uses_at_l2.append(lower_name)
            break
    for x in l2chains:
        if x[0] in uses_at_l2:
            uses_at_l1.append(x[1])
    print '* Uses at L1 by: '
    for x in uses_at_l1: print x
    print '* Uses at L2 by: '
    for x in uses_at_l2: print x
    print ''

        
if __name__ == '__main__':
    #
    if len(sys.argv)!=4:
        usage()
        sys.exit(1)
    lvl1xml = sys.argv[1]
    hltxml = sys.argv[2]
    item_name = sys.argv[3]
    #
    lvl1doc = xml.dom.minidom.parse(lvl1xml)
    hltdoc = xml.dom.minidom.parse(hltxml)

    all_l1items = readAllL1Items(lvl1doc)
    all_l2chains = readAllChainsAtLevel(hltdoc, 'L2')
    all_efchains = readAllChainsAtLevel(hltdoc, 'EF')

    level = whichLevel(item_name, all_l1items, all_l2chains, all_efchains)
    if level == 'L1':
        findRelatedForL1(item_name, all_l1items, all_l2chains, all_efchains)
        pass
    elif level == 'L2':
        findRelatedForL2(item_name, all_l1items, all_l2chains, all_efchains)
        pass
    elif level == 'EF':
        findRelatedForEF(item_name, all_l1items, all_l2chains, all_efchains)
        pass
    else:
        print "L1 item or HLT chain name '%s' not found in the XML files" \
              % item_name
        pass
