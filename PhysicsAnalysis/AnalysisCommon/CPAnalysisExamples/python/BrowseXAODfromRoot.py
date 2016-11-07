#! /usr/bin/evn python 

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import os, sys 

if len(sys.argv) < 2:
    print "USAGE: " + sys.argv[0] + " [filename]"
    sys.exit(-1)

filename = sys.argv[1]

# check if the file is there 

if not os.path.isfile(filename):
    print "Sorry, no file named " + filename + ", gently exiting..."
    sys.exit(-1)

#Load the RootCore libraries

ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C");

# Initialize root to use xAOD

ROOT.xAOD.Init()

f = ROOT.TFile.Open(filename,"read"); # Just an example xAOD file
  
if not f.IsOpen():
    print "Can't open " + filename + ", exiting"

# Get a tree from the xAOD file 

thetree = ROOT.xAOD.MakeTransientTree(f); 

# Ready to play with the tree 

C1 = ROOT.TCanvas()
C1.cd();
#Draw eta of the full jet container

h_jetEta = ROOT.TH1F("h_jetEta","eta of all jets in the container",100,-5,5)

thetree.Draw("AntiKt4LCTopoJets.eta() >> h_jetEta");

# Now make a loop. Compute and plot the eta separation between the two leading jets
# Also, save the histogram in a file 

outfile = ROOT.TFile("output.root","recreate")

h_deltaEta = ROOT.TH1F("h_deltaEta","Eta separation between leading jets",10,0,5);

totalEntries = thetree.GetEntries()

print ("There are %g events in this tree" % totalEntries)

for ev in thetree:

    print ("There are %i jets in the jet container" % len(ev.AntiKt4LCTopoJets))

    # count the number of central jets with pt > 20 GeV

    nj = 0

    for jet in ev.AntiKt4LCTopoJets:
        if jet.pt() > 20000 and ROOT.TMath.Abs(jet.eta()) < 2.5:
            nj = nj + 1

    if nj >= 2:
        h_deltaEta.Fill(ev.AntiKt4LCTopoJets[0].eta() - ev.AntiKt4LCTopoJets[1].eta())

outfile.cd()
h_jetEta.Write()
h_deltaEta.Write()
outfile.Close()
