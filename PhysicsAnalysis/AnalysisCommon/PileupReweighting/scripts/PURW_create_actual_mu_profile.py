#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import argparse, sys, os, array

parser = argparse.ArgumentParser(description='Make a BCID-aware pileup reweighting file')
parser.add_argument('--grl', type=str,
                    help='GRL to use (mandatory)')
parser.add_argument('--bcidfile', type=str,
                    help='Input BCID lumi ROOT file (not from iLumiCalc!) (mandatory) For example, /afs/cern.ch/atlas/www/GROUPS/DATAPREPARATION/InteractionsperCrossing/data17_13TeV-OflLumi-13TeV-010.root')
                    
parser.add_argument('--outfile', type=str,
                    help='Output file for PURW tool (default: purw.root)',
                    default='purw.root')

args = parser.parse_args()

if not args.bcidfile:
    print 'ERROR: BCID File must be specified\n'
    parser.print_help()
    sys.exit(2)

bcidfile = ROOT.TFile.Open(args.bcidfile)
if not bcidfile:
    print 'ERROR: cannot open the BCID lumi file', args.bcidfile
    print 'Exiting'
    sys.exit(1)

grl = None
if not args.grl:
    print 'ERROR: Must specify GRL file\n'
    parser.print_help()
    sys.exit(2)

if not os.path.isfile(args.grl):
    print 'ERROR: Cannot find GRL file', args.grl
    print 'Exiting'
    sys.exit(1)

outfile = ROOT.TFile.Open(args.outfile, 'RECREATE')
metadata = ROOT.TTree('DataCustomReweighting', 'DataCustomReweighting')
cname_arr = array.array('c', 'pileup\0')
runno_arr = array.array('I', [0])
histname_arr = array.array('c', 'pileup_data_run_XXXXXX\0')
metadata.Branch('CustomName', cname_arr, 'CustomName/C')
metadata.Branch('RunNumber', runno_arr, 'RunNumber/i')
metadata.Branch('HistName', histname_arr, 'HistName/C')

lgp = ROOT.Root.TGoodRunsListReader(args.grl)
lgp.Interpret()
grl = lgp.GetMergedGoodRunsList()
for run in grl.GetGoodRuns():
    outhist=None
    runno = run.GetRunNumber()
    runno_arr[0] = runno
    histname_arr[-7:-1] = array.array('c', `runno`)
    rdir = bcidfile.Get(`runno`)
    if not rdir:
        print 'ERROR retrieving mu distributions for run %s, skipping ...' % runno
        continue
    #print rdir
    runhist = None
    for lbrange in run:
        #print runno, lbrange.Begin(), lbrange.End()
        for lb in xrange(lbrange.Begin(), lbrange.End()+1):
            inhist = rdir.Get('%srec' % lb)
            if not inhist:
                print 'ERROR retrieving mu distribution for run %s LB %s' % (runno, lb)
            else:
                if not runhist:
                    runhist = inhist.Clone('pileup_data_run_%s' % runno)
                else:
                    runhist.Add(inhist)
    outfile.WriteTObject(runhist)
    metadata.Fill()

outfile.WriteTObject(metadata)

#make some other metadata
lumitag = bcidfile.Get('LuminosityTag')
if not lumitag:
    print "Unfortunately, LuminosityTag object not found in BCID info file"
    print "This is not fatal, but reduces trackability of information."
else:
    outfile.WriteTObject(lumitag)

grlo = ROOT.TNamed("InputGRL", args.grl)
outfile.WriteTObject(grlo)

outfile.Close()

        


