#!/usr/bin/env python
import sys, os, glob
import ROOT

import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--infile', type=str, help='input HIST file')
parser.add_argument('--outfile', type=str, help='output ROOT file')
parser.add_argument('--mulow', type=int, help='lowest mu (default 1)', default=1)
parser.add_argument('--muhigh', type=int, help='highest mu (default 101)', default = 101)

args = parser.parse_args()
infilename = args.infile
mulow = args.mulow
muhigh = args.muhigh

infile = ROOT.TFile.Open(infilename, 'READ')

for k in infile.GetListOfKeys():
    if k.GetName().startswith('run_'):
        runname = k.GetName()
        break
if not runname:
    print 'Script Failure: Cannot find run directory in input file'
    sys.exit(1)

if not infile.Get('%s/GLOBAL/DQTDataFlow/m_sumweights' % (runname)):
    print 'Script Failure: Not meant for data!'    
    sys.exit(1)
if mulow > muhigh:
    print 'Script Failure: mulow must be smaller than muhigh!'
    sys.exit(1)

outfile = ROOT.TFile(args.outfile if args.outfile else '%s_out.root' % runname, 'RECREATE')
outfile.cd()
outfile.mkdir('%s/GLOBAL/DQTGlobalWZFinder' % (runname))
outfile.cd('%s/GLOBAL/DQTGlobalWZFinder' % (runname))
h = infile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_Z_Counter_mu' % (runname))
h.Write()
h = infile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_fiducialSumWeights_mu' % (runname))
h.Write()
h = infile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_Z_Counter_el' % (runname))
h.Write()
h = infile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_fiducialSumWeights_el' % (runname))
h.Write()
h = infile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_totalSumWeights' % (runname))
h.Write()


outfile.mkdir('%s/GLOBAL/DQTDataFlow' % (runname))
outfile.cd('%s/GLOBAL/DQTDataFlow' % (runname))
h = infile.Get('%s/GLOBAL/DQTDataFlow/m_sumweights' % (runname))
h.Write()

histlist = ['mutrigtp_matches', 'muloosetp_match_os', 'muloosetp_match_ss', 'muloosetp_nomatch_os', 'muloosetp_nomatch_ss', 'mcmatch', 'ele_tight_bad_os', 'ele_tight_bad_ss', 'ele_tight_good_os', 'ele_tight_good_ss', 'eltrigtp_matches']

for i in range (mulow,muhigh+1):
    outfile.cd()
    outfile.mkdir('%s/lb_%s/GLOBAL/DQTGlobalWZFinder' % (runname,i)) 
    outfile.cd('%s/lb_%s/GLOBAL/DQTGlobalWZFinder' % (runname,i))
    for hist in histlist:
        h = infile.Get('%s/GLOBAL/DQTGlobalWZFinder/m_mc_%s' % (runname,hist))
        h2 = h.ProjectionY("bin2",h.GetXaxis().FindBin(i),h.GetXaxis().FindBin(i))
        h2.Write('m_%s' % (hist) ) 

print 'Script Success! Output saved as %s' % (args.outfile if args.outfile else '%s_out.root' % runname)
