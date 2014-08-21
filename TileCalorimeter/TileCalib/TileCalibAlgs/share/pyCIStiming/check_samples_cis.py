#!/bin/env python
# Author: Michael Miller (miller@uchicago.edu)

import os, struct, sys
import time
import pickle
import ROOT
from aux_functions import *

def readModule(part, mod, runnumber, datadir, outdir):

    ROOT.gROOT.SetBatch()
    print 'Checking', part, mod
    
    filename = '%stiletb_%d_CIS.%s%02d.0.aan.root' % (datadir, runnumber, part, mod)

    partnames = ['LBA', 'LBC', 'EBA', 'EBC']
    partmap = {'LBA': 'A', 'LBC': 'C', 'EBA': 'D', 'EBC': 'E'}
    
    histoshg = [ROOT.TH1F('%s MOD %02d PMT %02d HG' % (part, mod, int(pmt + 1)), '%02d' % (int(pmt + 1)), 7, 1, 8) for pmt in range(48)]
    histoslg = [ROOT.TH1F('%s MOD %02d PMT %02d LG' % (part, mod, int(pmt + 1)), '%02d' % (int(pmt + 1)), 7, 1, 8) for pmt in range(48)]
    
    entrieshg = [0 for pmt in range(48)]
    entrieslg = [0 for pmt in range(48)]
    
    for list in [histoshg, histoslg]:
        for hist in list:
            hist.GetXaxis().CenterLabels(ROOT.kTRUE)
            hist = cleanHisto(hist)


    if os.path.exists(filename):
        print 'Using', filename 
        file = ROOT.TFile.Open(filename)
        if file and not file.IsZombie():
            tree = file.Get('h1000')
            nevents = tree.GetEntries()
            #nevents = 100  # for testing
    
            for ev in range(1, nevents):

                tree.GetEntry(ev)

                pha = tree.m_cispar[5] 
                dac = tree.m_cispar[6]
                cap = tree.m_cispar[7]
                if pha!=0 or cap!=100:
                    continue
                if dac==7:
                    for pmt in range(48):
                        sampleshg = getattr(tree, 'sample%s%02dhi' % (partmap[part], mod))
                        for samp in range(7):
                            histoshg[pmt].Fill(samp+1, sampleshg[pmt*9+samp])
                        entrieshg[pmt] = entrieshg[pmt] + 1
                if dac==128:
                    for pmt in range(48):
                        sampleslg = getattr(tree, 'sample%s%02dlo' % (partmap[part], mod))
                        for samp in range(7):
                            histoslg[pmt].Fill(samp+1, sampleslg[pmt*9+samp])
                        entrieslg[pmt] = entrieslg[pmt] + 1
     
        
            for pmt in range(48):
                if entrieshg[pmt]==0:
                    continue
                factor = 1./float(entrieshg[pmt])
                histoshg[pmt].Scale(factor)

            for pmt in range(48):
                if entrieslg[pmt]==0:
                    continue
                factor = 1./float(entrieslg[pmt])
                histoslg[pmt].Scale(factor)
            
                    
            for histo in histoshg:
                histo, color = checkSamples(histo)

            for histo in histoslg:
                histo, color = checkSamples(histo)
             
            
            saveHistos(histoshg, part, mod, 'hg', runnumber, outdir)
            saveHistos(histoslg, part, mod, 'lg', runnumber, outdir)
        file.Close()
    

def checkSamples(histo):

    maxval = 0
    maxvalsamp = 0

    color = 'null'

    for bin in range(1, histo.GetXaxis().GetNbins()):
        val = histo.GetBinContent(bin)
        if val > maxval:
            maxval = val
            maxvalsamp = bin

    if maxvalsamp==4:
        histo.SetFillColor(ROOT.kGreen)
        color = 'green'
    elif maxvalsamp==3 or maxvalsamp==5:
        histo.SetFillColor(ROOT.kYellow)
        color = 'yellow'
    elif maxvalsamp==1 or maxvalsamp==2 or maxvalsamp==6 or maxvalsamp==7:
        histo.SetFillColor(ROOT.kRed)
        color = 'red'

    return histo, color


