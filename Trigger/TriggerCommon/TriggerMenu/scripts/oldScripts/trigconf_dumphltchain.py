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

def usage():
    print 'Usage: %s <HLT_XML_file> <chain_name>' % sys.argv[0]
    print '------'
    print 'Examples: %s hlt.xml EF_j10' % sys.argv[0]

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

if __name__ == '__main__':
    #
    if len(sys.argv)!=3:
        usage()
        sys.exit(1)
    hltxml = sys.argv[1]
    chain_name = sys.argv[2]
    #
    hltdoc = xml.dom.minidom.parse(hltxml)
    chain = findChainNode(hltdoc, chain_name)
    if chain!=None:
        sigs = getSignatureNodes(chain)
        isig = 1
        tes_created = ['']
        for s in sigs:
            isig = int(s.getAttribute('signature_counter'))
            print 'Signature %d' % isig
            tes = getTENodes(s)
            for te in tes:
                te_out = te.getAttribute('te_name')
                printTE(te_out, hltdoc, tes_created)
            isig += 1
