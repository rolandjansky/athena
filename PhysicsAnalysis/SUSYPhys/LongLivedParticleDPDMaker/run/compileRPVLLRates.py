# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Author: Heather Russel
# Modified: 17 July 2017 by Colleen Treado

# STEP 1: PARSE PHYSICS MAIN LOGFILES
# $ python readFiles.py [output file name] [InputFolderWithLogFiles]
# before deleted, can find physics_Main log files on eos -- ex:
# /eos/atlas/atlastier0/tzero/prod/data16_13TeV/physics_Main/00303304/data16_13TeV.00303304.physics_Main.daq.RAW.f716.recon.task.LOG/
# 2*n_filters+1 lines should be recovered from each file!
# 22 filters + DVAugmentationKernel --> 2*23+1 = 47

# NOTE: log files only stay around for a few weeks
# or the folder is empty and LOGARC remains -->
# logs can likely be recoverd from LOGARC files, if necessary

# NOTE: for shorter or local individual runs can grep logs directly:
# grep -E "RAWtoALL.*RPVLL.*Events|BSESOutputSvcStreamDRAW_RPVLL.*events" INPUT_LOG_FILES > rpvllLog_runNumber.out &

# STEP 2: COMPILE RPVLL RATE INFORMATION
# $ python compileRPVLLRates.py rpvllLog_runNumber.out RUNNUMBER lbTimes_runNumber.out
# rpvllLog = output from readFiles.py or grep command
# RUNNUMBER is data run number (for labeling plots)
# lbTimes = list of lumiblocks and seconds/lumiblocks (can be copied from runquery)

# NOTE ON lbTimes FILE:
# for easiest copying from runquery, code designed to read file with (at least) three columns:
# lb | start time (not used) | duration | nEvents in lb (optional)
# --> second column must contain any non-empty string
# --> fourth column is optional and used for scaling of analyses of partial lb's

# outputs n_filters+1 rate plots:
# one plot for overall rate + stacked filter rates
# n_filters plots for individual filter rates -- one per filter
# plots outputted into "plots/" directory
# --> MAKE SURE THIS EXISTS IN CURRENT WORKING DIRECTORY (wherever script called from)

# NOTE: for personal runs, RAWtoDRAW_RPVLL log files need to be in following format to work w/ this script:
# *._lb*._*


import sys
import ROOT
from ROOT import gROOT, TCanvas, TH1F, THStack, gPad, gStyle
import math
from decimal import Decimal



## GET INPUT ARGUMENTS ##
mypath = sys.argv[1]
runNumber = sys.argv[2]
lb_rates = sys.argv[3]


##  GET FILTERS ##
# DVAugmentationKernel not a filter -- include in printout but not plots or rate calc.
# DVAugmentationKernel appears 7th in filter list (counting from 0)
# 22 filters -- 23 including DVAug
filterNames = [ 'DiLep_SiElectronFilterKernel',
                'DiLep_SiPhotonXFilterKernel',
                'DiLep_SiMuonFilterKernel',
                'DiLep_DiElectronFilterKernel',
                'DiLep_DiPhotonFilterKernel',
                'DiLep_DiElPhFilterKernel',
                'DiLep_DiLoElPhFilterKernel',
                'DVAugmentationKernel',
                'DVMuonFilterKernel',
                'DV_PhotonFilterKernel',
                'DV_PhotonPlusTLJetFilterKernel',
                'DV_MultiJetFilterKernel',
                'DV_METFilterKernel',
                'DV_MeffFilterKernel',
                'KinkedTrackJetFilterKernel',
                'KinkedTrackMultiJetFilterKernel',
                'KinkedTrackZeeFilterKernel',
                'KinkedTrackZmumuFilterKernel',
                'EmergingFilterKernel',
                'HnlFilterKernel',
                'HV_MuvtxFilterKernel',
                'HV_JetMETFilterKernel',
                'HV_CalRatioFilterKernel' ]

# list filter names excluding DVAugmentationKernel -- for plotting and calculating rate
filterNames_mAug = [fn for fn in filterNames if fn != 'DVAugmentationKernel']

