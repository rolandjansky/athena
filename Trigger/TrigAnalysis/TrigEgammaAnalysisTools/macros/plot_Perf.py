# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# 
# python plot_Perf.py -b path_to_file.root
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
	probelist = default # or yours

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
		leg1 = TLegend(0.25,0.25,0.75,0.75, triggerName)
	else:
		leg1 = TLegend(0.25,0.25,0.45,0.45, triggerName)
	leg1.AddEntry(hist_on, "Online","l")
	leg1.AddEntry(hist_off, "Offline","l")
	leg1.SetBorderSize(0)
	leg1.SetTextSize(0.05)
	SetOwnership( leg1, 0 )   # 0 = release (not keep), 1 = keep
	canvas_1.cd(index)
	leg1.Draw()
	return canvas_1

##############################

##### Full Trigger name list
triggerName_list = probelist

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
	c2 = TCanvas();
	c2 = produceCanvas(3, 3, triggerName, variable_name_list_2)
	c2.Print("plots/plot_Perf_2_for_"+triggerName+".pdf")	
	c3 = TCanvas();
	c3 = produceCanvas(4, 4, triggerName, variable_name_list_3)
	c3.Print("plots/plot_Perf_3_for_"+triggerName+".pdf")	
