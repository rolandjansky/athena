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

    filename = '%stiletb_%d_MonoCis.%s%02d.0.aan.root' % (datadir, runnumber, part, mod)

    partnames = ['LBA', 'LBC', 'EBA', 'EBC']
    partmap = {'LBA': 'A', 'LBC': 'C', 'EBA': 'D', 'EBC': 'E'}
    
    histos = [ROOT.TH1F('%s MOD %02d PMT %02d' % (part, mod, int(pmt + 1)), '%02d' % (int(pmt + 1)), 7, 1, 8) for pmt in range(48)]
    entries = [0 for pmt in range(48)]
    for hist in histos:
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
                if pha!=0 or dac!=120 or cap!=100:
                    continue
                for pmt in range(48):
                    samples = getattr(tree, 'sample%s%02d' % (partmap[part], mod))
                    for samp in range(7):
                        histos[pmt].Fill(samp+1, samples[pmt*9+samp])
                    entries[pmt] = entries[pmt] + 1
     
        
            for pmt in range(48):
                if entries[pmt]==0:
                    continue
                factor = 1./float(entries[pmt])
                histos[pmt].Scale(factor)
                    
            for histo in histos:
                histo, color = checkSamples(histo)
            
            saveHistos(histos, part, mod, runnumber, outdir)
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


def saveHistos(histos, part, mod, runnumber, outdir):
        
    name = '%s%02d.ps' % (part, mod)

    packname = outdir+name
    partnames = ['LBA', 'LBC', 'EBA', 'EBC']

    can = SetupDraw()
    can.Divide(1,1)

    toptitle = ROOT.TLatex(0.45, 0.9, '%s %02d' % (part, mod)) 
    toptitle.SetTextSize(0.05)
    toptitle.SetNDC()
    toptitle.SetTextAlign(12)

    can.Divide(8,7)
        
    for hist in histos:
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
       
    pickfile = open('%s%s%02d.dat' % (outdir, part, mod), 'w')        
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
            
    name = 'samples_%d.ps' % runnumber
    packname = outdir + name

    can = SetupDraw()
    can.Print('%s[' % packname)

    for part in parts:
        for mod in range(1,65):
            pickfile = '%s%s%02d.dat' % (outdir, part, mod)
            if os.path.exists(pickfile):
                pickfile = open(pickfile, 'r')
                page = pickle.load(pickfile)
                page.Print(packname)
                pickfile.close()
                
    can.Print('%s]' % packname)

if __name__ == '__main__':
    main()
