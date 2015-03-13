#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Create beam spot primary vertex plots (e.g. for approved plots).
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id:$'
__usage__   = '%prog [options] nt.root'


# Argument parsing
from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-p', '--plot', dest='plot', default='', help='what to plot')
parser.add_option('-c', '--comment', dest='comment', default=None, help='additional text (use semicolon to indicate line breaks)')
parser.add_option('-m', '--more', dest='more', default='', help='more info (displayed beneath statistics box)')
parser.add_option('-o', '--output', dest='output', default='.gif', help='comma-separated list of output files or formats (default: .gif')
parser.add_option('', '--name', dest='name', default=None, help='base name for plots (default: ntuple name)')
parser.add_option('-n', '--ntracks', dest='ntracks', type='int', default=5, help='Number of tracks/vtx (default: 4)')
parser.add_option('', '--lbmin', dest='lbmin', type='int', default=-1, help='Minimum LB to consider')
parser.add_option('', '--lbmax', dest='lbmax', type='int', default=9999999, help='Maximum LB to consider')
parser.add_option('', '--xmin', dest='xmin', type='float', default=None, help='x axis minimum')
parser.add_option('', '--xmax', dest='xmax', type='float', default=None, help='x axis maximum')
parser.add_option('', '--ymin', dest='ymin', type='float', default=None, help='y axis minimum')
parser.add_option('', '--ymax', dest='ymax', type='float', default=None, help='y axis maximum')
parser.add_option('', '--passed', dest='passed', action='store_true', default=False, help='vertex must have passed athena selection')
parser.add_option('', '--cuts', dest='cuts', default='', help='additional cuts (in addition to vertex type and what can be set with other options)')
parser.add_option('', '--bins', dest='nbins', type='int', default=200, help='number of bins')
parser.add_option('', '--fit', dest='fit', default='', help='fit histogram with function (e.g. gaus) (default: no fit)')
parser.add_option('', '--logy', dest='logy', action='store_true', default=False, help='log scale')
parser.add_option('', '--optstat', dest='optstat', default='emruo', help='default OptStat value (Default: emruo)')
parser.add_option('', '--energy', dest='energy', action='store_true', default=False, help='add energy label')
parser.add_option('', '--public', dest='public', action='store_true', default=False, help='use labelling for public plots')
parser.add_option('', '--prelim', dest='prelim', action='store_true', default=False, help='Add ATLAS Preliminary to figure')
parser.add_option('', '--approval', dest='approval', action='store_true', default=False, help='Label figure ATLAS for approval')
parser.add_option('', '--published', dest='published', action='store_true', default=False, help='add ATLAS to figure')
parser.add_option('', '--canvas', dest='canvas', default='default', help='canvas size: default, page, wide, extrawide or square')
parser.add_option('', '--atlasx', dest='atlasx', type='float', default=None, help='x position for drawing ATLAS label')
parser.add_option('', '--atlasy', dest='atlasy', type='float', default=None, help='y position for drawing ATLAS label')
parser.add_option('', '--atlasdx', dest='atlasdx', type='float', default=None, help='x position offset for drawing Preliminary label')
parser.add_option('-i', '--interactive', dest='interactive', action='store_true', default=False, help='interactive')
parser.add_option('-b', '--batch', dest='batch', action='store_true', default=False, help='run in batch mode')
(options,args) = parser.parse_args()
if len(args) != 1:
    parser.error('wrong number of command line arguments')
ntFile = args[0]
ntName = ntFile.split('/')[-1][:-5] if not options.name else options.name