def saveHistos(histos, part, mod, gain, runnumber, outdir):
        

       
    partnames = ['LBA', 'LBC', 'EBA', 'EBC']

    can = SetupDraw()
    can.Divide(1,1)

    if gain == 'hg':
        gn = 'HG'
    elif gain == 'lg':
        gn = 'LG'
        
    toptitle = ROOT.TLatex(0.42, 0.9, '%s %02d %s' % (part, mod, gn)) 
    toptitle.SetTextSize(0.05)
    toptitle.SetNDC()
    toptitle.SetTextAlign(12)

    can.Divide(8,7)
        
    for hist in histos:
        if gain == 'hg':
            hist.SetMaximum(500)
        elif gain == 'lg':
            hist.SetMaximum(200)
        can.cd(histos.index(hist)+8+1)
        pmt = int(hist.GetTitle()) 
        if pmt == '':
            continue
        if checkPMTValid(partnames.index(part), pmt-1)=='noninst':
            continue
        
        if hist!=0: 
            hist.Draw()
            can.Modified()
            can.Update()

    can.Modified()

    can.cd(0)
    toptitle.Draw()
    can.Modified()
    can.Update()
       
    pickfile = open('%s%s%02d%s.dat' % (outdir, part, mod, gain), 'w')        
    pickle.dump(can, pickfile)
    pickfile.close()


def getPMTTex(pmt):

    pave = ROOT.TPaveText(0.7, 0.6, 0.9, 0.95)
    pave.AddText(pmt) 
    pave.SetFillColor(0)

    pave.SetBorderSize(1)
    pave.SetTextSize(0.15)
    pave.SetTextAlign(12)
    
    return pave
    
def cleanHisto(hist):

   hist.GetXaxis().CenterTitle()
   hist.GetXaxis().SetTitleOffset(1.4)
   hist.GetXaxis().SetLabelOffset(0.021)
   hist.GetXaxis().SetLabelSize(0.06)
   hist.GetYaxis().CenterTitle()
   hist.GetYaxis().SetTitleOffset(1.3)
   hist.GetYaxis().SetLabelOffset(0.015)
   hist.GetYaxis().SetLabelSize(0.06)
   hist.SetFillColor(ROOT.kGray)

   return hist


def SetupDraw():
    
    ROOT.gROOT.SetStyle("Plain")
    ROOT.gStyle.SetCanvasBorderMode(0)
    ROOT.gStyle.SetPadBorderMode(0)
    ROOT.gStyle.SetTitleX(0.75)
    ROOT.gStyle.SetTitleY(1.0)
    ROOT.gStyle.SetTitleW(0.25)
    ROOT.gStyle.SetTitleH(0.25)
    #ROOT.gStyle.SetStatX(0.64)
    #ROOT.gStyle.SetStatY(0.64)
    ROOT.gStyle.SetLabelFont(42,"XYZ")
    ROOT.gStyle.SetTextFont(42)
    #ROOT.gStyle.SetOptStat(111110)
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetPalette(1)
    ROOT.gStyle.SetTitleFont(42,"XYZ")
    ROOT.gStyle.SetTitleBorderSize(1)
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


    if len(sys.argv)!=2:
        print "Please pass a run number."
        return True

    runnumber = int(sys.argv[1])

    datadir = '/location/of/ntuples'
    outdir = '/where/output/will/be/sent/'   
    
    datadir = datadir + str(runnumber) + '/'
    outdir = outdir + str(runnumber) + '/'

    if not os.path.exists(outdir):
        os.mkdir(outdir)

    print 'Using data from', datadir
    print 'Ouput will be sent to', outdir

    parts = ['LBA', 'LBC', 'EBA', 'EBC']
        
    for part in parts:
        for mod in range(1,65):
            readModule(part, mod, runnumber, datadir, outdir)
            
    namehg = 'samples_HG_%d.ps' % runnumber
    namelg = 'samples_LG_%d.ps' % runnumber
    packnamehg = outdir + namehg
    packnamelg = outdir + namelg
    
    can = SetupDraw()
    can.Print('%s[' % packnamehg)

    for part in parts:
        for mod in range(1,65):
            pickfile = '%s%s%02dhg.dat' % (outdir, part, mod)
            if os.path.exists(pickfile):
                pickfile = open(pickfile, 'r')
                page = pickle.load(pickfile)
                page.Print(packnamehg)
                pickfile.close()
                
    can.Print('%s]' % packnamehg)
    
    can = SetupDraw()
    can.Print('%s[' % packnamelg)

    for part in parts:
        for mod in range(1,65):
            pickfile = '%s%s%02dlg.dat' % (outdir, part, mod)
            if os.path.exists(pickfile):
                pickfile = open(pickfile, 'r')
                page = pickle.load(pickfile)
                page.Print(packnamelg)
                pickfile.close()
                
    can.Print('%s]' % packnamelg)


if __name__ == '__main__':
    main()
