# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# the input log file should have the output of:
# $ python readFiles.py [output file name] [InputFolderWithLogFiles]
# before deleted, can find log files on eos -- ex:
# /eos/atlas/atlastier0/tzero/prod/data16_13TeV/physics_Main/00303304/data16_13TeV.00303304.physics_Main.daq.RAW.f716.recon.task.LOG/
# log files only stay around for a few weeks, or the folder is empty and only LOGARC remains
# logs can likely be recovered from LOGARC files, if necessary

# for shorter or local individual runs, you can also use a grep command:
# grep -E "RAWtoALL.*RPVLL.*Events|BSESOutputSvcStreamDRAW_RPVLL.*events" INPUT_LOG_FILES > rpvllLog_runNumber.out &

# 2*n_filters+1 lines should be recovered from each file!
# 22 filters + DVAugmentationKernel --> 2*23+1 = 47

# compile rate estimates w/:
# $ python compileRPVLLRates.py rpvllLog_runNumber.out RUNNUMBER lbTimes_runNumber.out
# rpvllLog = output from readFiles.py or grep command
# RUNNUMBER is data run number (for labeling plots)
# lbTimes = list of lumiblocks info (can be copied from runquery)
# --> column 1: lumiblock; column 2: lb duration; column 3: events per lb

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

#############################################################################
## TEST SCRIPT FOR EMERGING FILTER --> TESTING NEW PT CUTS + DIJET FILTERS ##
#############################################################################

import sys
import ROOT
from ROOT import gROOT, TCanvas, TH1F, THStack, gPad, gStyle
import math
from decimal import Decimal



## GET INPUT ARGUMENTS ##
mypath = sys.argv[1]
runNumber = sys.argv[2]
lb_rates = sys.argv[3]



## GET FILTERS ##
# DVAugmentationKernel not a filter -- remove from all outputs
filterNames = [
    'DiLep_SiElectronFilterKernel',
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
    'EmergingPt120FilterKernel',
    'EmergingPt130FilterKernel',
    'EmergingPt140FilterKernel',
    'EmergingPt150FilterKernel',
    'EmergingPt120HighPt250FilterKernel',
    'EmergingPt130HighPt250FilterKernel',
    'EmergingPt140HighPt250FilterKernel',
    'EmergingPt150HighPt250FilterKernel',
    'EmergingPt120HighPt300FilterKernel',
    'EmergingPt130HighPt300FilterKernel',
    'EmergingPt140HighPt300FilterKernel',
    'EmergingPt150HighPt300FilterKernel',
    'EmergingPt120HighPt350FilterKernel',
    'EmergingPt130HighPt350FilterKernel',
    'EmergingPt140HighPt350FilterKernel',
    'EmergingPt150HighPt350FilterKernel',
    'EmergingDiJet110FilterKernel',
    'EmergingDiJet110Pt120FilterKernel',
    'EmergingDiJet110Pt130FilterKernel',
    'EmergingDiJet110Pt140FilterKernel',
    'EmergingDiJet110Pt150FilterKernel',
    'EmergingDiJet150FilterKernel',
    'EmergingDiJet150Pt120FilterKernel',
    'EmergingDiJet150Pt130FilterKernel',
    'EmergingDiJet150Pt140FilterKernel',
    'EmergingDiJet150Pt150FilterKernel',
    'EmergingDiJet175FilterKernel',
    'EmergingDiJet175Pt120FilterKernel',
    'EmergingDiJet175Pt130FilterKernel',
    'EmergingDiJet175Pt140FilterKernel',
    'EmergingDiJet175Pt150FilterKernel',
    'EmergingDiJet260FilterKernel',
    'EmergingDiJet260Pt120FilterKernel',
    'EmergingDiJet260Pt130FilterKernel',
    'EmergingDiJet260Pt140FilterKernel',
    'EmergingDiJet260Pt150FilterKernel',
    'HnlFilterKernel',
    'HV_MuvtxFilterKernel',
    'HV_JetMETFilterKernel',
    'HV_CalRatioFilterKernel' ]

# list filter names excluding DVAugmentationKernel -- want to ignore completely
filterNames_mAug = [fn for fn in filterNames if fn != 'DVAugmentationKernel']

