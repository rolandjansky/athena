#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,commands
import sys
import re
from ROOT import TTree, TFile, AddressOf, gROOT
from array import array
from TrigCostPython import TrigCostAnalysis,TrigCostCool

###########################################
# How to run this code:
#
# For data:
#   A pickle file can be created if desired. This way the mu values do not repeatable need
#   to be looked up in COOL
#
#   > python DumpMuXML.py --input runs.txt -o mu.xml
#
# 
# 
#
#
###########################################

#
# Parse command line options
#   
from optparse import OptionParser
p = OptionParser()
p.add_option( '-i', '--input',  type="string",       default="runs.txt",   dest="input",  help="input file with a comma separated list of input runs")
p.add_option( '-o', '--output', type="string",       default="mu.xml",     dest="output", help="xml with run and mu")

p.add_option('--test',          action="store_true", default=False,        dest='test',   help="Test only one run")
p.add_option('--writeTree',     action="store_true", default=False,        dest='writeT', help="Write root tree")
p.add_option('-d', '--debug',   action="store_true", default=False,                       help="Run in debug mode")
p.add_option('-q', '--quiet',   action="store_true", default=False,                       help="Run in quiet mode (suppresses chains with zero rates)")

(options, args) = p.parse_args()

def WriteXML(run,lbs):

    line='<Run runnr=%s>\n' %run
    #for lbs in lbList:
    if True:
        for i in range(lbs.FirstLB(),lbs.LastLB()):
            if False:
                print 'muRead:',lbs.GetMeanPileUp(i)
                sys.stdout.flush()
                
            mu=lbs.GetMeanPileUp(i)

            line += '       <lb=%s mu=%s />\n' %(i,mu)

    line += '</Run>\n'

    return line

def WriteTree(runSet):

    print 'Writing tree'

    # Make a tree
    froot = TFile('mu.root','RECREATE')
    tree  = TTree("mu", "Flat ntuple for mu");
    tree.SetDirectory(froot);

    leaves = "run/F:lb/F:mu/F"

    leafValues = array("f", [0.0,0.0,0.0])

    newBranch = tree.Branch( "mu_vars" , leafValues, leaves )

    for r,lbs in runSet.iteritems():
        for i in range(lbs.FirstLB(),lbs.LastLB()):
            leafValues[2] = lbs.GetMeanPileUp(i)
            leafValues[0] = float(r)
            leafValues[1] = i
            
            print 'mu: %s run: %s lb: %s' %(leafValues[2],leafValues[0],leafValues[1])
            
            tree.Fill();

    tree.Write()
    #froot.Write()
    froot.Close()

if __name__=="__main__":

    # Set options
    opts=[""]
    if options.debug: opts.append('debug')
    if options.quiet: opts.append('quiet')

    lbSet =[]
    lbList=[]
    runs = []
    
    iFile = open(options.input, 'read')
    oFile = open(options.output,'write')

    for l in iFile:
        runs+=l.split(',')

    iFile.close()
    print 'Input runs: ',runs
    
    # map runs to LBs with mu data
    r=0
    for run in runs:
        
        if options.test and r>2:
            continue

        print 'run:',run
        lbList = TrigCostCool.GetLumiblocks(int(run),1,1500,opts);
        if options.writeT:
            lbSet.append(lbList)
        
        xmlLines = WriteXML(run,lbList)
        oFile.write(xmlLines)
        r+=1
    oFile.close()

    if options.writeT:
        WriteTree(dict(zip(runs,lbSet)))

    print 'Please find the output xml here: ',options.output
    print 'Done'

        

    
