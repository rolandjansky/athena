#!/usr/bin/env python
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

# Basic ROOT setup
import ROOT,sys,math,glob
ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kError

# Check for the right input files
if len(sys.argv)>1:
    inputfilenames = []
    for a in sys.argv[1].split(','):
        inputfilenames+=glob.glob(a)
else:
    print ('Please specify an input LHE file (or comma-separated lists of them)')
    sys.exit(1)

# Set up histograms
# Energy and PDG ID of incoming partons
e_init_0 = ROOT.TH1D('e_init_0','',100,0,2000)
e_init_1 = ROOT.TH1D('e_init_1','',100,0,2000)
pdg_init = ROOT.TH1D('pdg_init','',25,0,25)
# Hard outgoing partons, just plot pTs
pt_hard_0 = ROOT.TH1D('pt_hard_0','',100,0,1000)
pt_hard_1 = ROOT.TH1D('pt_hard_1','',100,0,1000)
# Generally after the first two, we're into extra partons
pt_extra_0 = ROOT.TH1D('pt_extra_0','',100,0,1000)
pt_extra_1 = ROOT.TH1D('pt_extra_1','',100,0,1000)
pt_extra_2 = ROOT.TH1D('pt_extra_2','',100,0,1000)
pt_extra_3 = ROOT.TH1D('pt_extra_3','',100,0,1000)
pt_extra_4 = ROOT.TH1D('pt_extra_4','',100,0,1000)
pt_extra_5 = ROOT.TH1D('pt_extra_5','',100,0,1000)
pdg_extras = ROOT.TH1D('pdg_extras','',100,0,1000)
# And LHE multiplicity
multip = ROOT.TH1D('mult','',20,0,20)
weights = ROOT.TH1D('weights','',100,0,1000)
# Array of all the histograms for later writing
histograms = [e_init_0,e_init_1,pdg_init,pt_hard_0,pt_hard_1,pt_extra_0,pt_extra_1,pt_extra_2,pt_extra_3,pt_extra_4,pt_extra_5,pdg_extras,multip,weights]

# Now loop through input file
for inputfilename in inputfilenames:
    # LHE format documented in https://arxiv.org/pdf/1405.1067.pdf
    with open(inputfilename,'r') as inputfile:
        event = False
        npartons = -1
        extras = 0
        ep_pt = 0.
        for line in inputfile:
            # Check for the ktdurham cut
            if 'ktdurham' in line and '=' in line:
                # Print the matching cut for info
                print ('Matching cut:',float(line.split()[0]))
                continue
            # Check for a comment
            if len(line.split('#')[0].strip())==0:
                continue
            # Check if we have entered an event
            if not event and '<event>' not in line:
                continue
            # Check if we are just starting an event
            if not event and '<event>' in line:
                event = True
                continue
            # Check if we have finished and are doing something else
            if '<' in line or '>' in line:
                event = False
                npartons = -1
                continue
            # Check to parse the first line of the event
            if npartons<0:
                npartons = int(line.split()[0])
                multip.Fill(npartons-2) # Final state multiplicity
                weight = float(line.split()[2])
                weights.Fill(weight)
                extras = npartons-4
                continue
            # Deal with the inital state partons first
            if npartons>extras+2:
                if npartons>extras+3:
                    e_init_0.Fill( float(line.split()[9]) )
                else:
                    e_init_1.Fill( float(line.split()[9]) )
                pdg_init.Fill( abs(int(line.split()[0])) )
                npartons -= 1
                continue
            momentum_x = float(line.split()[6])
            momentum_y = float(line.split()[7])
            pt = math.sqrt(momentum_x*momentum_x+momentum_y*momentum_y)
            # Now deal with hard scatter partons
            if npartons>extras:
                if npartons>extras+1:
                    pt_hard_0.Fill(pt)
                else:
                    pt_hard_1.Fill(pt)
                npartons -= 1
                continue
            # Now we are into the extras
            if extras==npartons:
                pt_extra_0.Fill(pt)
            elif extras-1==npartons:
                pt_extra_1.Fill(pt)
            elif extras-2==npartons:
                pt_extra_2.Fill(pt)
            elif extras-3==npartons:
                pt_extra_3.Fill(pt)
            elif extras-4==npartons:
                pt_extra_4.Fill(pt)
            elif extras-5==npartons:
                pt_extra_5.Fill(pt)
            pdg_extras.Fill( abs(int(line.split()[0])) )
            npartons-=1
        # End of loop over input file lines
# Done holding open LHE file

# Write all the histograms for future use
output_hists = ROOT.TFile.Open('output_hists.root','RECREATE')
output_hists.cd()
for h in histograms:
    h.Write(h.GetName())
output_hists.Close()