# list pt100 emerging filters
filterNames_pt100 = [fn for fn in filterNames if ('Emerging' in fn and 'Pt' not in fn)]
# list pt120 emerging filters
filterNames_pt120 = [fn for fn in filterNames if ('Emerging' in fn and 'Pt120' in fn)]
# list pt130 emerging filters
filterNames_pt130 = [fn for fn in filterNames if ('Emerging' in fn and 'Pt130' in fn)]
# list pt140 emerging filters
filterNames_pt140 = [fn for fn in filterNames if ('Emerging' in fn and 'Pt140' in fn)]
# list pt150 emerging filters
filterNames_pt150 = [fn for fn in filterNames if ('Emerging' in fn and 'Pt150' in fn)]

# extract just emerging + just dijet filters from each list
# --> em_120, di_120
# --> concatenate em + di lists --> em, di
filterNames_em_pt100 = [fn for fn in filterNames_pt100 if 'DiJet' not in fn]
filterNames_di_pt100 = [fn for fn in filterNames_pt100 if 'DiJet' in fn]
#emFilterList = filterNames_em_pt100 + filterNames_di_pt100
emFilterList = [fn for fn in filterNames_pt100 if 'DiJet' not in fn] + [fn for fn in filterNames_pt120 if 'DiJet' not in fn] + [fn for fn in filterNames_pt130 if 'DiJet' not in fn] + [fn for fn in filterNames_pt140 if 'DiJet' not in fn] + [fn for fn in filterNames_pt150 if 'DiJet' not in fn]
dijetFilterList = [fn for fn in filterNames_pt100 if 'DiJet' in fn] + [fn for fn in filterNames_pt120 if 'DiJet' in fn] + [fn for fn in filterNames_pt130 if 'DiJet' in fn] + [fn for fn in filterNames_pt140 if 'DiJet' in fn] + [fn for fn in filterNames_pt150 if 'DiJet' in fn]
for i in emFilterList:
    print i
print ''
for i in dijetFilterList:
    print i
print ''



# numbers of filters in different lists
nf = len(filterNames)               # number of filters
nft = nf*2+1                        # number of lines per lumiblock 

# indices of certain filters
ix_dv = filterNames.index('DVAugmentationKernel')
ix_em = filterNames.index('EmergingFilterKernel')
ix_dj = filterNames.index('EmergingDiJet110FilterKernel')
print nf, nft, ix_dv, ix_em, ix_dj
print ''



# class to fill rate histograms with number of passing events per lumiblock
# lumiblock = current lumiblock
# lbEventList = filter event info for current lumiblock
# --> n events processed + passed for each filter + n total rpvll events
# rateHists = individual filter rate histograms
# totalRateHist = overall rpvll rate histogram
def end_lb(lumiBlock, lbEventList = [], rateHists = [], totalRateHist = 0):
    n_passed_lb = lbEventList[nft-1] # get total number of rpvll events per lumiblock
    lbFilters = [0] * (nf-1)         # initialize list of filter-passing events for specific lb
    for i in range (0,nf):
        # exclude DVAug -- don't want to plot this
        if i < ix_dv:
            # every second line contains number of accepted events (starts from 0)
            lbFilters[i] = lbEventList[2*i+1]
        elif i > ix_dv:
            lbFilters[i-1] = lbEventList[2*i+1]
    # fill each filter histogram with number of filter-passing events
    for i,lb in enumerate(lbFilters):
        rateHists[i].Fill(lumiBlock, float(lb))
    # fill total-rate histogram with number of overall rpvll-passing events
    totalRateHist.Fill(lumiBlock, float(n_passed_lb))
    return



## INITIALIZE HISTOGRAMS ##
# individual filter histograms
rateHists = []
for i in range(0,nf-1):
    rateHists.append(TH1F("ratePerLB_filter" + str(i), "ratePerLB_filter;lb;rate", 2500, 0, 2500))
    rateHists[i].Sumw2()
eventHists = []
for i in range(0,nf-1):
    eventHists.append(TH1F("eventsPerLB_filter" + str(i), "eventsPerLB_filter;lb;events", 2500, 0, 2500))
    eventHists[i].Sumw2()
