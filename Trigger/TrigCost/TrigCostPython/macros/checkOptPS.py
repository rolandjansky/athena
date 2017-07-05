#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from TrigCostPython import TrigCostAnalysis, TrigCostXML , TrigCostTRP
from optparse import OptionParser
import os, sys, string, math, shutil
import ROOT
ROOT.gROOT.SetBatch(True)

#-----------------------------------------------------------------------
#  Options definitions
#-----------------------------------------------------------------------
from optparse import OptionParser

usage='<rates xml>'
full_usage='checkOptPS.py '+usage

p = OptionParser(usage)
    

(opts, args) = p.parse_args()

if len(args) != 1:
    print "Must have exactly one argument"
    print full_usage
    sys.exit(-1)

#-----------------------------------------------------------------------
#  Read in XML file
#-----------------------------------------------------------------------
results = TrigCostXML.ReadXmlFile(args[0])

#-----------------------------------------------------------------------
#  Make L1 -> L2 and L2 -> EF maps
#-----------------------------------------------------------------------
daughter_map = {}
for ch in results.GetChains():

    if ch.GetLevel()=='L1':
        continue
    
    lower_chain = ''
    try:
        lower_chain = ch.GetLowerChain()
    except TrigCostAnalysis.VarNotAvailable,e:
        if not (TrigCostTRP.NonChainTRP(ch.GetName()) or 
                string.count(ch.GetName(),"_RATE:")==1 or 
                string.count(ch.GetName(),"LVL_")==1):
            print "Missing lower chain for ",ch.GetName()
        lower_chain = ''

    # exclude multiseeded
    if string.count(lower_chain,","):
        continue

    if lower_chain!='':
        ch_daughters = daughter_map.get(lower_chain,[])
        ch_daughters.append(ch)
        daughter_map[lower_chain]=ch_daughters
             
#-----------------------------------------------------------------------
#  Check for non-optimal prescales
#-----------------------------------------------------------------------
print
for (parent,daughters) in daughter_map.items():

    # skip deactivated chains
    if results.GetChain(parent).GetPrescale() == -1:
        continue
    
    allPsGtOne=True
    for daughter in daughters:
        if daughter.GetPrescale()==1:
            allPsGtOne=False

    if allPsGtOne:
        print "Non-optimal chain: %-32s    PS=%7.1f   PT=%7.1f" % (parent,results.GetChain(parent).GetPrescale(),results.GetChain(parent).GetPassthrough())
        print "--------------------------------------------------------------------------------"
        for daughter in daughters:
            print "   Daughter  %-38s    PS=%7.1f   PT=%7.1f" % (daughter.GetName(),daughter.GetPrescale(),daughter.GetPassthrough())
        print
        