# Definition of generic plots
plotDef = {
    'x': {'code': 'hist', 'var': 'x', 'hname': 'pvX', 'xmin': -2, 'xmax': 2, 'units': '[mm]'},
    'y': {'code': 'hist', 'var': 'y', 'hname': 'pvY', 'xmin': -2, 'xmax': 2, 'units': '[mm]'},
    'z': {'code': 'hist', 'var': 'z', 'hname': 'pvZ', 'xmin': -500, 'xmax': 500, 'units': '[mm]'},
    'errX': {'code': 'histErr', 'var': 'vxx', 'hname': 'errX', 'xmin': 0, 'xmax': 500, 'units': '[mm]'},
    'errXComp': {'code': 'histErrComp', 'var': 'vxx', 'hname': 'errX', 'xmin': 0, 'xmax': 100, 'units': '[mm]'},
    'errY': {'code': 'histErr', 'var': 'vyy', 'hname': 'errY', 'xmin': 0, 'xmax': 500, 'units': '[mm]'},
    'errZ': {'code': 'histErr', 'var': 'vzz', 'hname': 'errZ', 'xmin': 0, 'xmax': 500, 'units': '[mm]'},
    'ntracks': {'code': 'hist', 'var': 'nTracks', 'hname': 'pvNTracks', 'xmin': 0, 'xmax': 200, 'nbins': 200},
    'xz': {'xmin': -250, 'xmax': 250, 'ymin': -1.5, 'ymax': 2.5},
    'yz': {'xmin': -250, 'xmax': 250, 'ymin': -0.5, 'ymax': 3.5},
    'yx': {'xmin': -1.4, 'xmax': 0.6, 'ymin': 0, 'ymax': 2}
}
def getPlotDef(what,property,default=''):
    try:
        return plotDef[what][property]
    except:
        return default

# Plot defaults
xmin = options.xmin if options.xmin else getPlotDef(options.plot,'xmin',-500)
xmax = options.xmax if options.xmax else getPlotDef(options.plot,'xmax',+500)
ymin = options.ymin if options.ymin else getPlotDef(options.plot,'ymin',-500)
ymax = options.ymax if options.ymax else getPlotDef(options.plot,'ymax',+500)
nbins = getPlotDef(options.plot,'nbins',options.nbins)

# Graphics defaults
if options.atlasx==None:
    if options.published:
        options.atlasx = 0.2
    else:
        options.atlasx = 0.2
if options.atlasy==None:
    options.atlasy = 0.86
if options.atlasdx==None:
    options.atlasdx = 0.115

# Set cuts
cuts = 'vType==1 && lb>=%i && lb<%i' % (options.lbmin,options.lbmax)
if options.ntracks:
    cuts += ' && nTracks>=%i' % options.ntracks
if options.passed:
    cuts += ' && passed==1'
if options.cuts:
    cuts += ' && %s' % options.cuts
print
print 'Using cuts:  ',cuts
print


# Reset DISPLAY if in batch
import os
if options.batch:
    os.unsetenv('DISPLAY')


# Import ROOT (do this only now to avoid conlicts w/OptionParser)
import ROOT
from InDetBeamSpotExample import ROOTUtils
from InDetBeamSpotExample.Utils import getRunFromName
# TODO: Switch to using COOLQuery from COOLUtils for LHC info
from InDetBeamSpotExample.LHCInfoUtils import lhcFillData
from InDetBeamSpotExample.LHCInfoUtils import lhcEnergyData
from InDetBeamSpotExample.BeamSpotData import *
ROOTUtils.setStyle()
ROOT.gStyle.SetPalette(1)  # Better color scheme than default


def drawLabels():
    if options.prelim:
        ROOTUtils.atlasLabel(options.atlasx,options.atlasy,True,offset=options.atlasdx,energy=None)
    if options.approval:
        ROOTUtils.atlasLabel(options.atlasx,options.atlasy,False,offset=options.atlasdx,isForApproval=True,energy=None)
    if options.published:
        ROOTUtils.atlasLabel(options.atlasx,options.atlasy,False,offset=options.atlasdx,energy=None)

    ROOTUtils.drawText(options.atlasx,0.77,0.06,runFillInfo,font=42)
    comment = options.comment if options.comment is not None else '#geq %i tracks/vertex' % (options.ntracks)
    ROOTUtils.drawText(options.atlasx,0.71,0.06,lhcEnergyInfo+comment,font=42)


def drawInfo(c,h,more,x=0.6,size=0.04):
    what = c.GetName()
    info  = 'Entries  %5i' % h.GetEntries()
    info += ';RMS %s   %6.3g mm' % (what[0],h.GetRMS(2))
    info += ';RMS %s   %6.3g mm' % (what[1],h.GetRMS(1))
    ROOTUtils.drawText(x,0.88,size,info,font=102)
    if more:
        ROOTUtils.drawText(x,0.73,size,more,font=102)


