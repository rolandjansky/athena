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
### Set folder names and parameters
#########################################################################################
doReference=True
basePathExpert="run_222525/HLT/Egamma/Expert/Event/"
extraplots=["Zee_nProbes","Zee_nProbesHLT","Zee_EffHLT"]
#tpplots=["nProbes","nProbesL1","nProbesL2Calo","nProbesEFCalo","nProbesHLT","EffL1","EffL2","EffEFCalo","EffHLT"]
name="Analysis_Event"+".pdf"

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
dayref = datetime.datetime.fromtimestamp(mtimeref).isoweekday() #monday=1,...sunday=7  
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

# get trigger counts histograms from input .root file
directory = inputfile.GetDirectory(basePathExpert)
next = ROOT.TIter(directory.GetListOfKeys())
key=1
triggerhistos_inputfile=[]
while (key!=0) :
   key = next()
   if key==None : break
   cl = gROOT.GetClass(key.GetClassName())
   if (cl.InheritsFrom("TH1")==False) : continue
   h = key.ReadObj();
   if "trigger_counts" in h.GetName() :
       triggerhistos_inputfile.append(h.GetName())

print "Trigger count histograms in current file: " , triggerhistos_inputfile

if doReference==True : # read reference file
   inputfile_reference = TFile(referencefile)
   if inputfile_reference.IsZombie() :
      doReference=False
      print "Error: no reference found"

if doReference==True: # get trigger counts histograms from reference .root file 
   #inputfile_reference.cd(basePathExpert)
   #inputfile_reference.ls()
   directory_ref = inputfile_reference.GetDirectory(basePathExpert)
   if not directory_ref :
      doReference=False
   else :
      next_ref = ROOT.TIter(directory_ref.GetListOfKeys())
      key=1
      triggerhistos_fullreference=[]
      while (key!=0) :
         key = next_ref()
         if key==None : break
         cl = gROOT.GetClass(key.GetClassName())
         if (cl.InheritsFrom("TH1")==False) : continue
         h = key.ReadObj();
         if "trigger_counts" in h.GetName() :
            triggerhistos_fullreference.append(h.GetName())

if doReference :
   print "Trigger count histograms in reference file: " , triggerhistos_fullreference
   triggerhistos_reference=list(set(triggerhistos_inputfile).intersection(triggerhistos_fullreference))

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
   firstpage.AddText("No comparison file found, or old naming of folders")
firstpage.Draw()
c.Print(name)


# Make trigger plots
p1 = ROOT.TPad("p","pad",0.0,0.45,1,1,0)
p2 = ROOT.TPad("p","pad",0.0,0.01,1,0.45,0)
p1.Draw()
p2.Draw()
p1.SetBottomMargin(0.)
p2.SetTopMargin(0.)
p2.SetBottomMargin(0.65)

for triggerplot in triggerhistos_inputfile :
   p1.cd() 
   print basePathExpert+triggerplot  # use histogram in Trigger/HLT/Egamma/Analysis/Counters/trigger_counts
   h_ref=0
   if (doReference and (triggerplot in triggerhistos_reference)) :
      h_ref=inputfile_reference.Get(basePathExpert+triggerplot)
      h_ref.GetXaxis().SetTitle("")
      h_ref.SetTitle(triggerplot)
      h_ref.SetStats(0)
      h_ref.SetLineColor(ROOT.kRed-3)
      h_ref.SetLineWidth(2)
      h_ref.Draw("")
   h=inputfile.Get(basePathExpert+triggerplot)
   h.SetLineWidth(2)
   h.SetLineStyle(2)
   h.SetTitle(triggerplot)
   h.SetLineColor(ROOT.kBlack)
   h.GetXaxis().SetTitle("")
   h.SetStats(0)
   if (doReference and (triggerplot in triggerhistos_reference)) :
      h.Draw("SAME")
   else :
      h.Draw("")
   legend = TLegend(0.72,0.9,0.9,0.995)
   legend.AddEntry(h,release_current+" (current)","l")
   if (doReference and (triggerplot in triggerhistos_reference)):
      if weekold > 0 :
        legend.AddEntry(h_ref,release_previous+" (>"+str(weekold)+" week old)","l")
      else :
        legend.AddEntry(h_ref,release_previous+" (ref)","l")
   legend.Draw()
   p2.cd()
   h_ratio = h.Clone()
   if (doReference and (triggerplot in triggerhistos_reference)) :
      h_ratio.Divide(h_ref)
      h_ratio.GetYaxis().SetTitle(release_current+"/"+release_previous)
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
for myplot in extraplots :
   print basePathExpert+triggerplot
   p1.cd()
   if (doReference and (inputfile_reference.Get(basePathExpert+myplot)!=False)) :
      h_ref=inputfile_reference.Get(basePathExpert+myplot)
      h_ref.SetLineColor(ROOT.kRed-3)
      h_ref.GetXaxis().SetTitle("")
      h_ref.SetTitle(myplot)
      h_ref.SetLineWidth(2)
      h_ref.SetStats(0)
      h_ref.Draw()
   h=inputfile.Get(basePathExpert+myplot)
   h.SetLineColor(ROOT.kBlack)
   h.SetTitle(myplot)
   h.GetXaxis().SetTitle("")
   h.SetLineStyle(2)
   h.SetStats(0)
   h.SetLineWidth(2)
   if (doReference and (inputfile_reference.Get(basePathExpert+myplot)!=False)) :
      h.Draw("SAME")
   else :
      h.Draw("")
   legend.Draw()
   p2.cd()
   h_ratio = h.Clone()
   if (doReference and (inputfile_reference.Get(basePathExpert+myplot)!=False)) :
      h_ratio.Divide(h_ref)
      h_ratio.GetYaxis().SetTitle(release_current+"/"+release_previous)
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


c.Print(name+"]");
