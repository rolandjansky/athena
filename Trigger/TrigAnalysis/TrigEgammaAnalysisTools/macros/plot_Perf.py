# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import TFile, TH1F
from ROOT import TCanvas, TLegend
from probelist import *
# Import file as TFile

from ROOT import SetOwnership


inputfile = TFile("Validation_Zee.root")

leveltype = ["L2Efficiencies","HLTEfficiencies"]

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

def produceCanvas(ix, iy, triggerName, variable_list):
	variable_name_list = variable_list
	hist_on  = TH1F()
	hist_off = TH1F()
	canvas_1 = TCanvas()
	canvas_1.Divide(ix, iy)
	index = 1
	for varname in variable_name_list:
		canvas_1.cd(index)
		hist_on, hist_off = produceTH1F(triggerName, "HLTEfficiencies", "hlt_"+varname, varname)
		hist_off.Draw()
		hist_on.Draw("SAME")
		index = index+1
	if ix<4:
		leg1 = TLegend(0.25,0.25,0.75,0.75)
	else:
		leg1 = TLegend(0.25,0.25,0.45,0.45)
	leg1.AddEntry(hist_on, "Online","l")
	leg1.AddEntry(hist_off, "Offline","l")
	SetOwnership( leg1, 0 )   # 0 = release (not keep), 1 = keep
	canvas_1.cd(index)
	leg1.Draw()
	return canvas_1

def produceRatioCanvas(ix, iy, triggerName, variable_list):
	variable_name_list = variable_list
	hist_on  = TH1F()
	hist_off = TH1F()
	canvas_1 = TCanvas()
	canvas_1.Divide(ix, iy)
	index = 1
	for varname in variable_name_list:
		canvas_1.cd(index)
		hist_on, hist_off = produceTH1F(triggerName, "HLTEfficiencies", "hlt_"+varname, varname)
		hist_on.Divide(hist_off)
		hist_on.Draw("pe")
		index = index+1
	canvas_1.cd(index)
	return canvas_1
##############################

##### Full Trigger name list
triggerName_list = probelist

# comment the line below to produce all plots
#triggerName_list = ["e26_tight_iloose"]

variable_name_list_1 = ["et", "pt", "eta", "phi", "deta1", "deta2", "dphi2", "dphiresc"]
variable_name_list_2 = ["d0", "d0sig", "eprobht", "nscthits", "npixhits"]
variable_name_list_3 = ["e011", "e132", "e237", "e277", 
				        "ethad", "ethad1", "weta1", "weta2",
				        "f1", "f3", "e2tsts1", "Reta",
				        "Rphi", "Rhad", "Rhad1", "eratio"]

for triggerName in triggerName_list:
	c1 = TCanvas();
	c1 = produceCanvas(3, 3, triggerName, variable_name_list_1)
	c1.Print("plots/plot_Perf_1_for_"+triggerName+".pdf")
	c1a = TCanvas();
	c1a = produceRatioCanvas(3, 3, triggerName, variable_name_list_1)
	c1a.Print("plots/plot_PerfRatio_1_for_"+triggerName+".pdf")
	c2 = TCanvas();
	c2 = produceCanvas(3, 3, triggerName, variable_name_list_2)
	c2.Print("plots/plot_Perf_2_for_"+triggerName+".pdf")	
	c2a = TCanvas();
	c2a = produceRatioCanvas(3, 3, triggerName, variable_name_list_2)
	c2a.Print("plots/plot_PerfRatio_2_for_"+triggerName+".pdf")	
	c3 = TCanvas();
	c3 = produceCanvas(4, 4, triggerName, variable_name_list_3)
	c3.Print("plots/plot_Perf_3_for_"+triggerName+".pdf")	
	c3a = TCanvas();
	c3a = produceRatioCanvas(4, 4, triggerName, variable_name_list_3)
	c3a.Print("plots/plot_PerfRatio_3_for_"+triggerName+".pdf")	
