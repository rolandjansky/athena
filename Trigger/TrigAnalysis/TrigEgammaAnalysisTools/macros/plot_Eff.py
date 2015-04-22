# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# 
# python plot_Eff.py -b path_to_file.root
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

def produceTH1F(triggerName, leveltype):
	histEffEt  = TH1F()
	histEffEta = TH1F()
	histEffPhi = TH1F()
	histEffEt  = inputfile.Get("NavZeeTPEff/"+triggerName+"/"+leveltype+"/ZeeProbeEffEt")
	histEffEta = inputfile.Get("NavZeeTPEff/"+triggerName+"/"+leveltype+"/ZeeProbeEffEta")
	histEffPhi = inputfile.Get("NavZeeTPEff/"+triggerName+"/"+leveltype+"/ZeeProbeEffPhi")
	return histEffEt, histEffEta, histEffPhi

def produceTH1FPerf(triggerName, leveltype):
	histEffEt  = TH1F()
	histEffEta = TH1F()
	histEffPhi = TH1F()
	histEffEt  = inputfile.Get("NavZeeTPPerf/"+triggerName+"/"+leveltype+"/eff_et")
	histEffEta = inputfile.Get("NavZeeTPPerf/"+triggerName+"/"+leveltype+"/eff_eta")
	histEffPhi = inputfile.Get("NavZeeTPPerf/"+triggerName+"/"+leveltype+"/eff_phi")
	return histEffEt, histEffEta, histEffPhi

