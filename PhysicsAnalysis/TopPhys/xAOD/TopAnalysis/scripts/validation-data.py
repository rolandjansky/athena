#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import random,string

import TopExamples.AtlasStyle

import sys
import os
import re

def thumbnail(plot, width, height):
    img = ROOT.TASImage(plot)
    img.Scale(width, height)
    name = plot.replace('.png', '_tn.png')
    img.WriteImage(name)

def br():
    out.write('<br>\n')

def h1(txt):
    out.write('<h1>%s</h1>\n' % txt)

def h3(txt):
    out.write('<h3>%s</h3>\n' % txt)


# rebin histogram to add underflow and overflow bins
def addUnderOverflowBin(h):
    # see https://root.cern.ch/phpBB3/viewtopic.php?t=6764
    # function to paint the histogram h with extra bins for underflows/overflows
    nx = h.GetNbinsX()+1
    xbins = []
    for r in range(nx+1):
        xbins.append(h.GetBinLowEdge(r))

    xbins.append(xbins[nx-1]+h.GetBinWidth(nx))
    # book a temporary histogram having extra bins, change name to avoid memory leak warnings
    xName="".join(random.SystemRandom().choice(string.ascii_uppercase + string.digits) for _ in range(5))
    htmp = ROOT.TH1D(h.GetName()+xName, h.GetTitle(), nx+1, h.GetBinLowEdge(1)-h.GetBinWidth(1), xbins[nx])
    # Set up plot for markers and errors
    htmp.Sumw2()
    # fill the new histogram including the underflows/overflows
    for r in range(nx+1):
        htmp.SetBinContent(htmp.FindBin(htmp.GetBinCenter(r+1)),h.GetBinContent(r))
        htmp.SetBinError(htmp.FindBin(htmp.GetBinCenter(r+1)),h.GetBinError(r))
    
    htmp.SetEntries(h.GetEffectiveEntries())
    htmp.SetXTitle(h.GetXaxis().GetTitle())
    htmp.SetYTitle(h.GetYaxis().GetTitle())
    return htmp


# create ratio plot
def createRatio(h1, h2):
    hRatio = h1.Clone("hRatio")
    hRatio.SetLineColor(ROOT.kBlack)
    hRatio.SetMarkerStyle(20)
    hRatio.SetTitle("")
    hRatio.SetMinimum(0.8)
    hRatio.SetMaximum(1.20)
    #hRatio.Sumw2() # this should have already been done in addUnderOverflowBin (warnings if uncommented)
    hRatio.SetStats(0)
    hRatio.Divide(h2)
    
    # Adjust y-axis settings
    y = hRatio.GetYaxis()
    y.SetTitle("Ratio")
    y.SetNdivisions(505)
    y.SetTitleSize(20)
    y.SetTitleFont(43)
    y.SetTitleOffset(1.2)
    y.SetLabelFont(43)
    y.SetLabelSize(15)
    
    # Adjust x-axis settings
    x = hRatio.GetXaxis()
    x.SetTitleSize(20)
    x.SetTitleFont(43)
    x.SetTitleOffset(3.5)
    x.SetLabelFont(43)
    x.SetLabelSize(10)
    return hRatio


def createCanvasPads():
    c1.cd()
    # Upper histogram plot is pad1
    pad1.SetBottomMargin(0)
    pad1.Draw()
    # Lower ratio plot is pad2
    pad2.SetTopMargin(0)
    pad2.SetBottomMargin(0.2)
    pad2.SetGridx()
    pad2.SetGridy()
    pad2.Draw()