nf = len(filterNames) # number of filters (23)
nfm = len(filterNames_mAug) # number of filters excluding DVAug (22)
nft = nf*2+1 # number of filters times two (events analyzed + accepted) plus one (total) (47)
n_aug = filterNames.index('DVAugmentationKernel') # filterNames ix where DVAug resides (7)


## CLASS TO FILL RATE HISTOGRAMS W/ NUMBER OF PASSING EVENTS PER LUMIBLOCK ##
# lumiblock = specific lumiblock being read
# lbEventList = list of number of events analyzed and accepted per filter + total accepted for one lumiblock
# rateHists = list of individual filter rate histograms
# totalRateHist = overall rpvll rate histogram
def end_lb(lumiBlock, lbEventList = [], rateHists = [], totalRateHist = 0):
    n_events_lb = lbEventList[0] # get total number of events per lumiblock
    n_passed_lb = lbEventList[nft-1] # get total number of filter-passing events per lumiblock
    lbFilters = [0] * (nf-1) # initialize list of filter-passing events for specific lumiblock
    for i in range (0,nf):
        # exclude DVAug -- don't want to plot this
        if i < n_aug:
            # every second line contains number of accepted events
            lbFilters[i] = lbEventList[2*i+1]
        elif i > n_aug:
            lbFilters[i-1] = lbEventList[2*i+1]
    # fill each filter histogram with filter rate
    for i,lb in enumerate(lbFilters):
        rateHists[i].Fill(lumiBlock, float(lb))
    # fill total-rate histogram with overal rpvll rate
    totalRateHist.Fill(lumiBlock, float(n_passed_lb))
    return


## INITIALIZE HISTOGRAMS ##
# individual filter histograms
rateHists = []
for i in range(0,nf-1):
    rateHists.append(TH1F("ratePerLB_filter" + str(i), "ratePerLB_filter;lb;rate", 2500, 0, 2500))
    rateHists[i].Sumw2()
eventHists=[]
for i in range(0,nf-1):
    eventHists.append(TH1F("eventsPerLB_filter" + str(i), "eventsPerLB_filter;lb;events", 2500, 0, 2500))
    eventHists[i].Sumw2()
# overall rate histogram
totalRateHist = TH1F("ratePerLB_overall", "ratePerLB_overall;lb;rate", 2500, 0, 2500)
totalRateHist.Sumw2()

# lumiblock-time hist (needed to calculate rate)
lbTimeHist = TH1F("lbTimeHist", "lbTimeHist", 2500, 0, 2500)
lbList = open(lb_rates).readlines()
for lineNo,line in enumerate(lbList):
    lbTimeHist.Fill(float(line.split()[0]), float(line.split()[2]))
    lbTimeHist.SetBinError(lbTimeHist.FindBin(float(line.split()[0])), 0) # ???


## GET FILTER EVENT DATA FROM RPVLL LOG FILE ##
# open list of filter data
List = open(mypath).readlines()

# initialize lists to hold filter data from log files
lbEventList = [0] * nft # events per lumiblock
eventList = [0] * nft # total events for all lumiblocks

# initialize lumiblock variables
first_lb = 0
first_line = List[0]
for lb1 in first_line.split("._"):
    if "lb" in lb1: first_lb = int(lb1[2:])
current_lb = first_lb
last_lb = 2500 # will change based on data in file
print "FIRST LB:", first_lb, current_lb

# initialize list to hold total number of processed events per lumiblock
procEvents = []

# loop over lines in filter-data list (from rpvll log output)
for lineNo,line in enumerate(List):
    # if line number mod 2*n_filters+1 = 0, we're at the beginning of filter list
    if lineNo % nft == 0:
        # extract lumiblock info from line
        for lb in line.split("._"):
            if "lb" in lb:
                # if lumiblock read from line not "current_lb"
                # we've collected all info for current lumiblock --> start of new lb
                # --> call end_lb for lumiblock just processed + reset
                if int(lb[2:]) != current_lb:
                    end_lb(current_lb, lbEventList, rateHists, totalRateHist)
                    # save number of events processed per lumiblock
                    procEvents.append(lbEventList[0])
                    # set current lumiblock to lumiblock read from line
                    current_lb = int(lb[2:])
                    # re-initialize lbEventList -- set all elements to zero
                    lbEventList = [0] * nft
    # catches any repeats ??? -- necessary to avoid overcounting somehow
    if (lineNo < len(List) - nft):
        if (line.split()[0][:-10] == List[lineNo+nft].split()[0][:-10]):
            continue
    # get number of events analyzed/accepted from each line
    s = line.split()[-1]
    # add number of events analyzed/accepted per lb to lbEventList
    lbEventList[lineNo % nft] += int(s)
    eventList[lineNo % nft] += int(s)

