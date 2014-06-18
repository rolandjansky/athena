# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import gROOT
gROOT.SetBatch(True)

from optparse import OptionParser
p = OptionParser()
p.add_option('--input1',  type = 'string', default = "fitResPlots_2nd.root",   dest = 'inFile1', help = 'intput File1' )
p.add_option('--input2',  type = 'string', default = "fitResPlots_1st.root",   dest = 'inFile2', help = 'intput File2' )
p.add_option('--name1',  type = 'string', default = "1st",   dest = 'name1', help = 'intput hist name1' )
p.add_option('--name2',  type = 'string', default = "2nd",   dest = 'name2', help = 'intput help name2' )
p.add_option('--output', type = 'string', default = "compResPlots.root", dest = 'outFile', help = 'output File' )
p.add_option('--mode', type = 'string', default = "sigma", dest = 'mode', help = 'mode' )
p.add_option('--pdf', action = 'store_true', default = False, dest = 'doPDFs', help = 'save pdfs' )
p.add_option('--addOff', action = 'store_true', default = False, dest = 'addOff', help = 'add offline' )
(o,a) = p.parse_args()

from ROOT import *
import OfficialAtlasStyle
gStyle.SetOptStat(0)
gStyle.SetOptTitle(0)

##open the root files
file1 = TFile(o.inFile1,"READ");
file2 = TFile(o.inFile2,"READ");

outFile = TFile(o.outFile,"RECREATE");

def getMax(tp,ref,hist):
    maxDic = {}
    maxDic["z0_vs_InvPt"] = 0.3
    maxDic["z0_vs_eta"]   = 0.6

    maxDic["d0_vs_InvPt"] = 0.15
    maxDic["d0_vs_eta"]   = 0.1
    
    if tp+"_vs_"+ref not in maxDic:
        return hist.GetMaximum()

    return maxDic[tp+"_vs_"+ref]


def compPlot(tp,ref,addOff=False):

    can1 = file1.Get("can_rmsVsSigma"+tp+"_vs_"+ref)
    can2 = file2.Get("can_rmsVsSigma"+tp+"_vs_"+ref)

    if addOff:
        can3 = file1.Get("can_rmsVsSigmaoff_"+tp+"_vs_"+ref)        
    
    if o.mode == "sigma":
        hist1 = can1.FindObject("sigma_"+tp+"_vs_"+ref)
        hist2 = can2.FindObject("sigma_"+tp+"_vs_"+ref)
        if addOff:
            hist3 = can3.FindObject("sigma_off_"+tp+"_vs_"+ref)
            hist3.SetLineColor(ROOT.kBlue)
            hist3.SetMarkerColor(ROOT.kBlue)
    else:
        hist1 = can1.FindObject("rms_"+tp+"_vs_"+ref)
        hist1.SetLineColor(ROOT.kBlack)
        hist1.SetMarkerColor(ROOT.kBlack)
        hist2 = can2.FindObject("rms_"+tp+"_vs_"+ref)
        if addOff:
            hist3 = can3.FindObject("rms_off_"+tp+"_vs_"+ref)
            hist3.SetLineColor(ROOT.kBlue)
            hist3.SetMarkerColor(ROOT.kBlue)
        
    hist2.SetLineColor(ROOT.kRed)
    hist2.SetMarkerColor(ROOT.kRed)

    cComp = TCanvas(tp+"_vs_"+ref+"_"+o.name1+"_vs_"+o.name2+"",tp+"_vs_"+ref,700,500)
    cComp.SetTopMargin(0.1)
    cComp.cd()
    hist1.SetStats(0)
    hist1.GetYaxis().SetRangeUser(0,getMax(tp,ref,hist1))
    hist2.SetStats(0)
    hist1.Draw()
    hist2.Draw("same")

    leg = TLegend(0.2,0.2,0.4,0.3)
    leg.SetFillColor(0)
    leg.AddEntry(hist1,o.name1,"l");
    leg.AddEntry(hist2,o.name2,"l");
    if addOff:
        hist3.Draw("same")
        leg.AddEntry(hist3,"Offline","l")

    leg.Draw("same")
    
    cComp.Write()

    if o.doPDFs:
        pdfDirName = o.outFile.rstrip(".root")
        print pdfDirName+"/"+cComp.GetName()+".pdf"
        import os
        if not os.path.isdir(pdfDirName):
            print "Making dir ",pdfDirName
            os.mkdir(pdfDirName)
        cComp.SaveAs(pdfDirName+"/"+cComp.GetName()+".pdf")
        cComp.SaveAs(pdfDirName+"/"+cComp.GetName()+".eps")
        
resTP= ["d0","z0","Ipt","eta","phi"]
for tp in resTP:
    compPlot(tp,"InvPt",o.addOff)
    compPlot(tp,"eta",  o.addOff)

