# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#the input log file should have the output of:
#python readFiles.py [output file name] [InputFolderWithLogFiles]
#easiest to do if you're in the run folder.
# do "eosmount eosDir; cd eosDir/atlas/atlastier0/tzero/prod/data16_13TeV/physics_Main/00303304/"
# then run the python command from that folder, with the input folder name in this case being data16_13TeV.00303304.physics_Main.daq.RAW.f716.recon.task.LOG/

#for shorter runs, you can also use a grep command:
#grep -E "RAWtoESD.*RPVLL.*Events|BSESOutputSvcStreamDRAW_RPVLL.*events" 00300784/data16_13TeV.00300784.physics_Main.daq.RAW.f708.recon.task.LOG/* > ~/rpvllLog_00300784.out &
#the folder with log files is on eos: eos/eosatlas/atlastier0/tzero/prod/physics_Main/ 
#if grep complains about the long argument, just cd into the folder with the log files. The command does take quite a while to run.
#The log files only stay around for a few weeks, or the folder is empty and only LOGARC remain. the logs can likely be recovered from the LOGARC files, if necessary

#43 lines should be recovered from each file!

#run file with python compileRPVLLRates.py rpvllLog_00303304.out 303304 lbTimes_00303304.out
#where the rpvllLog is the output from your grep command, next entry is the run number (plot label), and the next is a list of lb and seconds/lb (can be copied from runquery)

import sys
import ROOT
from ROOT import gROOT, TCanvas, TH1F, THStack
import math

#Make sure you have the atlas style files accessible!
from AtlasStyle import SetAtlasStyle 
SetAtlasStyle() 

def end_lb(lumiBlock, lbEventList = [], rateHists = [], totalRateHist = 0):
	events_in_lb = lbEventList[0]
	events_passed_lb = lbEventList[42]
	lbFilters = [0] * 20	
	for i in range(0,21):
	   if i < 7: 
		lbFilters[i] = lbEventList[2*i+1]
	   elif i > 7: lbFilters[i-1] = lbEventList[2*i+1]
	for i,lb in enumerate(lbFilters):
		rateHists[i].Fill(lumiBlock, float(lb) )
	totalRateHist.Fill(lumiBlock, float(events_passed_lb) )
	return

mypath = sys.argv[1]
runNumber = sys.argv[2]
lb_rates = sys.argv[3]

List = open(mypath).readlines()
c1 = TCanvas( 'c1', 'yay canvas', 1000, 600 )

#the filter names could change run-to-run, with filter updates. If so, need to update the number of items in the list
#and the hardcoded numbers 43 (2*number of filters + 1 line for the total accepted into RPVLL) , 21 (number of filters), 42 (2*number of filters)
filterNames = ['DiLep_SiElectronFilterKernel','DiLep_SiPhotonXFilterKernel','DiLep_SiMuonFilterKernel','DiLep_DiElectronFilterKernel','DiLep_DiPhotonFilterKernel',
'DiLep_DiElPhFilterKernel','DiLep_DiLoElPhFilterKernel','DVAugmentationKernel','DVMuonFilterKernel','DV_PhotonFilterKernel',
'DV_PhotonPlusTLJetFilterKernel','DV_MultiJetFilterKernel','DV_METFilterKernel','DV_MeffFilterKernel','KinkedTrackJetFilterKernel',
'KinkedTrackZeeFilterKernel','KinkedTrackZmumuFilterKernel','HnlFilterKernel','HV_MuvtxFilterKernel','HV_JetMETFilterKernel','HV_CalRatioFilterKernel']

#just a list of filter names minus the augmentation kernel, which is just every event in the run (we don't want to plot that!)
filterNames_mAug = ['DiLep_SiElectronFilterKernel','DiLep_SiPhotonXFilterKernel','DiLep_SiMuonFilterKernel','DiLep_DiElectronFilterKernel','DiLep_DiPhotonFilterKernel',
'DiLep_DiElPhFilterKernel','DiLep_DiLoElPhFilterKernel','DVMuonFilterKernel','DV_PhotonFilterKernel',
'DV_PhotonPlusTLJetFilterKernel','DV_MultiJetFilterKernel','DV_METFilterKernel','DV_MeffFilterKernel','KinkedTrackJetFilterKernel',
'KinkedTrackZeeFilterKernel','KinkedTrackZmumuFilterKernel','HnlFilterKernel','HV_MuvtxFilterKernel','HV_JetMETFilterKernel','HV_CalRatioFilterKernel']

eventList = [0] * 43
lbEventList = [0] * 43 

rateHists = []
for i in range(0,20):
  rateHists.append(TH1F("ratePerLB_filter"+str(i),"ratePerLB_filter;lb;rate",2500,0,2500))
  rateHists[i].Sumw2()
totalRateHist = TH1F("ratePerLB_full","ratePerLB_full;lb;rate",2500,0,2500)
totalRateHist.Sumw2()

lbTimeHist = TH1F("lbTimeHist","lbTimeHist",2500,0,2500)
lbList = open(lb_rates).readlines()
for lineNo,line in enumerate(lbList):
	lbTimeHist.Fill(float(line.split()[0]), float(line.split()[1]))
	lbTimeHist.SetBinError(lbTimeHist.FindBin(float(line.split()[0])),0)

current_lb = 0
first_lb = 0
last_lb = 2500

