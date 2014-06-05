#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys, getopt, math
import CoolHelper

def usage():
    print "%s" % sys.argv[0]
    print "  --help                                this printout"
    print "  --refItem  <i1>                       item to be used for deadtime calc"
    print "  --testItem <i2>                       item to be tested against reference item deadtime"
    print "  --grl <grl>                           GRL to be used" 

try:
    longopts=['grl=','help','testItem=','refItem=']
    opts,args=getopt.getopt(sys.argv[1:],'',longopts)
except getopt.GetoptError,e:
    usage()
    sys.exit(2)

grlFile=''
refItem=''
testItem=''
orbitFreq = 40.0790e6 / 3564.

for o,a in opts:
    if o in ('-h','--help'):
        usage()
        sys.exit(0)
    if o == '--refItem':
        refItem = str(a)
    if o == '--testItem':
        testItem = str(a)
    if o == '--grl':
        grlFile = str(a)

if (not grlFile or not refItem or not testItem) : 
    usage()
    sys.exit(2)

splitList = grlFile.split('.')
fileName = 'DummyName'
for label in splitList:
    if 'period' in label: fileName = label

itemList = [refItem, testItem]
# ## Needed to correct ROOT behavior; see below
# CWD = os.getcwd()
import ROOT
# ## Importing gSystem may change the current directory to one of the
# ## command-line arguments; chdir to original directory to have
# ## predictable behavior
# os.chdir(CWD)
# ## Import the ROOT library for reading han results
ROOT.gSystem.Load('libGoodRunsListsLib')
from ROOT import Root

refFractions = []
testFractions = []
refRates = []
testRates = []

## read the goodrunslist xml file(s)
reader = Root.TGoodRunsListReader(grlFile)
#reader.AddXMLFile('LumiRangeCollectionB.xml')
reader.Interpret()
grl = reader.GetMergedGoodRunsList()
goodruns = grl.GetGoodRuns()
for rr in goodruns:
    rr.Summary()
    runNum = int(rr.GetRunNumber())
    TAPs = CoolHelper.readLvl1Counters(runNum, itemList, "TAP")
    TAVs = CoolHelper.readLvl1Counters(runNum, itemList, "TAV")
    tapTurns = CoolHelper.readLvl1Turns(runNum, "TAP")
    lbDictRefTAP = TAPs[refItem]
    lbDictRefTAV = TAVs[refItem]
    for testLb in lbDictRefTAP.keys():
        if not grl.HasRunLumiBlock(runNum,testLb): continue
        liveFrac = 100.
        N = lbDictRefTAP[testLb]
        errorDown = float(N) - 0.5*ROOT.TMath.ChisquareQuantile(0.1586555,2.*float(N))
        errorUp = 0.5*ROOT.TMath.ChisquareQuantile(1.-0.1586555,2.*(float(N)+1.)) - float(N)
        if N: liveFrac = (100.*float(lbDictRefTAV[testLb])/float(N))
#        else: print "ZERO!!"
        if N:
            fracErrorUp = errorUp/float(N) * liveFrac
            fracErrorDown = errorDown/float(N) * liveFrac
        else:
            fracErrorUp = 0.
            fracErrorDown = 0.
        refFractions.append(liveFrac)
        refRates.append( float(N) / float(tapTurns[testLb]) * orbitFreq)

    lbDictTestTAP = TAPs[testItem]
    lbDictTestTAV = TAVs[testItem]
    for testLb in lbDictTestTAP.keys():
        if not grl.HasRunLumiBlock(runNum,testLb): continue
        liveFrac = 100.
        N = lbDictTestTAP[testLb]
        if N: liveFrac = (100.*float(lbDictTestTAV[testLb])/float(N))
        testFractions.append(liveFrac)
        testRates.append( float(N) / float(tapTurns[testLb]) * orbitFreq)

