#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import optparse
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetTitleFont(42,"xy")
ROOT.gStyle.SetTitleFont(42,"t")

ScalarColor     = ROOT.kPink+4
DarkPhotonColor = ROOT.kAzure-3
ElectronColor   = ROOT.kRed-3
MuonColor       = ROOT.kBlue-3
PionColor       = ROOT.kGreen-3

def make2DPdf(name, hist, prefix):
    if not hist:
        return
    canvas = ROOT.TCanvas('whoCares'+name , 'name', 1600,1600)
    canvas.cd(0)
    hist.Draw('COLZ')
    if prefix != '':
        prefix += '_'
        pass
    canvas.SaveAs(prefix+name+'.pdf')

def makePdf(name, hists, prefix):
    if len(hists) == 0:
        return
    if prefix != '' and prefix[-1] != '_':
        prefix += '_'
        pass

    if isinstance(hists, list):
        maxY = -999
        maxEntries = -999
        for hist in hists:
            hist.Scale( 1./float(hist.GetEntries()) )
            hist.SetYTitle('Normalized to Unity')
            if hist.GetMaximum() > maxY and ('Electron' in hist.GetName() or 'Pion' in hist.GetName() or 'Muon' in hist.GetName()):
                maxY = hist.GetMaximum()
                maxEntries = hist.GetEntries()
                pass
            hist.SetLineWidth(2)
            pass
    
        legend = ROOT.TLegend(.6, .65, .85, .89)
        legend.SetFillColor(0)
        legend.SetBorderSize(0)
        legend.SetTextFont(102)
        legend.SetTextSize(0.04)
        
        canvas = ROOT.TCanvas('whoCares'+name , 'name', 1600,1200)
        canvas.cd(0)
        scalarhist = None
        darkphotonhist = None
        for index,hist in enumerate(hists):
            #print hist.GetName()
            drawOptions = 'HIST'
            if index!=0:
                drawOptions += 'SAME'
                pass

            if 'Electron' in hist.GetName():
                legend.AddEntry(hist, '#splitline{Electrons}{'+str(int(hist.GetEntries()))+' events}', 'f')
                hist.SetLineColor(ElectronColor)
                hist.SetMaximum(1.2*maxY )
                pass
            elif 'Muon' in hist.GetName():
                legend.AddEntry(hist, '#splitline{Muons}{'+str(int(hist.GetEntries()))+' events}', 'f')
                hist.SetLineColor(MuonColor)
                hist.SetMaximum(1.2*maxY )
                pass
            elif 'Pion' in hist.GetName():
                legend.AddEntry(hist, '#splitline{Pions}{'+str(int(hist.GetEntries()))+' events}', 'f')
                hist.SetLineColor(PionColor)
                hist.SetMaximum(1.2*maxY )
                pass
            elif 'DarkPhoton' in hist.GetName() or 'darkPhoton' in hist.GetName():
                hist.SetLineColor(DarkPhotonColor)
                darkphotonhist = hist
                continue
                pass
            elif 'Scalar' in hist.GetName() or 'scalar' in hist.GetName():
                hist.SetLineColor(ScalarColor)
                scalarhist = hist
                continue
                pass
            hist.Draw(drawOptions)
            pass
        if legend.GetNRows():
            legend.Draw()
            pass
        canvas.SaveAs(prefix+name+'.pdf')
        if darkphotonhist:
            if not 'darkPhoton' in name:
                name = 'darkPhoton'+name
                pass
            makePdf(name, darkphotonhist, prefix)
            pass
        if scalarhist:
            if not 'scalar' in name:
                name = 'scalar'+name
                pass
            makePdf(name, scalarhist, prefix)
            pass
    else:
        canvas = ROOT.TCanvas('whoCares2'+name , 'name', 1600,1200)
        canvas.cd(0)
        hists.Draw('HIST')
        canvas.SaveAs(prefix+name+'.pdf')


if __name__=='__main__':
    ROOT.gROOT.SetBatch(1)
    parser = optparse.OptionParser()
    parser.add_option('-f', '--file',   dest='hist_file_in',             help='path to ROOT file with histograms' )
    parser.add_option('-p', '--prefix', dest='prefix',       default='', help='prefix to all output pdfs' )
    #parser.add_argument('-b')

    (options, args) = parser.parse_args()

    if not options.hist_file_in:
        print 'ERROR: input file (./plotmaker.py -f <path to ROOT file>) needed'
        exit(0)
        pass

    hist_file = ROOT.TFile.Open(options.hist_file_in, 'r')
    if not hist_file:
        print 'Path provided to ROOT file is incorrect. Or something is wrong with the file.'
        pass


    hist1DDict = { 'darkPhotonPt': [], 'darkPhotonEta':[], 'darkPhotonPhi': [], 'Polarization': [],
                   'scalarPt': [], 'scalarEta':[], 'scalarPhi': [],
                   'decayR1D': [], 'decayZ1D':[],
                   'OpeningR':[], 'OpeningPhi':[], 'OpeningEta':[],
                   'ptBalance':[], 'subLeadingPt':[], 'leadingPt':[]}

    hist2DDict = { 'decayZR2D': None, 'decayEtaR2D':None }

    keys = hist_file.GetListOfKeys()
    for key in keys:
        keyName =  key.GetName()
        for histName in hist1DDict.keys():
            if histName in keyName:
                hist = ROOT.TH1F()
                hist_file.GetObject(keyName, hist)
                if hist.GetEntries() == 0:
                    continue
                hist1DDict[histName].append(hist)
                pass
            continue
        pass

    for key in keys:
        keyName = key.GetName()
        for histName in hist2DDict.keys():
            if histName in keyName:
                hist = ROOT.TH2F()
                hist_file.GetObject(keyName, hist)
                if hist.GetEntries() == 0:
                    continue
                hist2DDict[histName] = hist
                pass
            continue
        pass

    for name,hist in hist1DDict.iteritems():
        makePdf(name, hist, options.prefix)
        pass

    for name,hist in hist2DDict.iteritems():
        make2DPdf(name, hist, options.prefix)
