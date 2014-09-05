#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# 
# Checks if given triggers are in an allowed range for an XML input
# 

import optparse
import string
import xml.dom.minidom
import sys
from TrigCostPython import TrigCostAnalysis,TrigCostTRP,TrigCostCool,TrigCostXML

#
# Inputs
#
preamble="Usage: rangeCheck.py --ranges range.xml rates.xml"
optparser = optparse.OptionParser(usage=preamble)

optparser.add_option('', '--ranges',   type="string",       default="",
                     dest="ranges",      help="XML file with ranges name")

(opts, args) = optparser.parse_args()

if len(args) < 1:
    print "No input file given"
    sys.exit(-1)
infile = args[0]

if opts.ranges=='':
    print "No range XML given"
    sys.exit(-1)

#
# Range Definition, this is a python map from trigger name to allowed range
#

rngs = {}

dom = xml.dom.minidom.parse(open(opts.ranges))

rngsxml = dom.getElementsByTagName("ranges")
for rng in rngsxml[0].getElementsByTagName("range"):
    chain = rng.getAttribute("chain")
    minv  = string.atof(rng.getAttribute("min"))
    maxv  = string.atof(rng.getAttribute("max"))
    rngs[chain] = (minv,maxv)
    
#
# Read in rates from XML
#   rates will a "CostResult" object which lists all the chains
rates = TrigCostXML.ReadXmlFile(infile)

if rngs.has_key("DEFAULT"):
    #
    # Do default, loop over all chains
    #
    for lvl in ["L1","L2","EF"]:
        chnames = rates.GetChainNames(lvl)
        chnames.sort()
        for chname in chnames:
            # get range, fill in with default
            rng = rngs.get(chname,rngs["DEFAULT"])

            ch = rates.GetChain(chname)
            if ch.GetRate() < rng[0] or ch.GetRate() > rng[1]:
                print "The rate for %s is %8.2f  which is not in the range (%8.2f,%8.2f)" %\
                      (chname,ch.GetRate(),rng[0],rng[1])
                        
else:
    #
    # Do only listed chains, loop of ranges
    #
    for (chname,rng) in rngs.items():
        if not rates.HasChain(chname):
            print "Chain "+chname+" is not in rates file"
            print "Chains in file: ",rates.GetChainNames('L1')
            continue

        ch = rates.GetChain(chname)
        if ch.GetRate() < rng[0] or ch.GetRate() > rng[1]:
            print "The rate for %s is %8.2f  which is not in the range (%8.2f,%8.2f)" %\
                (chname,ch.GetRate(),rng[0],rng[1])



