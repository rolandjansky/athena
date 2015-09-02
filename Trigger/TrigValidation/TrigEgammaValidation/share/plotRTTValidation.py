#!/usr/bin/python
# 

from sys import argv, exit

import ROOT
import os
import datetime
import calendar
import time
from ROOT import TFile, TH1F
from ROOT import TCanvas, TLegend
from ROOT import SetOwnership
from ROOT import gROOT
gROOT.SetBatch(True)
gROOT.ProcessLine("gErrorIgnoreLevel=4000;")

#########################################################################################
### Main program
#########################################################################################


currentfile = str(argv[1])
referencefile = str(argv[2])
print "Comparing " , currentfile , " and " , referencefile
inputfile = TFile(currentfile)
mtime = os.path.getmtime(str(currentfile))
day = datetime.datetime.fromtimestamp(mtime).isoweekday() #monday=1,...sunday=7
mtimeref = os.path.getmtime(str(referencefile))
dayref = datetime.datetime.fromtimestamp(mtime).isoweekday() #monday=1,...sunday=7  
if day == 7 :
   day = 0
if dayref == 7 :
   dayref = 0
now = calendar.timegm(time.gmtime())
weekold = 0
if now - mtimeref > 60*60*24*6.5 :
   weekold = 1
if now - mtimeref > 60*60*24*13.5 :
   weekold = 2
print "Time now: " , now , "s. Last modification of reference file: " , mtimeref , "s. Diff = " , now - mtimeref , "s. Age of reference file in weeks: " , weekold 
if weekold == 2 :
   "Warning! Could be older than this.."  
release_current = "rel_"+str(day)
release_previous = "rel_"+str(dayref)
print "Current release: " , release_current , " Reference release: " , release_previous
basePath1="Trigger/HLT/Egamma/Analysis/Counters/"
basePath2="Trigger/HLT/Egamma/TPAnalysis/Counters/"
plots=["hlt_el_et","l1_rot_et"]
tpplots=["nProbes","nProbesL1","nProbesL2Calo","nProbesEFCalo","nProbesHLT","EffL1","EffL2","EffEFCalo","EffHLT"]
name="Analysis_Event"+".pdf"

inputfile_reference = TFile(referencefile)
#if inputfile_reference.IsZombie() == False :
#   print "Using reference:  " , str(referencefile)
#print inputfile_reference.IsZombie() , "  " ,  release_previous!=release_current , "  " , inputfile_reference.IsZombie() and release_previous!=release_current

doReference=True
if inputfile_reference.IsZombie() :
   doReference=False
   print "Error: no reference found"

c=TCanvas()
c.Print(name+"[")
c.SetBottomMargin(0.5)

firstpagetitle = ROOT.TPaveText(0.1,0.55,0.9,0.8,"brNDC");
firstpagetitle.AddText("TrigEgammaValidation")
firstpagetitle.SetTextSize(0.1)
firstpagetitle.SetTextColor(ROOT.kRed)
firstpagetitle.Draw()
firstpage = ROOT.TPaveText(0.2,0.2,0.8,0.50,"brNDC");
if inputfile.IsZombie() == False :
   firstpage.AddText("Showing validation plots in "+release_current)
else :
   firstpage.AddText("RTT test failed")
if doReference :
   firstpage.AddText("Found reference file for comparison in "+release_previous)
   if weekold > 0 :
      firstpage.AddText("Reference file is at least "+str(weekold)+" week old")
else :
   firstpage.AddText("No comparison file found")
firstpage.Draw()
c.Print(name)

#first plot
p1 = ROOT.TPad("p","pad",0.0,0.45,1,1,0);
p2 = ROOT.TPad("p","pad",0.0,0.01,1,0.45,0);
p1.Draw()
p2.Draw()
p1.SetBottomMargin(0.);
p2.SetTopMargin(0.);
p2.SetBottomMargin(0.65);
p1.cd() ;
print basePath1+"trigger_counts"
h_ref=0
if doReference :
   h_ref=inputfile_reference.Get(basePath1+"trigger_counts")
   h_ref.GetXaxis().SetTitle("")
   h_ref.SetStats(0)
   h_ref.SetLineColor(ROOT.kRed-3)
   h_ref.SetLineWidth(2)
   h_ref.Draw("")
h=inputfile.Get(basePath1+"trigger_counts")
h.SetLineWidth(2)
h.SetLineStyle(2)
h.SetLineColor(ROOT.kBlack)
h.GetXaxis().SetTitle("")
h.SetStats(0)
if doReference :
   h.Draw("SAME")
else :
   h.Draw("")
