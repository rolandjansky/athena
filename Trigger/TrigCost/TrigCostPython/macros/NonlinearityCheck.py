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
preamble="Usage: NonlinearityCheck.py rates.xml"
optparser = optparse.OptionParser(usage=preamble)

optparser.add_option('', '--L1fac', type="float", default=0.5,
                     dest='L1fac', help="Nonlinearity factor (default 0.5)")
optparser.add_option('', '--L2fac', type="float", default=0.5,
                     dest='L2fac', help="Nonlinearity factor (default 0.5)")
optparser.add_option('', '--EFfac', type="float", default=0.5,
                     dest='EFfac', help="Nonlinearity factor (default 0.5)")

(opts, args) = optparser.parse_args()

if len(args) < 1:
    print "No input file given"
    sys.exit(-1)
infile = args[0]

#
# Read in rates from XML
#   rates will a "CostResult" object which lists all the chains
rates = TrigCostXML.ReadXmlFile(infile)

# L1 just at L1scale*all XE chains
L1scale = opts.L1fac
L1rate = rates.GetChain("L1_L1A").GetRate()
L1rateCor = L1rate
for ch in rates.GetL1Chains():
    if string.count(ch.GetName(),"XE"):
        L1rateCor += L1scale*ch.GetRate()
        
# L2 just at L2scale*MET group rate
L2scale = opts.L2fac
L2rate = rates.GetChain("L2_total").GetRate()
L2rateCor = L2rate
for ch in rates.GetL2Chains():
    if string.count(ch.GetName(),"L2_RATE:MET"):
        L2rateCor += L2scale*ch.GetRate()



# EF just at EFscale*MET group rate
EFscale = opts.EFfac
EFrate = rates.GetChain("EF_total").GetRate()
EFrateCor = EFrate
for ch in rates.GetEFChains():
    if string.count(ch.GetName(),"EF_RATE:MET"):
        EFrateCor += EFscale*ch.GetRate()

print "Lvl   Nominal   Corrected"
print "--------------------------"
print "L1   %8.1f  %8.1f" % (L1rate,L1rateCor)
print "L2   %8.1f  %8.1f" % (L2rate,L2rateCor)
print "EF   %8.1f  %8.1f" % (EFrate,EFrateCor)



