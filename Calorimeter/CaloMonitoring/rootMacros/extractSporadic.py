#!/usr/bin env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ======================================================================
def printProperties(h,q,hLB):

   print "=======================Harware coordinates :",h.GetName()[0:h.GetName().find("Phi")-1]
   print "Coordinates : Eta = ",h.GetName()[h.GetName().find("Eta")+3:h.GetName().find("Eta")+7]," Phi = ",h.GetName()[h.GetName().find("Phi")+3:h.GetName().find("Phi")+7]
   
   
   # Nb of events with E>20GeV and mean energy
   nbEvts20GeV = 0
   meanE = 0
   for ix in range(61,h.GetNbinsX()+2):# Bin 61 = 20GeV
      nbEvts20GeV = nbEvts20GeV+h.GetBinContent(ix)
      meanE = meanE + h.GetBinContent(ix)*h.GetBinCenter(ix)
   meanE = meanE / nbEvts20GeV/ 1000

   # Nb of events with q factor > 4000
   nbEvts4000 = 0
   for ix in range(11,q.GetNbinsX()+2):# Bin 11 = 4000
      nbEvts4000 = nbEvts4000 + q.GetBinContent(ix)

   # Nb of LBs where at least one event with E>20GeV is found
   nbLB20GeV = 0
   allLBs = ""
   for ix in range(1,hLB.GetNbinsX()+1):
      lbAff = False
      for iy in range(61,hLB.GetNbinsY()+2):# Bin 61 = 20GeV
         if hLB.GetBinContent(ix,iy) != 0:
            lbAff = True
      if lbAff:
         nbLB20GeV = nbLB20GeV+1
         allLBs = allLBs + " %d"%ix

   print "# of events: E>20GeV / E>20GeV && q>4000 : %d / %d"%(nbEvts4000,nbEvts20GeV)
   print "Mean energy above 20geV: %.2f GeV"%meanE
   print nbLB20GeV," LBs contains energetic events: ", allLBs 
   return

# ======================================================================
def displayHistos(h,q,hLB,canvas):
   canvas.Divide(1,2)
   canvas.cd(1)
   ROOT.gPad.Divide(2,1)
   ROOT.gPad.cd(1)
   ROOT.gPad.SetLogy()
   gStyle.SetOptStat(100110)
   h.Draw()

   canvas.cd(1)
   ROOT.gPad.cd(2)
   q.Draw()

   canvas.cd(2)
   hLB.SetStats(0)
   hLB.Draw("COLZ")

   canvas.cd()
   return


# Main =================================================================
import os, sys  
import string

if len(sys.argv)<4:
   print "python -i extractSporadic.py 159041 x29_m545 EMBA [FT29Sl2Ch68]"
   print "If no channel is specified, displays all with more than 20 events above 20 GeV"
   sys.exit()

#os.system("nsls /castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_7TeV/physics_CosmicCalo/0"+sys.argv[1])

import ROOT
from ROOT import gROOT, gDirectory
from ROOT import gStyle, TCanvas, TString, TPad
from ROOT import TFile, TTree, TRFIOFile, TStyle
from ROOT import TH1F,TH2F,TBrowser,TPaveStats
from ROOT import TPaveText

gROOT.Reset()
gStyle.SetPalette(1)

nameFile = "/castor/cern.ch/grid/atlas/tzero/prod1/perm/data10_7TeV/physics_CosmicCalo/0"+sys.argv[1]+"/data10_7TeV.00"+sys.argv[1]+".physics_CosmicCalo.merge.HIST."+sys.argv[2]+"/data10_7TeV.00"+sys.argv[1]+".physics_CosmicCalo.merge.HIST."+sys.argv[2]+"._0001.1"
nameDir = "run_"+sys.argv[1]+"/CaloMonitoring/LArCellMon_NoTrigSel/Sporadic20GeV/"+sys.argv[3]

myFile = TRFIOFile(nameFile)

# General numbers
hNbEvts = myFile.Get("run_"+sys.argv[1]+"/LAr/FEBMon/perPartitionData/Eventtype")
print "This stream contains %d events"%hNbEvts.GetEntries()

myFile.cd(nameDir)

# Compile all channels 
if len(sys.argv) == 4:
   h=[]
   q=[]
   hLB=[]
   listOfKeys = gDirectory.GetListOfKeys()
   for key in listOfKeys:
      name = key.GetName()
      type = key.GetClassName()
      if type == "TH1F" and name.find("EN") != -1 :
         h.append(myFile.Get(nameDir+"/"+name))
      if type == "TH1F" and name.find("Quality") != -1 :
         q.append(myFile.Get(nameDir+"/"+name))
      if type == "TH2F" and name.find("ENLB") != -1 :
         hLB.append(myFile.Get(nameDir+"/"+name))

   nhists = len(h)
   print "retrieved %i histos"%nhists

   for i in range(0, nhists):
      if h[i].Integral(61,h[i].GetNbinsX()+1) >= 20:
         printProperties(h[i],q[i],hLB[i])

# Extract the corresponding channel
if len(sys.argv) == 5:
   nameHisto = sys.argv[4]
   listOfKeys = gDirectory.GetListOfKeys()
   for key in listOfKeys:
      name = key.GetName()
      type = key.GetClassName()
      if type == "TH1F" and name.find("EN") != -1 and name.find(nameHisto) != -1:
         h = myFile.Get(nameDir+"/"+name)
      if type == "TH2F" and name.find("ENLB") != -1 and name.find(nameHisto) != -1:
         hLB = myFile.Get(nameDir+"/"+name)
      if type == "TH1F" and name.find("Quality") != -1 and name.find(nameHisto) != -1:
         q = myFile.Get(nameDir+"/"+name)

   if (h != 0):
      printProperties(h,q,hLB)
      c1 = TCanvas('c1','c1',1000,600)
      displayHistos(h,q,hLB,c1)