legend = TLegend(0.72,0.9,0.9,0.995)
legend.AddEntry(h,release_current+" (current)","l")
if doReference:
   if weekold > 0 :
     legend.AddEntry(h_ref,release_previous+" (>"+str(weekold)+" week old)","l")
   else :
     legend.AddEntry(h_ref,release_previous+" (ref)","l")
legend.Draw()
p2.cd()
h_ratio = h.Clone()
if doReference :
   h_ratio.Divide(h_ref)
   h_ratio.GetYaxis().SetTitle(release_current+"/"+release_previous)
   h_ratio.SetTitle("Ratio")
   h_ratio.GetXaxis().SetLabelSize(.05)
   h_ratio.SetLineStyle(1)
   h_ratio.GetXaxis().SetTitleSize(.10)
   h_ratio.GetXaxis().SetTitleOffset(.5)
   h_ratio.GetYaxis().SetRangeUser(0.9,1.1)
   h_ratio.GetYaxis().SetTickLength(0.01)
   h_ratio.Draw("")
else :
   h_ratio.Divide(h)
   h_ratio.GetYaxis().SetTitle(release_current+"/"+release_current)
   h_ratio.SetTitle("Ratio")
   h_ratio.GetXaxis().SetLabelSize(.05)
   h_ratio.SetLineStyle(1)
   h_ratio.GetXaxis().SetTitleSize(.10)
   h_ratio.GetXaxis().SetTitleOffset(.5)
   h_ratio.GetYaxis().SetRangeUser(0.9,1.1)
   h_ratio.GetYaxis().SetTickLength(0.01)
   h_ratio.Draw("")
c.Print(name)


#second plot
p1.cd()
if doReference :
   h_ref=inputfile_reference.Get(basePath1+"electrons")
   h_ref.SetLineColor(ROOT.kRed-3)
   h_ref.SetLineWidth(2)
   h_ref.SetStats(0)
   h_ref.Draw()
h=inputfile.Get(basePath1+"electrons")
h.SetLineColor(ROOT.kBlack)
h.SetLineStyle(2)
h.SetStats(0)
h.SetLineWidth(2)
if doReference :
   h.Draw("SAME")
else :
   h.Draw("")
legend.Draw()
p2.cd()
h_ratio = h.Clone()
if doReference :
   h_ratio.Divide(h_ref)
   h_ratio.GetYaxis().SetTitle(release_current+"/"+release_previous)
   h_ratio.SetTitle("Ratio")
   h_ratio.GetXaxis().SetLabelSize(.05)
   h_ratio.SetLineStyle(1)
   h_ratio.GetXaxis().SetTitleSize(.10)
   h_ratio.GetXaxis().SetTitleOffset(.5)
   h_ratio.GetYaxis().SetRangeUser(0.9,1.1)
   h_ratio.GetYaxis().SetTickLength(0.01)
   h_ratio.Draw("")
else :
   h_ratio.Divide(h)
   h_ratio.GetYaxis().SetTitle(release_current+"/"+release_current)
   h_ratio.SetTitle("Ratio")
   h_ratio.GetXaxis().SetLabelSize(.05)
   h_ratio.SetLineStyle(1)
   h_ratio.GetXaxis().SetTitleSize(.10)
   h_ratio.GetXaxis().SetTitleOffset(.5)
   h_ratio.GetYaxis().SetRangeUser(0.9,1.1)
   h_ratio.GetYaxis().SetTickLength(0.01)
   h_ratio.Draw("")
c.Print(name)


#set of tpplots
for iplot in tpplots:
    print basePath2+"/"+iplot
    p1.cd()
    if doReference :
       h_ref=inputfile_reference.Get(basePath2+"/"+iplot)
       h_ref.GetXaxis().SetTitle("")
       h_ref.SetStats(0)
       h_ref.SetLineWidth(2)
       h_ref.SetLineColor(ROOT.kRed-3)
       h_ref.Draw()
    h=inputfile.Get(basePath2+"/"+iplot)
    h.GetXaxis().SetTitle("")
    h.SetLineStyle(2)
    h.SetLineWidth(2)
    h.SetLineColor(ROOT.kBlack)
    h.SetStats(0)
    if doReference :
       h.Draw("SAME")
    else :
       h.Draw("")
    legend.Draw()
    p2.cd()
    h_ratio = h.Clone()
    if doReference :
       h_ratio.Divide(h_ref)
       h_ratio.GetYaxis().SetTitle(release_current+"/"+release_previous)
       h_ratio.SetTitle("Ratio")
       h_ratio.GetXaxis().SetLabelSize(.05)
       h_ratio.SetLineStyle(1)
       h_ratio.GetXaxis().SetTitleSize(.10)
       h_ratio.GetXaxis().SetTitleOffset(.5)
       h_ratio.GetYaxis().SetRangeUser(0.9,1.1)
       h_ratio.GetYaxis().SetTickLength(0.01)
       h_ratio.Draw("")
    else :
       h_ratio.Divide(h)
       h_ratio.GetYaxis().SetTitle(release_current+"/"+release_current)
       h_ratio.SetTitle("Ratio")
       h_ratio.GetXaxis().SetLabelSize(.05)
       h_ratio.SetLineStyle(1)
       h_ratio.GetXaxis().SetTitleSize(.10)
       h_ratio.GetXaxis().SetTitleOffset(.5)
       h_ratio.GetYaxis().SetRangeUser(0.9,1.1)
       h_ratio.GetYaxis().SetTickLength(0.01)
       h_ratio.Draw("")
    c.Print(name)