# overall rate histogram
totalRateHist = TH1F("ratePerLB_overall", "ratePerLB_overall;lb;rate", 2500, 0, 2500)
totalRateHist.Sumw2()

# lumiblock time hists (for calculating rate)
lbTimeHist = TH1F("lbTimeHist", "lbTimeHist", 2500, 0, 2500)
lbList = open(lb_rates).readlines()
n_lb = 0
for lineNo,line in enumerate(lbList):
    lbTimeHist.Fill(float(line.split()[0]), float(line.split()[2]))
    lbTimeHist.SetBinError(lbTimeHist.FindBin(float(line.split()[0])), 0)
    n_lb += 1
    


## GET FILTER EVENT DATA FROM RPVLL LOG FILE ##
# open list of filter data
List = open(mypath).readlines()

# initialize lists to hold filter data from log files
lbEventList = [0] * nft # events per lumiblock
eventList = [0] * nft   # total events for all lumiblocks

# initialize lumiblock variables
first_lb = 0
first_line = List[0]
for lb1 in first_line.split("._"):
    if "lb" in lb1: first_lb = int(lb1[2:])
current_lb = first_lb
last_lb = 2500 # will change based on data in file

#initialize list to hold total number of processed events per lumiblock
procEvents = []
lb_i = 0

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
print 'Events passing RPVLL filters: ', totalRPVLLpass, 'out of ', totalEvents
print 'RPVLL filter efficiency: ', float(totalRPVLLpass)/float(totalEvents) * 100., '%'
print 'RPVLL normalized average rate: ', float(totalRPVLLpass)/float(totalEvents) * 1000., 'Hz'
print ''

# calculate fraction of events passing each individual filter
fracList_total = [0] * nf # fraction of ALL events passing filter
fracList_RPVLL = [0] * nf # fraction of RPVLL events passing filter
filterEvents = [0] * nf
closureTest = 0

print 'FRACTION OF (RPVLL | TOTAL) EVENTS PASSING EACH FILTER:'
for filterNo in range(0, nf):
    closureTest += eventList[filterNo*2+1] # add up all filter-passing events
    fracList_total[filterNo] = float(eventList[filterNo*2+1])/float(totalEvents)
    fracList_RPVLL[filterNo] = float(eventList[filterNo*2+1])/float(totalRPVLLpass)
    filterEvents[filterNo] = eventList[filterNo*2+1]
#    if filterNo != ix_dv:
#        print filterNames[filterNo], ' -- ', '%.2E' % Decimal(fracList_RPVLL[filterNo]), ' | ', '%.2E' % Decimal(fracList_total[filterNo])
#print ''

print 'NORMALIZED (to 1 kHz) AVERAGE FILTER RATE:'
for filterNo in range(0, nf):
    if filterNo != ix_dv:
        print filterNames[filterNo], ' -- ', '%.2f' % (fracList_total[filterNo]*1000), 'Hz'
print ''