# run end_lb for last lumiblock; set last_lb to last lb in file; get n events from last lb
end_lb(current_lb, lbEventList, rateHists, totalRateHist)
last_lb = current_lb
procEvents.append(lbEventList[0])


## RATE / EFFICIENCY CALCULATIONS ##
totalEvents = eventList[0]
totalRPVLLpass = eventList[nft-1]
print 'Events passing RPVLL filters:', totalRPVLLpass, 'out of', totalEvents
print 'RPVLL filter efficiency:', float(totalRPVLLpass)/float(totalEvents) * 100., '%'
print 'RPVLL normalized average rate: ', float(totalRPVLLpass)/float(totalEvents) * 1000., 'Hz'
print ''

# calculate fraction of events passing each indidivual filter
fracList_total = [0] * nf # fraction of ALL events passing filter
fracList_RPVLL = [0] * nf # fraction of RPVLL events passing filter
filterEvents = [0] * nf
closureTest = 0

print 'FRACTION OF (RPVLL | TOTAL) EVENTS PASSING EACH FILTER:'
for filterNo in range(0,nf):
    closureTest += eventList[filterNo*2+1]
    fracList_total[filterNo] = float(eventList[filterNo*2+1])/float(totalEvents)
    fracList_RPVLL[filterNo] = float(eventList[filterNo*2+1])/float(totalRPVLLpass)
    filterEvents[filterNo] = eventList[filterNo*2+1]
    if filterNo != n_aug:
        print filterNames[filterNo], ' -- ', '%.2E' % Decimal(fracList_RPVLL[filterNo]), ' | ', '%.2E' % Decimal(fracList_total[filterNo])
print ''

print 'NORMALIZED (to 1 kHz) AVERAGE FILTER RATE:'
for filterNo in range(0,nf):
    if filterNo != n_aug:
        print filterNames[filterNo], ' -- ', '%.2f' % (fracList_total[filterNo]*1000), 'Hz'
print ''

# subtract away events corresponding to DVAugmentationKernel -- NOT A FILTER
# closureTest_mAug will always be larger than totalRPVLLpass because of overlap
# --> some RPVLL events will pass multiple filters and thus be added multiple times
closureTest_mAug = closureTest - filterEvents[n_aug]
print 'Total number of events passing filters / total number of events passing RPVLL: ', closureTest_mAug, '/', totalRPVLLpass, '=', float(closureTest_mAug)/float(totalRPVLLpass) * 100., '%'
print ''
                
# printout for easy copy-paste into google spreadsheet
for n in filterNames_mAug:
    print n,
print ''
for n in range(0,nf):
    if n != n_aug: print filterEvents[n],
print ''



## INITIALIZE OTHER LUMIBLOCK HISTS ##
lbEventHist = TH1F("lbEventHist", "lbEventHist", 2500, 0, 2500)
lbFullEventHist = TH1F("lbFullEventHist", "lbFullEventHist", 2500, 0, 2500)
lbScaleHist = TH1F("lbScaleHist", "lbScaleHist", 2500, 0, 2500)
# parse lbTimes file for total events per lumiblock per run -- for scaling partial runs
scale = []
l = 0
for lineNo,line in enumerate(lbList):
    if (float(line.split()[0]) >= first_lb and float(line.split()[0]) <= last_lb):
        lbEventHist.Fill(float(line.split()[0]), procEvents[l])
        lbEventHist.SetBinError(lbEventHist.FindBin(float(line.split()[0])), 0)
        if len(line.split()) > 3:
            lbFullEventHist.Fill(float(line.split()[0]), float(line.split()[3]))
            lbFullEventHist.SetBinError(lbFullEventHist.FindBin(float(line.split()[0])), 0)
            scale.append(float(line.split()[3])/float(procEvents[l]))
        else:
            lbFullEventHist.Fill(float(line.split()[0]), procEvents[l])
            scale.append(1)
        lbScaleHist.Fill(float(line.split()[0]), scale[l])
        lbScaleHist.SetBinError(lbScaleHist.FindBin(float(line.split()[0])), 0)
        #print "TEST: ", line.split()[0], procEvents[l], scale[l], l, lineNo, len(procEvents)
        l += 1