#last two slides, starting from one with HLT and L1 E_T  
#c2=TCanvas("c","c",800,500)
#c2.Divide(2,1,0.01,0.15)
#c2.cd(1)
#p1bis = ROOT.TPad("p","pad",0.0,0.3,1,1,0);
#p2bis = ROOT.TPad("p","pad",0.0,0.01,1,0.3,0);
#p1bis.Draw()
#p2bis.Draw()
#p1bis.SetBottomMargin(0.);
#p2bis.SetTopMargin(0.);
#p2bis.SetBottomMargin(0.18);
#p1bis.cd()
#if doReference :
#   h_ref=inputfile_reference.Get(basePath1+"hlt_el_et")
#   h_ref.SetLineColor(ROOT.kRed-3)
#   h_ref.SetLineWidth(2)
#   h_ref.SetStats(0)
#   h_ref.Draw()
#h=inputfile.Get(basePath1+"hlt_el_et")
#h.SetLineColor(ROOT.kBlack)
#h.SetLineWidth(2)
#h.SetLineStyle(2)
#if doReference :
#   h.Draw("SAME")
#else :
#   h.Draw("")
#legend.Draw()
#p2bis.cd()
#h_ratio = h.Clone()
#if doReference :
#    h_ratio.Divide(h_ref)
#    h_ratio.GetYaxis().SetTitle(release_current+"/"+release_previous)
#    h_ratio.SetTitle("Ratio")
#    h_ratio.GetXaxis().SetLabelSize(.1)
#    h_ratio.SetLineStyle(1)
#    h_ratio.SetStats(0)
#    h_ratio.GetXaxis().SetTitleSize(.10)
#    h_ratio.GetXaxis().SetTitleOffset(.8)
#    h_ratio.GetYaxis().SetRangeUser(0.9,1.1)
#    h_ratio.GetYaxis().SetTickLength(0.01)
#    h_ratio.GetYaxis().SetTitleOffset(0.3)
#    h_ratio.GetYaxis().SetTitleSize(0.1)
#    h_ratio.GetYaxis().SetLabelSize(0.06)
#    h_ratio.Draw("")
#else :
#    h_ratio.Divide(h)
#    h_ratio.GetYaxis().SetTitle(release_current+"/"+release_current)
#    h_ratio.SetTitle("Ratio")
#    h_ratio.GetXaxis().SetLabelSize(.1)
#    h_ratio.SetLineStyle(1)
#    h_ratio.SetStats(0)
#    h_ratio.GetXaxis().SetTitleSize(.10)
#    h_ratio.GetXaxis().SetTitleOffset(.8)
#    h_ratio.GetYaxis().SetRangeUser(0.9,1.1)
#    h_ratio.GetYaxis().SetTickLength(0.01)
#    h_ratio.GetYaxis().SetTitleOffset(0.3)
#    h_ratio.GetYaxis().SetTitleSize(0.1)
#    h_ratio.GetYaxis().SetLabelSize(0.06)
#    h_ratio.Draw("")
#c2.cd(2)
#p1bisbis = ROOT.TPad("p","pad",0.0,0.3,1,1,0);
#p2bisbis = ROOT.TPad("p","pad",0.0,0.01,1,0.3,0);
#p1bisbis.Draw()
#p2bisbis.Draw()
#p1bisbis.SetBottomMargin(0.);
#p2bisbis.SetTopMargin(0.);
#p2bisbis.SetBottomMargin(0.18);
#p1bisbis.cd()
#if doReference :
#   h_ref=inputfile_reference.Get(basePath1+"l1_roi_et")
#   h_ref.SetLineColor(ROOT.kRed-3)
#   h_ref.SetLineWidth(2)
#   h_ref.SetStats(0)
#   h_ref.Draw()
#h=inputfile.Get(basePath1+"l1_roi_et")
#h.SetLineColor(ROOT.kBlack)
#h.SetLineWidth(2)
#h.SetLineStyle(2)
#if doReference :
#   h.Draw("SAME")
#else :
#   h.Draw("")
#legend.Draw()
#p2bisbis.cd()
#h_ratio_bis = h.Clone()
#if doReference :
#    h_ratio_bis.Divide(h_ref)
#    h_ratio_bis.GetYaxis().SetTitle(release_current+"/"+release_previous)
#    h_ratio_bis.SetTitle("Ratio")
#    h_ratio_bis.SetStats(0)
#    h_ratio_bis.GetXaxis().SetLabelSize(.1)
#    h_ratio_bis.SetLineStyle(1)
#    h_ratio_bis.GetXaxis().SetTitleSize(.10)
#    h_ratio_bis.GetXaxis().SetTitleOffset(.8)
#    h_ratio_bis.GetYaxis().SetRangeUser(0.9,1.1)
#    h_ratio_bis.GetYaxis().SetTickLength(0.01)
#    h_ratio_bis.GetYaxis().SetTitleOffset(0.3)
#    h_ratio_bis.GetYaxis().SetTitleSize(0.1)
#    h_ratio_bis.GetYaxis().SetLabelSize(0.06)
#    h_ratio_bis.Draw("")
#else :
#    h_ratio_bis.Divide(h)
#    h_ratio_bis.GetYaxis().SetTitle(release_current+"/"+release_current)
#    h_ratio_bis.SetTitle("Ratio")
#    h_ratio_bis.SetStats(0)
#    h_ratio_bis.GetXaxis().SetLabelSize(.1)
#    h_ratio_bis.SetLineStyle(1)
#    h_ratio_bis.GetXaxis().SetTitleSize(.10)
#    h_ratio_bis.GetXaxis().SetTitleOffset(.8)
#    h_ratio_bis.GetYaxis().SetRangeUser(0.9,1.1)
#    h_ratio_bis.GetYaxis().SetTickLength(0.01)
#    h_ratio_bis.GetYaxis().SetTitleOffset(0.3)
#    h_ratio_bis.GetYaxis().SetTitleSize(0.1)
#    h_ratio_bis.GetYaxis().SetLabelSize(0.06)
#    h_ratio_bis.Draw("")
#c2.Print(name)