def plot(name):
    legend = ROOT.TLegend(0.70, 0.80, 0.90, 0.90)
    legend.SetFillColor(0)
    legend.SetFillStyle(0)
    legend.SetBorderSize(0)
    legend.SetTextFont(72)
    legend.SetTextSize(0.04)

    stack = ROOT.THStack()
    h = [] # list of rebinned histograms for the stack

    for i, f in enumerate(files):
        h_orig = f.Get(name)
        if h_orig == None:
            print "Cannot find :",name,"in",f.GetName()
            return None
        h.append(addUnderOverflowBin(h_orig))

        h[i].SetLineColor(histogramStyles[i].m_linecolour)
        h[i].SetLineStyle(histogramStyles[i].m_linestyle)
        h[i].SetLineWidth(histogramStyles[i].m_linewidth)
        h[i].SetFillColor(histogramStyles[i].m_fillcolour)

        version = re.findall("\d+[\.]?", names[i])
        if (version):
            versionName = 'AT-%s'% ''.join(str(e) for e in version)
        else:
            versionName = names[i].replace('.root','')
        legend.AddEntry(h[i], versionName, 'f')

        stack.Add(h[i])

    #for the cutflow histograms print the number of entries on the bin
    txt = 'hist nostack'
    if name.find('cutflow') > -1:
        txt += ' text'

    # Create different pads
    createCanvasPads()
    pad1.cd()

    stack.Draw(txt)
    stack.GetHistogram().GetXaxis().SetTitle(h[0].GetXaxis().GetTitle())
    stack.GetHistogram().GetYaxis().SetTitle(h[0].GetYaxis().GetTitle())
    # Change the size of the numbers in the Y axis and the range
    stack.GetHistogram().GetYaxis().SetLabelSize(0.04)
    Ymax = stack.GetHistogram().GetMaximum()
    stack.SetMaximum(Ymax+0.1*Ymax)    

    legend.Draw('same')

    if len(files) == 2:
        # Create ratio plot
        hRatio = createRatio(h[0],h[1])
        pad2.cd()
        hRatio.Draw()

        thesame = True
        for i in range(1, h[0].GetNbinsX()): #h.Chi2Test better?
            if h[0].GetBinContent(i) != h[1].GetBinContent(i):
                thesame = False

        if not thesame:
            x1 = stack.GetHistogram().GetXaxis().GetXmin()
            x2 = stack.GetHistogram().GetXaxis().GetXmax()

            y1 = stack.GetHistogram().GetMinimum()
            y2 = stack.GetHistogram().GetMaximum()

            pad1.cd()
            box = ROOT.TBox(x1, y1, x2, y2)
            box.SetFillStyle(0)
            box.SetLineColor(ROOT.kRed)
            box.SetLineWidth(3)
            box.Draw('same')


    #write the channel in the top right
    caption = name.split('/')[0]
    lmode = ROOT.TLatex()
    lmode.SetTextAlign(31); #right, obviously
    lmode.SetNDC()
    lmode.SetTextColor(1)
    lmode.SetTextFont(72)
    lmode.SetTextSize(0.04) 
    lmode.DrawLatex(0.95, 0.96, caption)

    filename = name.replace('/','_') + '.png'
    c1.Print(outputdir + '/' + filename)
    thumbnail(outputdir + '/' + filename, 250, 250)
    out.write('<a href="%s"><img src="%s"></a>\n' % (filename, filename.replace('.png', '_tn.png')))


# electron plots
def plotElectrons(channel):
    h3('Electrons')
    plot(channel + '/el_true_type')
    plot(channel + '/el_n')
    br()
    plot(channel + '/el_pt')
    plot(channel + '/el_eta')
    plot(channel + '/el_phi')
    plot(channel + '/el_charge')
    plot(channel + '/el_trkpart_charge')
    br()

    plot(channel + '/el_topoetcone20')
    plot(channel + '/el_topoetcone30')
    plot(channel + '/el_topoetcone40')
    br()

    plot(channel + '/el_ptcone20')
    plot(channel + '/el_ptcone30')
    plot(channel + '/el_ptcone40')
    br()

    plot(channel + '/el_ptvarcone20')
    plot(channel + '/el_ptvarcone30')
    plot(channel + '/el_ptvarcone40')
    br()


# muon plots
def plotMuons(channel):
    h3('Muons')
    plot(channel + '/mu_true_type')
    plot(channel + '/mu_n')
    br()
    plot(channel + '/mu_pt')
    plot(channel + '/mu_eta')
    plot(channel + '/mu_phi')
    plot(channel + '/mu_charge')
    br()

    plot(channel + '/mu_topoetcone20')
    plot(channel + '/mu_topoetcone30')
    plot(channel + '/mu_topoetcone40')
    br()

    plot(channel + '/mu_ptcone20')
    plot(channel + '/mu_ptcone30')
    plot(channel + '/mu_ptcone40')
    br()

    plot(channel + '/mu_ptvarcone20')
    plot(channel + '/mu_ptvarcone30')
    plot(channel + '/mu_ptvarcone40')
    br()


# photon plots
def plotPhotons(channel):
    h3('Photons')
    plot(channel + '/ph_n')
    plot(channel + '/ph_pt')
    plot(channel + '/ph_eta')
    plot(channel + '/ph_phi')
    plot(channel + '/ph_e')
    plot(channel + '/ph_ptvarcone20')
    br()