for lineNo,line in enumerate(List):
    if lineNo%43 == 0:
    	for lb in line.split("._"):
	    if "lb" in lb:
	    	if int(lb[2:]) != current_lb:
			if current_lb != 0: end_lb(current_lb, lbEventList, rateHists, totalRateHist)
			lbEventList = [0] * 43
			current_lb = int(lb[2:])
		if first_lb == 0:
			first_lb = current_lb
    if(lineNo < len(List) - 43):
        if(line.split()[0][:-10] == List[lineNo+43].split()[0][:-10]):
		continue
    [int(s) for s in line.split() if s.isdigit()]
    eventList[lineNo%43] += int(s)
    lbEventList[lineNo%43] += int(s)

#print eventList
end_lb(current_lb, lbEventList, rateHists, totalRateHist)
last_lb = current_lb

TotalEvents = eventList[0]
TotalRPVLLPass = eventList[42]
print 'Out of ',TotalEvents,' events, ',TotalRPVLLPass,' events passed RPVLL filters. Efficiency: ',float(TotalRPVLLPass)/float(TotalEvents)*100.,'%.'

TotalFracList = [0]*21
RPVLLFracList = [0]*21
filterEvents = [0]*21
closureTest = 0

for filterNo in range(0,21):	
	closureTest += eventList[filterNo*2+1]
	TotalFracList[filterNo] = float(eventList[filterNo*2+1])/float(TotalEvents)
	RPVLLFracList[filterNo] = float(eventList[filterNo*2+1])/float(TotalRPVLLPass)
	filterEvents[filterNo] = eventList[filterNo*2+1]
	print filterNames[filterNo],' (rel, total): (',RPVLLFracList[filterNo],', ',TotalFracList[filterNo],')'
	
#subtracting away the results of the DVAugmentationKernel, which isn't a filter.
#the first number should always be greater than the second - because of overlaps.
print 'Total number of events passing filters / total number passing RPVLL: ',closureTest-eventList[15],'/',TotalRPVLLPass,'. Rate: ',float(closureTest-eventList[15])/float(TotalRPVLLPass)*100.,'%.'

#printout for easy copy-paste into google spreasheet:
for n in filterNames:
	print n,
print ''
for n in filterEvents:
	print n,

totalRateHist.SetMarkerStyle(22)
totalRateHist.SetMarkerSize(1.2)
if (last_lb - first_lb) > 600:
	totalRateHist.SetMarkerSize(0.6)
totalRateHist.SetMarkerColor(ROOT.kBlack)
totalRateHist.SetLineColor(ROOT.kBlack)
totalRateHist.Divide(lbTimeHist)
for bin in range(1,totalRateHist.GetNbinsX() + 1):
	totalRateHist.SetBinError( bin, 0 ) #the rate is what it is, there are no appreciable errors on it
hs = THStack("hs","Stacked 1D histograms");

colors = [
#Seven for DiLep
ROOT.kBlue+3, ROOT.kBlue +1, ROOT.kBlue- 4, ROOT.kBlue-2, ROOT.kBlue - 7,ROOT.kBlue - 5, ROOT.kBlue -9, 
#Six for DV
ROOT.kRed+3, ROOT.kRed +1, ROOT.kRed-2, ROOT.kRed - 4, ROOT.kRed -6,ROOT.kRed -9,
#Three for kinked track
ROOT.kCyan+2, ROOT.kTeal - 8, ROOT.kCyan -6, 
#HNL
ROOT.kOrange+8, 
#Three for HV
ROOT.kViolet +8, ROOT.kViolet+3,  ROOT.kViolet -9,
#Extra
ROOT.kGreen+3, ROOT.kGreen +1, ROOT.kGreen-1, ROOT.kGreen - 2, ROOT.kGreen -6 ]

c1.SetRightMargin(0.25)
c1.SetLeftMargin(0.1)
leg = ROOT.TLegend(0.76,.15,.97,.96)
leg.SetFillColor(0)
leg.SetFillStyle(0)
leg.SetBorderSize(0)
leg.SetTextSize(0.02)
leg.AddEntry(totalRateHist, "Overall RPVLL rate","lp")

for i,hist in enumerate(rateHists):
	hist.SetFillColor(colors[i])
	hist.SetLineColor(colors[i])
	hist.Divide(lbTimeHist)
	hs.Add(hist)
	leg.AddEntry(hist, filterNames_mAug[i], "f")	

hs.Draw("HIST")
hs.GetXaxis().SetTitle("lumi block")
hs.GetYaxis().SetTitle("rate [Hz]")
hs.GetYaxis().SetTitleOffset(0.8)
totalRateHist.Draw("pSAME")

hs.GetXaxis().SetRangeUser(first_lb - 20, last_lb + 20)
hs.GetYaxis().SetRangeUser(0,100)
leg.Draw()
latex2 = ROOT.TLatex(0.55,0.86,"Run "+runNumber)
latex2.SetNDC();
latex2.SetTextSize(0.05);
latex2.SetTextAlign(13); 
latex2.Draw()
latex = ROOT.TLatex(0.52,0.915,"#font[72]{ATLAS }#font[42]{Internal}")
latex.SetNDC();
latex.SetTextSize(0.05);
latex.SetTextAlign(13); 
latex.Draw()
c1.Update()
c1.Print("run_"+runNumber+"_rate.pdf")