class Plots(ROOTUtils.PlotLibrary):

    def hist(self):
        hname = getPlotDef(options.plot,'hname','h')
        var = getPlotDef(options.plot,'var')
        units = getPlotDef(options.plot,'units','')
        ROOT.gStyle.SetOptStat(options.optstat)
        if options.fit:
            ROOT.gStyle.SetOptFit(1111)
        c = ROOTUtils.protect(ROOTUtils.MyCanvas(var,options.canvas))
        c.SetRightMargin(0.14)
        h = ROOTUtils.protect(ROOT.TH1F(hname,'Primary vertex: %s;Primary vertex %s %s' % (var,var,units),nbins,xmin,xmax))
        nt.Draw('%s >> %s' % (var,hname),cuts)
        if options.fit:
            h.Fit(options.fit)
        ROOTUtils.drawText(0.2,0.77,0.06,runFillInfo,font=42)
        comment = options.comment if options.comment is not None else '#geq %i tracks/vertex;%s' % (options.ntracks,options.cuts)
        ROOTUtils.drawText(0.2,0.71,0.06,lhcEnergyInfo+comment,font=42)
        ROOT.gPad.SetLogy(options.logy)
        ROOT.gPad.Update()
        c.save()

    def histErr(self):
        hname = getPlotDef(options.plot,'hname','h')
        var = getPlotDef(options.plot,'var')
        vName = var[1:] # Ugly - should take from plotDef
        ROOT.gStyle.SetOptStat(options.optstat)
        ROOT.gStyle.SetOptFit(0)
        c = ROOTUtils.protect(ROOTUtils.MyCanvas(var,options.canvas))
        c.SetRightMargin(0.14)
        h = ROOTUtils.protect(ROOT.TH1F(hname,'Primary vertex error: %s;Primary vertex error #sqrt{V_{%s}} (#mum);Number of vertices' % (hname,vName),
                                        nbins,xmin,xmax))
        nt.Draw('1e3*sqrt(%s) >> %s' % (var,hname),cuts)
        ROOTUtils.drawText(0.45,0.86,0.06,'Primary Vertex Error #sqrt{V_{%s}}' % vName)
        comment = options.comment if options.comment is not None else '#geq %i tracks/vertex;%s' % (options.ntracks,options.cuts)
        ROOTUtils.drawText(0.45,0.8,0.06,lhcEnergyInfo+comment,font=42)
        ROOT.gPad.SetLogy(options.logy)
        ROOT.gPad.Update()
        c.save()

    def histErrComp(self):
        hname = getPlotDef(options.plot,'hname','h')
        var = getPlotDef(options.plot,'var')
        vName = var[1:] # Ugly - should take from plotDef
        ROOT.gStyle.SetOptStat(0)
        ROOT.gStyle.SetOptFit(0)
        c = ROOTUtils.protect(ROOTUtils.MyCanvas(var,options.canvas))
        c.SetRightMargin(0.14)
        h = ROOTUtils.protect(ROOT.TH1F(hname,'Vertex Error: %s;Vertex error #sqrt{V_{%s}} [#mum];Fraction of entries' % (hname,vName),
                                        nbins,xmin,xmax))
        nt.Draw('1e3*sqrt(%s) >> %s' % (var,hname),cuts)
        norm = h.Integral()
        h.Scale(1./norm)
        pname = hname+'_pileup'
        p = ROOTUtils.protect(ROOT.TH1F(pname,'Vertex Error: %s;Vertex error #sqrt{V_{%s}} [#mum];Fraction of entries' % (hname,vName),
                                        nbins,xmin,xmax))
        pcuts = cuts.replace('vType==1','vType==3')
        print pcuts
        nt.Draw('1e3*sqrt(%s) >> %s' % (var,pname),pcuts,"SAME")
        pnorm = p.Integral()
        p.Scale(1./pnorm)
        p.SetLineColor(4)
        p.SetLineStyle(2)
        ROOTUtils.drawText(0.51,0.86,0.06,'Vertex error #sqrt{V_{%s}}' % vName)
        comment = options.comment if options.comment is not None else '#geq %i tracks/vertex;%s' % (options.ntracks,options.cuts)
        ROOTUtils.drawText(0.51,0.8,0.06,lhcEnergyInfo+comment,font=42)
        legend = ROOT.TLegend(.50,.6,.85,.72)
        legend.AddEntry(h,'Primary vertices','L')
        legend.AddEntry(p,'Pileup vertices','L')
        legend.SetFillColor(0)
        legend.SetBorderSize(0)
        legend.Draw()
        ROOT.gPad.SetLogy(options.logy)
        ROOT.gPad.Update()
        c.save()

    def xz(self):
        #ROOT.gStyle.SetOptStat(1010)       
        ROOT.gStyle.SetOptStat(0)
        c = ROOTUtils.protect(ROOTUtils.MyCanvas('xz',options.canvas))
        c.SetRightMargin(0.14)
        h = ROOTUtils.protect(ROOT.TH2F('pvXZ','Primary vertex: x vs z;Primary vertex z [mm];Primary vertex x [mm]',nbins,xmin,xmax,nbins,ymin,ymax))
        nt.Draw('x:z >> pvXZ',cuts)
        h.Draw('COLZ')
        h.GetYaxis().SetTitleOffset(1.0)
        drawLabels()
        #ROOTUtils.moveStats(h,-0.16,-0.08)
        drawInfo(c,h,options.more)
        ROOT.gPad.Update()
        c.save()

    def yz(self):
        ROOT.gStyle.SetOptStat(0)
        c = ROOTUtils.protect(ROOTUtils.MyCanvas('yz',options.canvas))
        c.SetRightMargin(0.14)
        h = ROOTUtils.protect(ROOT.TH2F('pvYZ','Primary vertex: y vs z;Primary vertex z [mm];Primary vertex y [mm]',nbins,xmin,xmax,nbins,ymin,ymax))
        nt.Draw('y:z >> pvYZ',cuts)
        h.Draw('COLZ')
        h.GetYaxis().SetTitleOffset(1.0)
        drawLabels()
        drawInfo(c,h,options.more)
        ROOT.gPad.Update()
        c.save()

    def yx(self):
        ROOT.gStyle.SetOptStat(0)
        c = ROOTUtils.protect(ROOTUtils.MyCanvas('yx',options.canvas))
        c.SetRightMargin(0.14)
        h = ROOTUtils.protect(ROOT.TH2F('pvYX','Primary vertex: y vs x;Primary vertex x [mm];Primary vertex y [mm]',nbins,xmin,xmax,nbins,ymin,ymax))
        nt.Draw('y:x >> pvYX',cuts)
        h.Draw('COLZ')
        h.GetYaxis().SetTitleOffset(1.0)
        drawLabels()
        drawInfo(c,h,options.more)
        ROOT.gPad.Update()
        c.save()


# Start processing
run = getRunFromName(ntFile)
if options.public:
    runFillInfo = 'Fill %s' % (lhcFillData.get(run,'') if lhcFillData.get(run,'') else '')
else:
    runFillInfo = 'Run %s (LHC Fill %s)' % (run,lhcFillData.get(run,'') if lhcFillData.get(run,'') else '')

if options.energy:
    lhcEnergyInfo = '#sqrt{s} = %s;' % lhcEnergyData.get(run,'') if  lhcEnergyData.get(run,'') else ''
else:
    lhcEnergyInfo = ''

f = ROOT.TFile(ntFile)
if f.Get('Vertices'):
    nt = f.Get('Vertices')
elif f.Get('Beamspot/Vertices'):
    nt = f.Get('Beamspot/Vertices')
#nt = f.Get('Beamspot/Vertices')

ROOTUtils.MyCanvas.saveAsList = options.output.split(',')
ROOTUtils.MyCanvas.autoName = '%s-%%s%%s' % ntName

plots = Plots('Primary Vertex Plots',['hist','histErr'])
plots.plot(getPlotDef(options.plot,'code',options.plot))

if options.interactive:
    os.environ['PYTHONINSPECT'] = '1'
