#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# 
# python plotEfficiencies.py -b -i histo.root -t e24_lhtight_iloose
#

from sys import argv, exit

from ROOT import TFile, TH1F
from ROOT import TCanvas, TLegend
from ROOT import SetOwnership

from AtlasStyle import *
from getHistograms import *

#########################################################################################
def makeCanvas(histo,title,triggerName,folder):
	h = histo
	leg = TLegend(0.5,0.2,0.9,0.30, "HLT "+folder)
	leg.SetBorderSize(0)
	leg.SetFillStyle(0)
	leg.SetTextSize(0.04)
	leg.AddEntry(h, triggerName,"l")
	c = TCanvas("c","Efficiency",800,600)
	h.Draw()
	leg.Draw()
	c.SetGrid()
	ATLASLabel(0.2,0.2)
	if folder=="TPAnalysis":
		c.Print("plot_"+title+"_"+triggerName+"_TPAnalysis."+fileformat)
	else:
		c.Print("plot_"+title+"_"+triggerName+"_Analysis."+fileformat)

def setPlots(plots):
    colors=[1,2,1,2,4]
    marker=[4,2,4,2,5]

    icolor=0
    for iplot in plots:
         iplot.SetStats(False)
         iplot.SetLineColor(colors[icolor])
         iplot.SetMarkerStyle(marker[icolor])
         iplot.SetMarkerSize(0.3)
         iplot.SetMarkerColor(iplot.GetLineColor())
         icolor+=1

def produceCanvas(triggerName,folder,inputfile):

        level_A = "L2"
        level_B = "HLT"
        level_C = "L1Calo"
        level_D = "L2Calo"
        level_E = "EFCalo"

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

        histEffEt_A, histEffEta_A, histEffPhi_A, hMu_A = getTH1FsfromFile(inputfile,triggerName,folder,level_A)
        histEffEt_B, histEffEta_B, histEffPhi_B, hMu_B = getTH1FsfromFile(inputfile,triggerName,folder,level_B)
        histEffEt_C, histEffEta_C, histEffPhi_C, hMu_C = getTH1FsfromFile(inputfile,triggerName,folder,level_C)
        histEffEt_D, histEffEta_D, histEffPhi_D, hMu_D = getTH1FsfromFile(inputfile,triggerName,folder,level_D)
        histEffEt_E, histEffEta_E, histEffPhi_E, hMu_E = getTH1FsfromFile(inputfile,triggerName,folder,level_E)

        effplots=[histEffEt_A,histEffEt_B,histEffEt_C,histEffEt_D,histEffEt_E]
        etaplots=[histEffEta_A,histEffEta_B,histEffEta_C,histEffEta_D,histEffEta_E]
        phiplots=[histEffPhi_A,histEffPhi_B,histEffPhi_C,histEffPhi_D,histEffPhi_E]
        muplots=[hMu_A,hMu_B,hMu_C,hMu_D,hMu_E]

        plots=[effplots,etaplots,phiplots,muplots]
        for iplots in plots:
            setPlots(iplots)

        ####################
        ## some customization to the range of phi effs

        histEffEta_A.GetYaxis().SetRangeUser(0.75, 1)
        histEffEta_B.GetYaxis().SetRangeUser(0.75, 1)
        histEffEta_C.GetYaxis().SetRangeUser(0.75, 1)
        histEffEta_D.GetYaxis().SetRangeUser(0.75, 1)

        histEffPhi_A.GetYaxis().SetRangeUser(0.75, 1)
        histEffPhi_B.GetYaxis().SetRangeUser(0.75, 1)
        histEffPhi_C.GetYaxis().SetRangeUser(0.75, 1)
        histEffPhi_D.GetYaxis().SetRangeUser(0.75, 1)
        ##########################

        canv = TCanvas("canv","Efficiency",1000,600)
        canv.Divide(4,2)

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
        hMu_A.Draw()
        hMu_B.Draw("SAME")

        canv.cd(5)
        histEffEt_C.Draw()
        histEffEt_D.Draw("SAME")
        histEffEt_E.Draw("SAME")
        leg2.Draw()

        canv.cd(6)
        histEffEta_C.Draw()
        histEffEta_D.Draw("SAME")
        histEffEta_E.Draw("SAME")

        canv.cd(7)
        histEffPhi_C.Draw()
        histEffPhi_D.Draw("SAME")
        histEffPhi_E.Draw("SAME")


        for i in range(9):
            canv.cd(i+1).SetGrid()
            canv.cd(i+1).Update()

        return canv
#########################################################################################
### Main program
#########################################################################################

if(len(argv)<4):
	print "usage: plotEfficiencies.py -b <root file> <trigger name> <folder> <file format>"
	exit(2)
elif(len(argv)==6):
  filename = argv[2]
  triggerName = argv[3]
  folder = argv[4]
  fileformat = argv[5]
  print filename, triggerName, folder, fileformat
  if (folder!="TPAnalysis") or (folder!="Analysis"):
  	print "Last argument is wrong"
  print "ROOT File : ", filename
  print "Trigger name :", triggerName
  inputfile = TFile(filename)
else:
    print "usage: plotEfficiencies.py -b <root file> <trigger name> <folder> <format>"
    exit(2)


SetAtlasStyle()

name=triggerName+"_Efficiencies"+".ps"
canv=TCanvas()
canv.Print(name+"[")
canv=produceCanvas(triggerName,folder,inputfile)           
canv.Print(name)

ineffNames=[['IsEmFailLoose','IsEmFailMedium','IsEmFailTight'],['IsEmLHFailLoose','IsEmLHFailMedium','IsEmLHFailTight']] 
colors=[2,3,4]
canv2=TCanvas()
canv2.Divide(1,2)
ipad=1
for iname in ineffNames:
    canv2.cd(ipad)
    ic=0
    for iplot in iname:
        hist = getSingleTH1F(inputfile,triggerName,iplot,folder,"Efficiency","HLT")
        hist.SetLineColor(colors[ic])
        hist.Draw("SAME")
        ic+=1
    ipad+=1 

canv2.Print(name)    
canv2.Print(name+"]")    

        