print ''


## DRAW + PRINT PLOTS ##
# set colors for individual filter rate hists
colors = [
# seven for DiLep
ROOT.kBlue+3, ROOT.kBlue+1, ROOT.kBlue-4, ROOT.kBlue-2, ROOT.kBlue-7,ROOT.kBlue-5, ROOT.kBlue-9,
# six for DV
ROOT.kRed+3, ROOT.kRed+1, ROOT.kRed-2, ROOT.kRed-4, ROOT.kRed-6,ROOT.kRed-9,
# four for kinked track
ROOT.kCyan+2, ROOT.kTeal-8, ROOT.kCyan-6, ROOT.kCyan,
# emerging
ROOT.kGreen+1,
# HNL
ROOT.kOrange+8,
# three for HV
ROOT.kViolet+8, ROOT.kViolet+3, ROOT.kViolet-9 ]

# ALL FILTERS -- STACK PLOT
# create + configure new canvas
c1 = TCanvas('c1', '', 1000, 600)
c1.SetRightMargin(0.25)
c1.SetLeftMargin(0.1)

# create + configure new legend
l1 = ROOT.TLegend(0.76, 0.1, 0.97, 0.91)
l1.SetFillColor(0)
l1.SetFillStyle(0)
l1.SetBorderSize(0)
l1.SetTextSize(0.02)

# FILTER EVENTS
# configure total rate hist
totalRateHist.SetMarkerStyle(22)
totalRateHist.SetMarkerSize(1.2)
if (last_lb - first_lb) > 600:
    totalRateHist.SetMarkerSize(0.6)
totalRateHist.SetMarkerColor(ROOT.kBlack)
totalRateHist.SetLineColor(ROOT.kBlack)
for bin in range(1, totalRateHist.GetNbinsX() + 1):
    totalRateHist.SetBinError(bin, 0) # no appreciable errors on rate
# add total rate hist entry to legend
l1.AddEntry(totalRateHist, "Overall RPVLL Events", "lp")

# initialize stack for all filter rate hists
hs1 = THStack("hs1", "");
# add individual filter rate hists to stack
for i,hist in enumerate(rateHists):
    hist.SetFillColor(colors[i])
    hist.SetLineColor(colors[i])
    hs1.Add(hist)
    # add individual rate entries to legend
    l1.AddEntry(hist, filterNames_mAug[i], "f")

# draw individual filter stack + overall rate hist
hs1.Draw("HIST")
totalRateHist.Draw("pSAME")

## configure axes
# set axis titles
hs1.GetXaxis().SetTitle("lumi block")
hs1.GetYaxis().SetTitle("n events")
hs1.GetYaxis().SetTitleOffset(.75)
# set axis ranges
hs1.GetXaxis().SetRangeUser(first_lb - 20, last_lb + 20)
hs1.GetYaxis().SetRangeUser(0, 100)
# set tick marks on all axes
gPad.SetTicks(1, 1)

# draw legend
l1.Draw()

# add text to plot
latex1 = ROOT.TLatex(0.52, 0.875, "#font[72]{ATLAS }#font[42]{Internal}")
latex1.SetNDC()
latex1.SetTextSize(0.05)
latex1.SetTextAlign(13)
latex1.Draw()
latex2 = ROOT.TLatex(0.55, 0.82, "Run " + runNumber)
latex2.SetNDC()
latex2.SetTextSize(0.05)
latex2.SetTextAlign(13)
latex2.SetTextFont(42)
latex2.Draw()

# print stack
c1.Update()
c1.Print("run_" + runNumber + "_rpvllEvents.pdf")
c1.Clear()
l1.Clear()