def produceCanvas(triggerName):

	level_A = "L2Efficiencies"
	level_B = "HLTEfficiencies"
	level_C = "L1CaloEfficiencies"
	level_D = "L2CaloEfficiencies"
	level_E = "EFCaloEfficiencies"

	histEffEt_A  = TH1F()
	histEffEta_A = TH1F()
	histEffPhi_A = TH1F()
	histEffEt_B  = TH1F()
	histEffEta_B = TH1F()
	histEffPhi_B = TH1F()
	histEffEt_C  = TH1F()
	histEffEta_C = TH1F()
	histEffPhi_C = TH1F()
	histEffEt_D  = TH1F()
	histEffEta_D = TH1F()
	histEffPhi_D = TH1F()
	histEffEt_E  = TH1F()
	histEffEta_E = TH1F()
	histEffPhi_E = TH1F()

	## last row of histograms for Perf
	histEff_Et  = TH1F()
	histEff_Eta = TH1F()
	histEff_Phi = TH1F()
	histEff_Et, histEff_Eta, histEff_Phi = produceTH1FPerf(triggerName, level_B) # level_B is HLT

	histEffEt_A, histEffEta_A, histEffPhi_A = produceTH1F(triggerName, level_A)
	histEffEt_B, histEffEta_B, histEffPhi_B = produceTH1F(triggerName, level_B)
	histEffEt_C, histEffEta_C, histEffPhi_C = produceTH1F(triggerName, level_C)
	histEffEt_D, histEffEta_D, histEffPhi_D = produceTH1F(triggerName, level_D)
	histEffEt_E, histEffEta_E, histEffPhi_E = produceTH1F(triggerName, level_E)

	histEffEt_A.SetStats(False)
	histEffEta_A.SetStats(False)
	histEffPhi_A.SetStats(False)
	histEffEt_B.SetStats(False)
	histEffEta_B.SetStats(False)
	histEffPhi_B.SetStats(False)
	histEffEt_C.SetStats(False)
	histEffEta_C.SetStats(False)
	histEffPhi_C.SetStats(False)
	histEffEt_D.SetStats(False)
	histEffEta_D.SetStats(False)
	histEffPhi_D.SetStats(False)
	histEffEt_E.SetStats(False)
	histEffEta_E.SetStats(False)
	histEffPhi_E.SetStats(False)

	histEffEt_A.SetLineColor(1)
	histEffEta_A.SetLineColor(1)
	histEffPhi_A.SetLineColor(1)
	histEffEt_B.SetLineColor(2)
	histEffEta_B.SetLineColor(2)
	histEffPhi_B.SetLineColor(2)
	histEffEt_C.SetLineColor(1)
	histEffEta_C.SetLineColor(1)
	histEffPhi_C.SetLineColor(1)
	histEffEt_E.SetLineColor(2)
	histEffEta_E.SetLineColor(2)
	histEffPhi_E.SetLineColor(2)
	histEffEt_D.SetLineColor(4)
	histEffEta_D.SetLineColor(4)
	histEffPhi_D.SetLineColor(4)

	histEff_Et.SetLineColor(1)
	histEff_Eta.SetLineColor(1)
	histEff_Phi.SetLineColor(1)

	# Set different marker styles
	####################
	histEffEt_A.SetMarkerStyle(4)
	histEffEta_A.SetMarkerStyle(4)
	histEffPhi_A.SetMarkerStyle(4)
	histEffEt_B.SetMarkerStyle(2)
	histEffEta_B.SetMarkerStyle(2)
	histEffPhi_B.SetMarkerStyle(2)
	####################
	histEffEt_C.SetMarkerStyle(4)
	histEffEta_C.SetMarkerStyle(4)
	histEffPhi_C.SetMarkerStyle(4)
	histEffEt_D.SetMarkerStyle(2)
	histEffEta_D.SetMarkerStyle(2)
	histEffPhi_D.SetMarkerStyle(2)
	histEffEt_E.SetMarkerStyle(5)
	histEffEta_E.SetMarkerStyle(5)
	histEffPhi_E.SetMarkerStyle(5)
	####################
	histEff_Et.SetMarkerStyle(5)
	histEff_Eta.SetMarkerStyle(5)
	histEff_Phi.SetMarkerStyle(5)
	####################
	# Set different marker sizes
	####################
	histEffEt_A.SetMarkerSize(0.3)
	histEffEta_A.SetMarkerSize(0.3)
	histEffPhi_A.SetMarkerSize(0.3)
	histEffEt_B.SetMarkerSize(0.3)
	histEffEta_B.SetMarkerSize(0.3)
	histEffPhi_B.SetMarkerSize(0.3)
	############Size(0.3)
	histEffEt_C.SetMarkerSize(0.3)
	histEffEta_C.SetMarkerSize(0.3)
	histEffPhi_C.SetMarkerSize(0.3)
	histEffEt_D.SetMarkerSize(0.3)
	histEffEta_D.SetMarkerSize(0.3)
	histEffPhi_D.SetMarkerSize(0.3)
	histEffEt_E.SetMarkerSize(0.3)
	histEffEta_E.SetMarkerSize(0.3)
	histEffPhi_E.SetMarkerSize(0.3)
	############Size(0.3)
	histEff_Et.SetMarkerSize(0.3)
	histEff_Eta.SetMarkerSize(0.3)
	histEff_Phi.SetMarkerSize(0.3)
	# Set different marker colors, get them from line colors
	####################
	histEffEt_A.SetMarkerColor(histEffEt_A.GetLineColor())
	histEffEta_A.SetMarkerColor(histEffEta_A.GetLineColor())
	histEffPhi_A.SetMarkerColor(histEffPhi_A.GetLineColor())
	histEffEt_B.SetMarkerColor(histEffEt_B.GetLineColor())
	histEffEta_B.SetMarkerColor(histEffEta_B.GetLineColor())
	histEffPhi_B.SetMarkerColor(histEffPhi_B.GetLineColor())
	############Color(.GetLineColor())
	histEffEt_C.SetMarkerColor(histEffEt_C.GetLineColor())
	histEffEta_C.SetMarkerColor(histEffEta_C.GetLineColor())
	histEffPhi_C.SetMarkerColor(histEffPhi_C.GetLineColor())
	histEffEt_D.SetMarkerColor(histEffEt_D.GetLineColor())
	histEffEta_D.SetMarkerColor(histEffEta_D.GetLineColor())
	histEffPhi_D.SetMarkerColor(histEffPhi_D.GetLineColor())
	histEffEt_E.SetMarkerColor(histEffEt_E.GetLineColor())
	histEffEta_E.SetMarkerColor(histEffEta_E.GetLineColor())
	histEffPhi_E.SetMarkerColor(histEffPhi_E.GetLineColor())
	############Color(.GetLineColor())
	histEff_Et.SetMarkerColor(histEff_Et.GetLineColor())
	histEff_Eta.SetMarkerColor(histEff_Eta.GetLineColor())
	histEff_Phi.SetMarkerColor(histEff_Phi.GetLineColor())
	####################
	## some customization to the range of phi effs

	histEffEt_C.GetYaxis().SetRangeUser(0.5,1)
	histEffEta_C.GetYaxis().SetRangeUser(0.5,1)
	histEffPhi_C.GetYaxis().SetRangeUser(0.5,1)


	histEffPhi_A.GetYaxis().SetRangeUser(0.3, 1)
	histEffPhi_C.GetYaxis().SetRangeUser(0.3, 1)
	histEffPhi_A.GetYaxis().SetRangeUser(0.3, 1)
	histEffPhi_B.GetYaxis().SetRangeUser(0.3, 1)
	##########################

	canv = TCanvas()
	canv.Divide(3,3)

	leg1 = TLegend(0.4,0.2,0.75,0.40, triggerName)
	leg2 = TLegend(0.4,0.2,0.75,0.50, triggerName)
	leg3 = TLegend(0.4,0.2,0.75,0.40, triggerName)
	#leg4 = TLegend(0.4,0.2,0.95,0.30) # for the trigger name
	leg1.SetBorderSize(0)
	leg2.SetBorderSize(0)
	leg3.SetBorderSize(0)
	#leg4.SetBorderSize(0)
	leg1.SetTextSize(0.05)
	leg2.SetTextSize(0.05)
	leg3.SetTextSize(0.05)
	#leg4.SetTextSize(0.05)

	leg1.AddEntry(histEffEt_A, "L2Efficiencies","l")
	leg1.AddEntry(histEffEt_B, "HLTEfficiencies","l")
	
	leg2.AddEntry(histEffEt_C, "L1CaloEfficiencies","l")
	leg2.AddEntry(histEffEt_D, "L2CaloEfficiencies","l")
	leg2.AddEntry(histEffEt_E, "EFCaloEfficiencies","l")

	leg3.AddEntry(histEffEt_B, "Eff - HLTEfficiencies","l")
	leg3.AddEntry(histEff_Et, "Perf - HLTEfficiencies","l")

	SetOwnership( leg1, 0 )   # 0 = release (not keep), 1 = keep
	SetOwnership( leg2, 0 )   # 0 = release (not keep), 1 = keep
	SetOwnership( leg3, 0 )   # 0 = release (not keep), 1 = keep

	canv.cd(1)
	histEffEt_A.Draw()
	histEffEt_B.Draw("SAME")
	leg1.Draw()

	canv.cd(2)
	histEffEta_A.Draw()
	histEffEta_B.Draw("SAME")

	canv.cd(3)
	histEffPhi_A.Draw()
	histEffPhi_B.Draw("SAME")

	canv.cd(4)
	histEffEt_C.Draw()
	histEffEt_D.Draw("SAME")
	histEffEt_E.Draw("SAME")
	leg2.Draw()

	canv.cd(5)
	histEffEta_C.Draw()
	histEffEta_D.Draw("SAME")
	histEffEta_E.Draw("SAME")

	canv.cd(6)
	histEffPhi_C.Draw()
	histEffPhi_D.Draw("SAME")
	histEffPhi_E.Draw("SAME")

	canv.cd(7)
	histEffEt_B.Draw()
	histEff_Et.Draw("SAME")
	leg3.Draw()

	canv.cd(8)
	histEffEta_B.Draw()
	histEff_Eta.Draw("SAME")

	canv.cd(9)
	histEffPhi_B.Draw()
	histEff_Phi.Draw("SAME")

	return canv

##### Full Trigger name list
triggerName_list = probelist

for triggerName in triggerName_list:
	print "Looking for trigger : ", triggerName
	canv = TCanvas();
	canv = produceCanvas(triggerName)
	canv.Print("plots/plot_Eff_"+triggerName+".pdf")