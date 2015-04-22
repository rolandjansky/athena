# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# use
# python plot_Eff_compare_datasets.py -b
# change variables inputfile1 and inputfile2 with the path to datasets

from ROOT import TFile, TH1F
from ROOT import TCanvas, TLegend
from probelist import *
# Import file as TFile

from ROOT import SetOwnership

inputfile1 = TFile("../outputs_r6532/ZeePhysics/Validation_Zee_LowMidPtPhysicsTriggers.root")
inputfile2 = TFile("../outputs_r6220/ZeePhysics/Validation_Zee_LowMidPtPhysicsTriggers.root")

def produceTH1F(triggerName, leveltype, variable_name_online, variable_name_offline):
	print "producing... ", variable_name_online
	hist_on  = TH1F()
	hist_off = TH1F()
	hist_on  = inputfile.Get("NavZeeTPPerf/"+triggerName+"/"+leveltype+"/"+variable_name_online)
	hist_off = inputfile.Get("NavZeeTPPerf/"+triggerName+"/"+leveltype+"/"+variable_name_offline)
	hist_on.SetStats(False)
	hist_off.SetStats(False)
	hist_on.SetLineColor(1)
	hist_off.SetLineColor(2)
	return hist_on, hist_off

def produceTH1F(triggerName, leveltype, variable):
	histEff1 = TH1F()
	histEff2 = TH1F()
	histEff1 = inputfile1.Get("NavZeeTPEff/"+triggerName+"/"+leveltype+"/"+variable)
	histEff2 = inputfile2.Get("NavZeeTPEff/"+triggerName+"/"+leveltype+"/"+variable)
	histEff1.SetStats(False)
	histEff2.SetStats(False)
	histEff1.SetLineColor(1)
	histEff2.SetLineColor(2)
	histEff1.SetMarkerStyle(1)
	histEff2.SetMarkerStyle(2)
	histEff1.SetMarkerColor(histEff1.GetLineColor())
	histEff2.SetMarkerColor(histEff2.GetLineColor())
	histEff1.SetMarkerSize(0.3)
	histEff2.SetMarkerSize(0.3)
	return histEff1, histEff2

def produceCanvas(triggerName):
	print "Plotting for ... ",triggerName
	histEff1 = TH1F()
	histEff2 = TH1F()
	leveltype_list = ["HLTEfficiencies", "EFCaloEfficiencies", "L2CaloEfficiencies"]
	variable_list = ["ZeeProbeEffEt", "ZeeProbeEffEta", "ZeeProbeEffPhi"]
	canv = TCanvas()
	canv.Divide(3,3)
	canv.cd(1)
	histEff1, histEff2 = produceTH1F(triggerName, leveltype_list[0], variable_list[0])
	histEff1.Draw()
	histEff2.Draw("SAME")
	canv.SetGridx()
	canv.cd(2)
	histEff1, histEff2 = produceTH1F(triggerName, leveltype_list[0], variable_list[1])
	histEff1.Draw()
	histEff2.Draw("SAME")
	canv.cd(3)
	histEff1, histEff2 = produceTH1F(triggerName, leveltype_list[0], variable_list[2])
	histEff1.Draw()
	histEff2.Draw("SAME")
	canv.cd(4)
	histEff1, histEff2 = produceTH1F(triggerName, leveltype_list[1], variable_list[0])
	histEff1.Draw()
	histEff2.Draw("SAME")
	canv.cd(5)
	histEff1, histEff2 = produceTH1F(triggerName, leveltype_list[1], variable_list[1])
	histEff1.Draw()
	histEff2.Draw("SAME")
	canv.cd(6)
	histEff1, histEff2 = produceTH1F(triggerName, leveltype_list[1], variable_list[2])
	histEff1.Draw()
	histEff2.Draw("SAME")
	canv.cd(7)
	histEff1, histEff2 = produceTH1F(triggerName, leveltype_list[2], variable_list[0])
	histEff1.Draw()
	histEff2.Draw("SAME")
	canv.cd(8)
	histEff1, histEff2 = produceTH1F(triggerName, leveltype_list[2], variable_list[1])
	histEff1.Draw()
	histEff2.Draw("SAME")
	canv.cd(9)
	histEff1, histEff2 = produceTH1F(triggerName, leveltype_list[2], variable_list[2])
	histEff1.Draw()
	histEff2.Draw("SAME")
	####################################
	leg1 = TLegend(0.4,0.2,0.65,0.40,triggerName)
	leg1.AddEntry(histEff1, "r6532","l")
	leg1.AddEntry(histEff2, "r6220","l")
	SetOwnership( leg1, 0 )   # 0 = release (not keep), 1 = keep
	leg1.SetBorderSize(0)
	leg1.SetTextSize(0.05)
	canv.cd(1)
	leg1.Draw()
	return canv


##### Full Trigger name list
from TrigEgammaProbelist import probeListLowMidPtPhysicsTriggers
triggerName_list = probeListLowMidPtPhysicsTriggers

# comment the line below to produce all plots
#triggerName_list = ["e26_tight_iloose"]

for triggerName in triggerName_list:
	canv = TCanvas();
	canv = produceCanvas(triggerName)
	canv.Print("plots_compare/plot_Compare_Eff_"+triggerName+".pdf")