# individual filter events
c2 = TCanvas('c2', '', 850, 600)
c2.SetRightMargin(0.11765)
c2.SetLeftMargin(0.11765)

latex3 = ROOT.TLatex(0.62, 0.875, "#font[72]{ATLAS }#font[42]{Internal}")
latex3.SetNDC()
latex3.SetTextSize(0.05)
latex3.SetTextAlign(13)
latex4 = ROOT.TLatex(0.65, 0.82, "Run " + runNumber)
latex4.SetNDC()
latex4.SetTextSize(0.05)
latex4.SetTextAlign(13)
latex4.SetTextFont(42)

# loop over filter rate histograms
for j,h in enumerate(rateHists):
    h.SetMarkerStyle(22)
    h.SetMarkerSize(1.2)
    if (last_lb - first_lb) > 600:
        h.SetMarkerSize(0.6)
    h.SetMarkerColor(colors[j])  
    h.SetLineColor(colors[j])
    h.Draw("P")
    h.SetTitle(filterNames_mAug[j])
    h.GetXaxis().SetTitle("lumi block")
    h.GetYaxis().SetTitle("n events")
    h.GetXaxis().SetRangeUser(first_lb - 20, last_lb + 20)
    h.SetStats(ROOT.kFALSE)
    gPad.SetTicks(1,1)
    gStyle.SetErrorX(0)
    latex3.Draw()
    latex4.Draw()
    c2.Update()
    c2.Print("run_" + runNumber + "_" + str(j) + "_events.pdf")
    

# FILTER RATES
c1.cd()
# configure total rate hist
totalRateHist.Divide(lbTimeHist)
totalRateHist.Multiply(lbScaleHist)
for bin in range(1, totalRateHist.GetNbinsX() + 1):
    totalRateHist.SetBinError(bin, 0) # no appreciable errors on rate
# add total rpvll rate hist entry to legend
l1.AddEntry(totalRateHist, "Overall RPVLL Rate", "lp")

# initialize stack for all filter rate hists
hs2 = THStack("hs2", "");
# add individual filter rate hists to stack
for i,hist in enumerate(rateHists):
    hist.SetFillColor(colors[i])
    hist.SetLineColor(colors[i])
    hist.Divide(lbTimeHist)    # calculate rate estimate
    hist.Multiply(lbScaleHist) # scale to full lb
    hs2.Add(hist)
    # add individual event entries to legend
    l1.AddEntry(hist, filterNames_mAug[i], "f")

# draw individual filter stack + overall rate hist
hs2.Draw("HIST")
totalRateHist.Draw("pSAME")

## configure axes
# set axis titles
hs2.GetXaxis().SetTitle("lumi block")
hs2.GetYaxis().SetTitle("rate [Hz]")
hs2.GetYaxis().SetTitleOffset(.75)
# set axis ranges
hs2.GetXaxis().SetRangeUser(first_lb - 20, last_lb + 20)
hs2.GetYaxis().SetRangeUser(0,100)
# set tick marks on all axes
gPad.SetTicks(1,1)

# draw legend
l1.Draw()

# add text to plot
latex1.Draw()
latex2.Draw()

# print stack
c1.Update()
c1.Print("run_" + runNumber + "_rpvllRate.pdf")
c1.Clear()
l1.Clear()


# individual filter rates
c2.cd()
# loop through filter rate histograms
for j,h in enumerate(rateHists):
    h.SetMarkerStyle(22)
    h.SetMarkerSize(1.2)
    if (last_lb - first_lb) > 600:
        h.SetMarkerSize(0.6)
    h.SetMarkerColor(colors[j])  
    h.SetLineColor(colors[j])
    # already divided by lb-times + multiplied by scale
    h.Draw("P")
    h.SetTitle(filterNames_mAug[j])
    h.GetXaxis().SetTitle("lumi block")
    h.GetYaxis().SetTitle("rate [Hz]")
    h.GetXaxis().SetRangeUser(first_lb - 20, last_lb + 20)
    h.SetStats(ROOT.kFALSE)
    gPad.SetTicks(1,1)
    gStyle.SetErrorX(0)
    latex3.Draw()
    latex4.Draw()
    c2.Update()
    c2.Print("run_" + runNumber + "_" + str(j) + "_rate.pdf")