# jet and large-R jet plots
def plotJets(channel):
    h3('All Jets')
    plot(channel + '/jet_n')
    br()
    plot(channel + '/jet_pt')
    plot(channel + '/jet_eta')
    plot(channel + '/jet_phi')
    plot(channel + '/jet_e')
    br()
    plot(channel + '/jet_mv2c10')
    plot(channel + '/jet_truthflav')
    plot(channel + '/jet_isbtagged_MV2c10_77')
    plot(channel + '/jet_btagSF_MV2c10_77')

    for i in range(4):
        h3('Jet%d' % i)
        plot(channel + '/jet%d_pt' % i)
        plot(channel + '/jet%d_eta' % i)
        plot(channel + '/jet%d_phi' % i)
        plot(channel + '/jet%d_e' % i)
        br()
        plot(channel + '/jet%d_mv2c10' % i)
        plot(channel + '/jet%d_truthflav' % i)
        plot(channel + '/jet%d_isbtagged_MV2c10_77' % i)
        plot(channel + '/jet%d_btagSF_MV2c10_77' % i)

    h3('Large-R jet')
    plot(channel + '/ljet_n')
    plot(channel + '/ljet_pt')
    plot(channel + '/ljet_eta')
    plot(channel + '/ljet_phi')
    plot(channel + '/ljet_m')
    plot(channel + '/ljet_sd12')



# tau plots
def plotTau(channel):
    h3('Tau')
    plot(channel + '/taujet_n')
    plot(channel + '/taujet_pt')
    plot(channel + '/taujet_eta')
    plot(channel + '/taujet_phi')
    plot(channel + '/taujet_charge')



ROOT.gROOT.SetBatch(True)

channels = ['ejets_2015', 'ejets_2016', 'mujets_2015', 'mujets_2016', 'ee_2015', 'ee_2016', 'mumu_2015', 'mumu_2016', 'emu_2015', 'emu_2016']
particles = ['Electrons', 'Muons', 'Photons', 'Jets', 'Tau', 'MET']


if len(sys.argv) < 2:
    print 'Try:'
    print 'python %s outputdir/ primary.root rel_1.root' % sys.argv[0]
    sys.exit()

outputdir = sys.argv[1]

names = sys.argv[2:]
print 'Expecting these files', names

#try to make it, but don't worry too much if it already exists
try:
    os.mkdir(outputdir)
except:
    pass

class HistStyle:
    def __init__(self, linecolour, linestyle, linewidth, fillcolour):
        self.m_linecolour = linecolour
        self.m_linestyle = linestyle
        self.m_linewidth = linewidth
        self.m_fillcolour = fillcolour

histogramStyles = [
    HistStyle(ROOT.kYellow, 1, 3, ROOT.kYellow),
    HistStyle(ROOT.kBlack, 2, 3, 0),
    HistStyle(ROOT.kRed, 3, 3, 0),
    HistStyle(ROOT.kGreen, 4, 3, 0),
    HistStyle(ROOT.kBlue, 5, 3, 0),
    HistStyle(ROOT.kGray, 6, 3, ROOT.kGray),
]

files = []
for name in names:
    files.append(ROOT.TFile.Open(name))

if len(files) > len(histogramStyles):
    print 'You are trying to open more files than we have styles'
    print 'edit the histogramStyles list and add some more'
    sys.exit(1)

out = open(outputdir + '/index.html', 'w')
c1 = ROOT.TCanvas('c1', '', 600, 600)
pad1 = ROOT.TPad("pad1", "pad1", 0, 0.3, 1, 1.0)
pad2 = ROOT.TPad("pad2", "pad2", 0, 0.05, 1, 0.3)

h3('Cutflows')
for channel in channels:
    plot(channel + '/cutflow')

for channel in channels:
    h1(channel)

    h3('Event stuff')
    plot(channel + '/mc_weight')
    plot(channel + '/pileup_weight')
    plot(channel + '/event_mu')
    plot(channel + '/jvt_SF')

    print channel

    if 'Electrons' in particles:
        plotElectrons(channel)

    if 'Muons' in particles:
        plotMuons(channel)

    if 'Photons' in particles:
        plotPhotons(channel)

    if 'Jets' in particles:
        plotJets(channel)

    if 'Taus' in particles:
        plotTaus(channel)


    h3('MET')
    plot(channel + '/event_met_et')
    plot(channel + '/event_met_phi')

out.close()