#last slide with 2D plots
#c3 = TCanvas()
#c3.Divide(3,2)
#c3.cd(1)
#h=inputfile.Get(basePath1+"hlt_el_eta_phi")
#h.GetYaxis().SetTitle("phi")
#h.Draw("colz")
#htitle = ROOT.TPaveText(0.1,0.89,0.39,0.98,"brNDC");
#htitle.AddText(release_current+" (current)")
#htitle.Draw()
#c3.cd(2)
#if doReference :
#   h_refer=inputfile.Get(basePath1+"hlt_el_eta_phi")
#   h_refer.GetYaxis().SetTitle("phi")
#   h_refer.Draw("colz")
#   htitle_ref = ROOT.TPaveText(0.1,0.89,0.4,0.98,"brNDC")
#   if weekold > 0 :
#      htitle_ref.AddText(release_previous+" (>"+str(weekold)+" week old)")
#   else :
#      htitle_ref.AddText(release_previous+" (ref)")
#   htitle_ref.Draw()
#else :
#   pt = ROOT.TPaveText(0.2,0.4,0.8,0.6)
#   pt.AddText("box empty")
#   pt.AddText("(missing reference file)")  
#   pt.Draw()
#c3.cd(3)
#if doReference :
#   hdiff = h.Clone()
#   hdiff.Add(h_refer,-1);
#   hdiff.Draw("colz")
#   htitle_diff = ROOT.TPaveText(0.1,0.85,0.37,0.98,"brNDC")
#   htitle_diff.AddText(release_current+"-"+release_previous)
#   htitle_diff.AddText("DIFF")
#   htitle_diff.Draw()
#else :
#   pt2 = ROOT.TPaveText(0.2,0.4,0.8,0.6)
#   pt2.AddText("no diff plot")
#   pt2.AddText("(missing reference file)")
#   pt2.Draw()
#c3.cd(4)
#h=inputfile.Get(basePath1+"l1_eta_phi")
#h.Draw("colz")
#htitle.Draw()
#c3.cd(5)
#if doReference :
#   h_refer=inputfile.Get(basePath1+"l1_eta_phi")
#   h_refer.Draw("colz")
#   htitle_ref.Draw()
#else :
#   pt.Draw()
#c3.cd(6)
#if doReference :
#   hdiff2 = h.Clone()
#   hdiff2.Add(h_refer,-1)
#   hdiff2.Draw("colz")
#   htitle_diff.Draw()
#else :
#   pt2.Draw()

#c3.Print(name)
c.Print(name+"]"); 

#SetAtlasStyle()
# I want titles
#atlasStyle = AtlasStyle()
#atlasStyle.SetOptTitle(1)
#gROOT.SetStyle("ATLAS")
#gROOT.ForceStyle()
