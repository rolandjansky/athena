# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# 
# python plot_Perf_Eff_photons.py -b path_to_file.root
#

from sys import argv

from ROOT import TFile, TH1F
from ROOT import TCanvas, TLegend
from ROOT import SetOwnership

from probelist import *

filename = argv[2]
print "Looking for file : ",filename
inputfile = TFile(filename)

# probeListLowMidPtSupportingTriggers
# probeListLowMidPtPhysicsTriggers
# probeListHighPtSupportingTriggers
# probeListHighPtPhysicsTriggers
# probeListPhotonTriggers

if "LowMidPtSupportingTriggers" in filename:
	probelist = probeListLowMidPtSupportingTriggers
elif "LowMidPtPhysicsTriggers" in filename:
	probelist = probeListLowMidPtPhysicsTriggers
elif "HighPtSupportingTriggers" in filename:
	probelist = probeListHighPtSupportingTriggers
elif "HighPtPhysicsTriggers" in filename:
	probelist = probeListHighPtPhysicsTriggers
elif "ggH" in filename:
	probelist = probeListPhotonTriggers
else:
	print "Using the default list of triggers. Set another one if neeeded"
	probelist = default # or yours

def produceTH1FPerf(triggerName, leveltype):
	histEffEt  = TH1F()
	histEffEta = TH1F()
	histEffPhi = TH1F()
	histEffEt  = inputfile.Get("NavZeeTPPerf/"+triggerName+"/"+leveltype+"/eff_et")
	histEffEta = inputfile.Get("NavZeeTPPerf/"+triggerName+"/"+leveltype+"/eff_eta")
	histEffPhi = inputfile.Get("NavZeeTPPerf/"+triggerName+"/"+leveltype+"/eff_phi")
	return histEffEt, histEffEta, histEffPhi

def produceCanvas(triggerName):

	level = "HLTEfficiencies"

	## last row of histograms for Perf
	histEff_Et  = TH1F()
	histEff_Eta = TH1F()
	histEff_Phi = TH1F()
	histEff_Et, histEff_Eta, histEff_Phi = produceTH1FPerf(triggerName, level)

	histEff_Et.GetYaxis().SetRangeUser(0.5,1)
	histEff_Eta.GetYaxis().SetRangeUser(0.5,1)
	histEff_Phi.GetYaxis().SetRangeUser(0.5,1)

	histEff_Et.SetStats(False)
	histEff_Eta.SetStats(False)
	histEff_Phi.SetStats(False)

	histEff_Et.SetLineColor(2)
	histEff_Eta.SetLineColor(2)
	histEff_Phi.SetLineColor(2)
	####################
	# Set different marker styles
	####################
	histEff_Et.SetMarkerStyle(5)
	histEff_Eta.SetMarkerStyle(5)
	histEff_Phi.SetMarkerStyle(5)
	####################
	# Set different marker sizes
	####################
	histEff_Et.SetMarkerSize(0.3)
	histEff_Eta.SetMarkerSize(0.3)
	histEff_Phi.SetMarkerSize(0.3)
	####################
	# Set different marker colors, get them from line colors
	############Color(.GetLineColor())
	histEff_Et.SetMarkerColor(histEff_Et.GetLineColor())
	histEff_Eta.SetMarkerColor(histEff_Eta.GetLineColor())
	histEff_Phi.SetMarkerColor(histEff_Phi.GetLineColor())
	####################

	canv = TCanvas("canv__", "photons", 1800, 600);
	canv.Divide(3,1)

	leg1 = TLegend(0.3,0.2,0.75,0.30, triggerName)
	leg1.SetBorderSize(0)
	leg1.SetTextSize(0.05)
	leg1.AddEntry(histEff_Et, "Perf - HLTEfficiencies","l")
	SetOwnership( leg1, 0 )   # 0 = release (not keep), 1 = keep

	canv.cd(1)
	histEff_Et.Draw()
	leg1.Draw()

	canv.cd(2)
	histEff_Eta.Draw()

	canv.cd(3)
	histEff_Phi.Draw()

	return canv

##### Full Trigger name list
triggerName_list = probelist

for triggerName in triggerName_list:
	print "Looking for trigger : ", triggerName
	canv = TCanvas("canv", "photons", 1800, 600);
	canv = produceCanvas(triggerName)
	canv.Print("plots/plot_Eff_"+triggerName+".pdf")