tfile = ROOT.TFile(fileName + "_livefractions.root","recreate")
refHist = ROOT.TH1F("ReferenceItem_"+refItem,"Live fraction versus LB;Lumi blocks (a.u.);Live fraction AV/AP ( % )", len(refFractions),-0.5,float(len(refFractions)-0.5))
testHist = ROOT.TH1F("TestItem_"+testItem,"Live fraction versus LB;Lumi blocks (a.u.);Live fraction ( % )", len(testFractions),-0.5,float(len(testFractions)-0.5))
diffHist = ROOT.TH1F("Difference","Live fraction difference "+refItem+" - "+testItem+" versus LB;Lumi blocks (a.u.);#Delta live fraction ( % )", len(testFractions),-0.5,float(len(testFractions)-0.5))
refRateHist = ROOT.TH1F("RateReferenceItem_"+refItem,"Rate versus LB;Lumi blocks (a.u.);AP Rate ( Hz )", len(testFractions),-0.5,float(len(testFractions)-0.5))
testRateHist = ROOT.TH1F("RateTestItem_"+testItem,"Rate versus LB;Lumi blocks (a.u.);Rate ( Hz )", len(testFractions),-0.5,float(len(testFractions)-0.5))

cc = 0
for ref in refFractions: 
    #print ref,testFractions[cc]
    refHist.Fill(cc,ref)
    testHist.Fill(cc,testFractions[cc])
    diffHist.Fill(cc,ref-testFractions[cc])
    refRateHist.Fill(cc,refRates[cc])
    testRateHist.Fill(cc,testRates[cc])
    cc += 1

c = ROOT.TCanvas("LiveFractions","",1200,750)
c.Divide(1,3)
c.cd(1)
refHist.SetStats(0)
refHist.SetLineColor(46)
refHist.GetXaxis().SetTitleOffset(0.8)
refHist.GetXaxis().SetTitleSize(0.07)
refHist.GetXaxis().SetLabelSize(0.06)
refHist.GetYaxis().SetTitleOffset(0.5)
refHist.GetYaxis().SetTitleSize(0.08)
refHist.GetYaxis().SetLabelSize(0.06)
refHist.SetMinimum(-5.)
refHist.SetMaximum(160.)
refHist.Draw("hist")
testHist.SetLineColor(38)
testHist.Draw("hist same")

leg1 = ROOT.TLegend(0.7,0.7,1. - ROOT.gPad.GetRightMargin(),1. - ROOT.gPad.GetTopMargin());
leg1.SetFillStyle(0);
leg1.SetLineWidth(1);
leg1.AddEntry(refHist,refItem,"l");
leg1.AddEntry(testHist,testItem,"l");
leg1.Draw("same")
c.cd(2)
diffHist.SetMinimum(-15.)
diffHist.SetMaximum(15.)
diffHist.SetStats(0)
diffHist.GetXaxis().SetTitleOffset(0.8)
diffHist.GetXaxis().SetTitleSize(0.07)
diffHist.GetXaxis().SetLabelSize(0.06)
diffHist.GetYaxis().SetTitleOffset(0.5)
diffHist.GetYaxis().SetTitleSize(0.08)
diffHist.GetYaxis().SetLabelSize(0.06)
diffHist.Draw("hist")
pad=c.cd(3)
pad.SetLogy()
maxRef = refRateHist.GetMaximum()
maxTest = testRateHist.GetMaximum()
max = maxRef
if maxTest > max: max = maxTest
refRateHist.SetMaximum(max*1.7)
refRateHist.SetStats(0)
refRateHist.SetLineColor(46)
refRateHist.GetXaxis().SetTitleOffset(0.8)
refRateHist.GetXaxis().SetTitleSize(0.07)
refRateHist.GetXaxis().SetLabelSize(0.06)
refRateHist.GetYaxis().SetTitleOffset(0.5)
refRateHist.GetYaxis().SetTitleSize(0.08)
refRateHist.GetYaxis().SetLabelSize(0.06)
refRateHist.Draw("hist")
testRateHist.SetLineColor(38)
testRateHist.Draw("hist same")

refHist.Write()
testHist.Write()
diffHist.Write()
c.Write()

c.Print(fileName + "_livefractions.gif")

tfile.Close()



# Add gif print, 1D hist of livefrac differences
