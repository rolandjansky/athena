#!/bin/env python 
# Author: Michael Miller (miller@uchicago.edu)

import os, struct, sys
import pickle
import ROOT
from aux_functions import *


def readModule(part, mod, runnumber, datadir, outdir):

    ROOT.gROOT.SetBatch()
    
    print 'Checking', part, mod

    filename = '%stiletb_%d_MonoCis.%s%02d.0.aan.root' % (datadir, runnumber, part, mod)

    partnames = ['LBA', 'LBC', 'EBA', 'EBC']
    partmap = {'LBA': 'A', 'LBC': 'C', 'EBA': 'D', 'EBC': 'E'}
    

    digi_histos = [ROOT.TH1F('', 'Run %d: %s%02d MB%02d' % (runnumber, part, mod, (digi/2)+1), 200, -25., 25.) for digi in range(8)]      
  

    badlist = []

    for hist in digi_histos:           
        hist = cleanHisto(hist)


    outfile = open('%s%d_bad_digi.txt' % (outdir, runnumber), 'a')
    
    if os.path.exists(filename):
        print 'Using', filename 
        file = ROOT.TFile.Open(filename)
        if file and not file.IsZombie():
            tree = file.Get('h1000')
            nevents = tree.GetEntries()
            #nevents = 10000  # for testing
    
            for ev in range(1, nevents):

                tree.GetEntry(ev)

                pha = tree.m_cispar[5] 
                dac = tree.m_cispar[6]
                cap = tree.m_cispar[7]
                tfit = getattr(tree, 'tfit%s%02d' % (partmap[part], mod))

                if pha!=0 or dac!=120 or cap!=100:
                    continue
                for pmt in range(48):
                    if checkPMTValid(partnames.index(part), pmt)=='noninst':
                        continue
                    pmt_tfit = tfit[pmt]
                    
                    if (pmt)/12 == 0:
                        
                        if (pmt)%12<6:
                            digi_histos[7].Fill(pmt_tfit, 1.0)
                        else:
                            digi_histos[6].Fill(pmt_tfit, 1.0)
                    elif (pmt)/12 == 1:    
                        
                        if (pmt)%12<6:
                            digi_histos[5].Fill(pmt_tfit, 1.0)
                        else:
                            digi_histos[4].Fill(pmt_tfit, 1.0)
                    elif (pmt)/12 == 2:
                        
                        if (pmt)%12<6:
                            digi_histos[3].Fill(pmt_tfit, 1.0)
                        else:
                            digi_histos[2].Fill(pmt_tfit, 1.0)
                    elif (pmt)/12 == 3:    
                        
                        if (pmt)%12<6:
                            digi_histos[1].Fill(pmt_tfit, 1.0)
                        else:
                            digi_histos[0].Fill(pmt_tfit, 1.0)
            
            file.Close()

    pack = outdir + 'digi_%d_%s%02d.ps' % (runnumber, part, mod)


    badcans = '%s%d_bad_digi.ps' % (outdir, runnumber)
        
    can = SetupDraw()
    can.Print('%s[' % pack)
    can.cd()
    
    for digi in range(8):
        if digi%2==0:
            leg = ROOT.TLegend(0.8,0.8,0.95,1.0)
            leg.SetFillColor(0)
            leg.AddEntry(digi_histos[digi], 'Digi %02d' % (digi+1), 'f')
            if digi_histos[digi].GetMaximum()<digi_histos[digi+1].GetMaximum():
                digi_histos[digi].SetMaximum(digi_histos[digi+1].GetMaximum()+1000)
            digi_histos[digi].Draw()
            can.Modified()
            can.Update()
        elif digi%2==1:

                
            digi_histos[digi].SetFillColor(ROOT.kRed)
            leg.AddEntry(digi_histos[digi], 'Digi %02d' % (digi+1), 'f')
            digi_histos[digi].Draw('same')
            leg.Draw()
            can.Modified()
            can.Update()
            can.Print(pack)
            if abs(digi_histos[digi].GetMean()-digi_histos[digi-1].GetMean())>3:
                outfile.write('%s %02d Digi %d-%d\n' % (part, mod, digi-1, digi))
                can.Print('%s' % badcans)
            leg.Clear()
            can.Clear()
        
            
    can.Print('%s]' % pack)


    return badlist

    
def cleanHisto(hist):

   hist.GetXaxis().CenterTitle()
   hist.GetXaxis().CenterLabels(ROOT.kTRUE)
   hist.GetXaxis().SetTitleOffset(1.2)
   hist.GetXaxis().SetTitle('t_{fit} [ns]')
   #hist.GetXaxis().SetLabelOffset(0.021)
   #hist.GetXaxis().SetLabelSize(0.06)
   hist.GetYaxis().CenterTitle()
   hist.GetYaxis().SetTitleOffset(1.3)
   #hist.GetYaxis().SetLabelOffset(0.015)
   #hist.GetYaxis().SetLabelSize(0.06)
   hist.SetFillColor(ROOT.kGray)

   return hist


def SetupDraw():
    
    ROOT.gROOT.SetStyle("Plain")
    ROOT.gStyle.SetCanvasBorderMode(0)
    ROOT.gStyle.SetPadBorderMode(0)
    ROOT.gStyle.SetTitleX(0.12)
    #ROOT.gStyle.SetTitleY(1.0)
    #ROOT.gStyle.SetTitleW(0.5)
    #ROOT.gStyle.SetTitleH(0.2)
    #ROOT.gStyle.SetStatX(0.64)
    #ROOT.gStyle.SetStatY(0.64)
    ROOT.gStyle.SetLabelFont(42,"XYZ")
    ROOT.gStyle.SetTextFont(42)
    ROOT.gStyle.SetOptStat(111110)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetPalette(1)
    ROOT.gStyle.SetTitleFont(42,"XYZ")
    ROOT.gStyle.SetTitleBorderSize(0)
    ROOT.gStyle.SetPadColor(0)
    ROOT.gStyle.SetCanvasColor(0)
    ROOT.gStyle.SetOptFit(0)
    
    c1 = ROOT.TCanvas()
    c1.SetFrameBorderMode(0)
    c1.SetBorderSize(0)
    c1.SetBorderMode(0)
    c1.SetFillColor(0)
    c1.SetTickx()
    c1.SetTicky()
    
    return c1



def main():


    datadir = '/location/of/ntuples/'
    outdir = '/where/output/will/be/written/'


    if len(sys.argv)<2:
        print "Please pass a run number."
        return True

    runnumber = int(sys.argv[1])
    
    datadir = datadir + str(runnumber) + '/'
    outdir = outdir + str(runnumber) + '/'
    
    if not os.path.exists(outdir):
        os.mkdir(outdir)	

    print 'Using data from', datadir
    print 'Ouput will be sent to', outdir


    
    if not os.path.exists(outdir):
        os.mkdir(outdir)

    ROOT.gROOT.SetBatch()
    parts = ['LBA', 'LBC', 'EBA', 'EBC']
    
    can = SetupDraw()    
    badcans = '%s%d_bad_digi.ps' % (outdir, runnumber)
    can.Print('%s[' % badcans)

    for part in parts:
        for module in range(1,65):
            badlist = readModule(part, module, runnumber, datadir, outdir)

    can.Print('%s]' % badcans)
               

if __name__ == '__main__':
    main()