## subtract away events corresponding to DVAugmentationKernel -- NOT A FILTER
## closureTest_mAug will always be larger than totalRPVLLpass because of overlap
## --> some RPVLL events will pass multiple filters and thus be added multiple times
#closureTest_mAug = closureTest - filterEvents[ix_dv]
#print 'Total number of events passing filters / total number of events passing RPVLL: '
#print closureTest_mAug, '/', totalRPVLLpass, '=', float(closureTest_mAug)/float(totalRPVLLpass) * 100., '%'
#print ''
#
## printout for easy copy-paste into google spreadsheet
#for n in filterNames_mAug:
#    print n,
#print ''
#for i,n in enumerate(filterEvents):
#    if i != ix_dv: print n,
#print ''



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
        if (len(line.split()) > 3:
            lbFullEventHist.Fill(float(line.split()[0]), float(line.split()[3]))
            lbFullEventHist.SetBinError(lbFullEventHist.FindBin(float(line.split()[0])), 0)
            scale.append(float(line.split()[3])/float(procEvents[l]))
        else:
            lbFullEventHist.Fill(float(line.split()[0]), procEvents[l])
            scale.append(1)
        lbScaleHist.Fill(float(line.split()[0]), scale[l])
        lbScaleHist.SetBinError(lbScaleHist.FindBin(float(line.split()[0])), 0)
        print line.split()[0], procEvents[l], line.split()[3], scale[l]
        l += 1
print ''



## DRAW + PRINT PLOTS ##
# set colors for filters
colors = [
# seven for DiLep
ROOT.kBlue+3, ROOT.kBlue+1, ROOT.kBlue-4, ROOT.kBlue-2, ROOT.kBlue-7, ROOT.kBlue-5, ROOT.kBlue-9,
# six for DV
ROOT.kRed+3, ROOT.kRed+1, ROOT.kRed-2, ROOT.kRed-4, ROOT.kRed-6, ROOT.kRed-9,
# four for kinked track
ROOT.kCyan+2, ROOT.kTeal-8, ROOT.kCyan-6, ROOT.kCyan,
# five for emerging
ROOT.kGreen+1, ROOT.kGreen+3, ROOT.kSpring+4, ROOT.kYellow-2, ROOT.kOrange,
# HNL
ROOT.kOrange+8,
# three for HV
ROOT.kViolet+8, ROOT.kViolet+3, ROOT.kViolet-9,
# extras
ROOT.kViolet, ROOT.kMagenta+2, ROOT.kPink+10, ROOT.kPink+5, ROOT.kPink-5 ]


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
# add total rpvll event hist entry to legend
l1.AddEntry(totalRateHist, "Overall RPVLL Events", "lp")

# initialize stack for all filter rate hists
hs1 = THStack("hs1", "");
# add individual filter rate hists to stack
for i,hist in enumerate(rateHists):
    hist.SetFillColor(colors[i%len(colors)])
    hist.SetLineColor(colors[i%len(colors)])
    if i >= len(colors):
        hist.SetFillStyle(3004)
    hs1.Add(hist)
    # add individual event entries to legend
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
hs1.GetYaxis().SetRangeUser(0,100)
# set tick marks on all axes
gPad.SetTicks(1,1)

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
c1.Print("run_" + runNumber + "_events.pdf")
c1.Clear()
l1.Clear()


# INDIVIDUAL FILTER EVENTS
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
    h.SetMarkerColor(colors[j%len(colors)])  
    h.SetLineColor(colors[j%len(colors)])
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
    hist.SetFillColor(colors[i%len(colors)])
    hist.SetLineColor(colors[i%len(colors)])
    if i >= len(colors):
        hist.SetFillStyle(3004)
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
c1.Print("run_" + runNumber + "_rate.pdf")
c1.Clear()
l1.Clear()


# INDIVIDUAL FILTERS RATES
c2.cd()
# loop through filter rate histograms
for j,h in enumerate(rateHists):
    h.SetMarkerStyle(22)
    h.SetMarkerSize(1.2)
    if (last_lb - first_lb) > 600:
        h.SetMarkerSize(0.6)
    h.SetMarkerColor(colors[j%len(colors)])  
    h.SetLineColor(colors[j%len(colors)])
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


    
## --- EMERGING FILTERS --- ##
# set colors for emerging filters
em_col = [ ROOT.kBlue+3, ROOT.kBlue+1, ROOT.kBlue-7, ROOT.kBlue-9,
           ROOT.kRed+3, ROOT.kRed+1, ROOT.kRed-6, ROOT.kRed-9,
           ROOT.kCyan+2, ROOT.kTeal-8, ROOT.kCyan-6, ROOT.kCyan,
           ROOT.kGreen+1, ROOT.kGreen+3, ROOT.kYellow-2, ROOT.kOrange,
           ROOT.kViolet+8, ROOT.kViolet+3, ROOT.kViolet-9, ROOT.kViolet ]
    
c3 = TCanvas('c3', '', 1000, 600)
c3.SetRightMargin(0.25)
c3.SetLeftMargin(0.1)

l2 = ROOT.TLegend(0.76, 0.4, 0.97, 0.61)
l2.SetFillColor(0)
l2.SetFillStyle(0)
l2.SetBorderSize(0)
l2.SetTextSize(0.02)

## emerging filters (including dijets) by pt-cut
#ptFilterList = [filterNames_pt100]
#ptList = ["100"]
ptFilterList = [filterNames_pt100, filterNames_pt120, filterNames_pt130, filterNames_pt140, filterNames_pt150]
ptList = ["100", "120", "130", "140", "150"]
for pt,pf in enumerate(ptFilterList):
    c = pt*4
    d = pt*4
    # loop over histograms -- only grab those for emerging filters
    hs_ej = THStack("hs_ej", "");
    for k,h_ej in enumerate(rateHists):
        for p,fn in enumerate(ptFilterList[pt]):
            if filterNames_mAug[k] == ptFilterList[pt][p]:
                if 'DiJet' not in filterNames_mAug[k]:
                    h_ej.SetFillColor(em_col[c])
                    h_ej.SetLineColor(em_col[c])
                    h_ej.SetFillStyle(1000)
                    c += 1
                else:
                    h_ej.SetFillColor(em_col[d])
                    h_ej.SetLineColor(em_col[d])
                    h_ej.SetFillStyle(3004)
                    d += 1
                hs_ej.Add(h_ej)
                l2.AddEntry(h_ej, filterNames_mAug[k], "f")
    hs_ej.Draw("HIST")
    hs_ej.GetXaxis().SetTitle("lumi block")
    hs_ej.GetYaxis().SetTitle("rate [Hz]")
    hs_ej.GetYaxis().SetTitleOffset(.75)
    hs_ej.GetXaxis().SetRangeUser(first_lb - 20, last_lb + 20)
    hs_ej.GetYaxis().SetRangeUser(0, 25) # SET Y-RANGE SAME FOR ALL STACKS (HOW?)
    gPad.SetTicks(1, 1)
    l2.Draw()
    latex1.Draw()
    latex2.Draw()
    c3.Update()
    c3.Print("run_" + runNumber + "_ej_pt" + ptList[pt] + "_rate.pdf")
    c3.Clear()
    l2.Clear()
    

## all emerging filters (excluding dijets)
hs_em = THStack("hs_em", "");
c = 0
for p,fn in enumerate(emFilterList):
    for k,h_em in enumerate(rateHists):
        if emFilterList[p] == filterNames_mAug[k]:
            h_em.SetFillColor(em_col[c])
            h_em.SetLineColor(em_col[c])
            h_em.SetFillStyle(1000)
            if emFilterList[p] == 'EmergingFilterKernel':
                c += 4
            else:
                c += 1
            hs_em.Add(h_em)
            l1.AddEntry(h_em, filterNames_mAug[k], "f")
hs_em.Draw("HIST")
hs_em.GetXaxis().SetTitle("lumi block")
hs_em.GetYaxis().SetTitle("rate [Hz]")
hs_em.GetYaxis().SetTitleOffset(.75)
hs_em.GetXaxis().SetRangeUser(first_lb - 20, last_lb + 20)
hs_em.GetYaxis().SetRangeUser(0, 25)
gPad.SetTicks(1, 1)
l1.Draw()
latex1.Draw()
latex2.Draw()
c3.Update()
c3.Print("run_" + runNumber + "_ej_em_rate.pdf")
c3.Clear()
l1.Clear()

## all dijet filters
hs_di = THStack("hs_di", "");
d = 0
for p,fn in enumerate(dijetFilterList):
    for k,h_di in enumerate(rateHists):
        if dijetFilterList[p] == filterNames_mAug[k]:
            h_di.SetFillColor(em_col[d])
            h_di.SetLineColor(em_col[d])
            h_di.SetFillStyle(3004)
            d += 1
            hs_di.Add(h_di)
            l1.AddEntry(h_di, filterNames_mAug[k], "f")
hs_di.Draw("HIST")
hs_di.GetXaxis().SetTitle("lumi block")
hs_di.GetYaxis().SetTitle("rate [Hz]")
hs_di.GetYaxis().SetTitleOffset(.75)
hs_di.GetXaxis().SetRangeUser(first_lb - 20, last_lb + 20)
hs_di.GetYaxis().SetRangeUser(0, 25)
gPad.SetTicks(1, 1)
l1.Draw()
latex1.Draw()
latex2.Draw()
c3.Update()
c3.Print("run_" + runNumber + "_ej_di_rate.pdf")
c3.Clear()
l1.Clear()
