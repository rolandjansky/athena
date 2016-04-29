#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Command line tool to extract desired vertices from a beam spot fitter
ntuple, store them in a standalone ntuple for use in RooFit, and run
different RooFit beam spot fits.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: fitman.py 721742 2016-02-03 23:34:44Z beringer $'
__usage__   = """%prog [options] [cmd ...]

Commands are:

  extract          Extract selected vertices into vertex ntuple
                   (if given, must be first command)
  stdfit           Standard beam spot fit (except for outlier removal)
  stdfitvzz        Dito, but includes z resolution
  mypdf            Similar to stdfit, but uses dedicated BeamSpotPdf class
                   (fast, but doesn't implement all integrals for plotting yet)
  2gauss           Double Gaussian fit - NOT YET WORKING
  plot[xyz]        Plot data and fit of x, y, and z, or single plot
  data             Histograms of x, y, z
  cov              Histograms of vertex error covariance
  vtxerr           Histograms of vertex errors in x, y, z
  vtxerrxy         Histograms of vertex errors in x, y
"""

import os, sys, re
from array import array
from math import sqrt, floor
import random


#
# Save properly quoted string of original command
#
qargv = [ ]
for s in sys.argv:
    if re.search('\s|\*|\?',s):   # any white space or special characters in word so we need quoting?
        if "'" in s:
            qargv.append('"%s"' % re.sub('"',"'",s))
        else:
            qargv.append("'%s'" % re.sub("'",'"',s))
    else:
        qargv.append(s)
qcmd = ' '.join(qargv)


#
# Argument parsing
#
from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-e', '--srcnt', dest='srcNtName', default=None, help='extract: file name of source beam spot fitter ntuple')
parser.add_option('', '--srctree', dest='srcTreeName', default='Beamspot/Vertices', help='extract: tree name of vertex ntuple (default: Beamspot/Vertices)')
parser.add_option('-l', '--lbl', dest='lbMin', type='int', default=0, help='extract: minimum LB number (inclusive)')
parser.add_option('-u', '--lbu', dest='lbMax', type='int', default=9999, help='extract: maximum LB number (inclusive)')
parser.add_option('', '--bcid', dest='bcid', type='int', default=None, help='extract: BCID')
parser.add_option('-t', '--vtype', dest='vType', type='int', default=None, help='extract: vertex type (1=PriVtx, 3=PileUp)')
parser.add_option('-n', '--ntracks', dest='nTracks', type='int', default=None, help='extract: min number of tracks per vertex')
parser.add_option('-p', '--passed', dest='passed', type='int', default=None, help='extract: passed flag (use only vertices passing InDetBeamSpotFinder selection)')
parser.add_option('', '--rndfrac', dest='rndFrac', type='float', default=None, help='extract: pick given fraction of random vertices after all other selections')
parser.add_option('', '--prescale', dest='prescale', type='int', default=None, help='extract: prescale selected vertices')
parser.add_option('', '--nblk', dest='nblk', type='int', default=None, help='extract: number of selected vertices per block (default: not used)')
parser.add_option('', '--iblk', dest='iblk', type='int', default=0, help='extract: block number to use (default: 0)')
parser.add_option('', '--chi2ndf', dest='chi2ndf', type='float', default=None, help='extract: max chi2/ndf')
parser.add_option('', '--chi2prob', dest='chi2prob', type='float', default=None, help='extract: min chi2 probability')
parser.add_option('', '--maxvtxerr', dest='maxvtxerr', type='float', default=None, help='extract: max allowed transverse vertex error')
parser.add_option('', '--minvtxerr', dest='minvtxerr', type='float', default=None, help='extract: min allowed transverse vertex error')
parser.add_option('', '--xmin', dest='xmin', type='float', default=None, help='extract: lower cut on x')
parser.add_option('', '--xmax', dest='xmax', type='float', default=None, help='extract: upper cut on x')
parser.add_option('', '--ymin', dest='ymin', type='float', default=None, help='extract: lower cut on y')
parser.add_option('', '--ymax', dest='ymax', type='float', default=None, help='extract: upper cut on y')
parser.add_option('', '--zmin', dest='zmin', type='float', default=None, help='extract: lower cut on z')
parser.add_option('', '--zmax', dest='zmax', type='float', default=None, help='extract: upper cut on z')
parser.add_option('-f', '--nt', dest='ntName', default='vtx.root', help='file name vertex ntuple (default: vtx.root)')
parser.add_option('-g', '--nttree', dest='ntTree', default="Vertices", help='name of TTree with vertices')
parser.add_option('', '--plot', dest='plot', default='ALL', help='what to plot (default: ALL)')
parser.add_option('', '--canvas', dest='canvas', default='default', help='canvas size: default, page, wide, extrawide or square')
parser.add_option('', '--lsize', dest='lsize', type='float', default=0.05, help='axis label size')
parser.add_option('', '--hnbins', dest='hnbins', type='int', default=None, help='number of histogram bins')
parser.add_option('', '--hmin', dest='hmin', type='float', default=None, help='histogram x axis minimum')
parser.add_option('', '--hmax', dest='hmax', type='float', default=None, help='histogram x axis maximum')
parser.add_option('', '--hrange', dest='hrange', type='float', default=None, help='histogram x axis range')
parser.add_option('', '--fixK', dest='fixK', type='float', default=None, help='stdfit: fix k factor in fit')
parser.add_option('', '--showbs', dest='showbs', action='store_true', default=False, help='show beam spot')
parser.add_option('', '--showratio', dest='showratio', action='store_true', default=False, help='show ratio data/fit')
parser.add_option('', '--minrange', dest='minrange', type='float', default=0.1, help='min range of x-axis on plots')
parser.add_option('', '--logy', dest='logy', action='store_true', default=False, help='log scale')
parser.add_option('-s', '--stats', dest='stats', action='store_true', default=False, help='show statistics box on plot')
parser.add_option('', '--optstat', dest='optstat', default='emruo', help='default OptStat value (Default: emruo)')
parser.add_option('', '--prelim', dest='prelim', action='store_true', default=False, help='add ATLAS Preliminary to figure')
parser.add_option('', '--approval', dest='approval', action='store_true', default=False, help='Label figure ATLAS for approval')
parser.add_option('', '--published', dest='published', action='store_true', default=False, help='add ATLAS to figure')
parser.add_option('', '--atlasx', dest='atlasx', type='float', default=0.2, help='x position for drawing ATLAS (Preliminary) label')
parser.add_option('', '--atlasy', dest='atlasy', type='float', default=0.85, help='y position for drawing ATLAS (Preliminary) label')
parser.add_option('', '--atlasdx', dest='atlasdx', type='float', default=0.115, help='x position offset for drawing Preliminary label')
parser.add_option('', '--comment', dest='comment', default='', help='additional text (use semicolon to indicate line breaks)')
parser.add_option('', '--commentx', dest='commentx', type='float', default=0.2, help='x position for additional text')
parser.add_option('', '--commenty', dest='commenty', type='float', default=0.69, help='y position for additional text')
parser.add_option('', '--legendx', dest='legendx', type='float', default=0.70, help='x position of legend')
parser.add_option('', '--legendy', dest='legendy', type='float', default=0.93, help='y position of legend')
parser.add_option('', '--name', dest='name', default='fitman', help='name for set of plots (default: fitman)')
parser.add_option('-o', '--output', dest='output', default='.gif', help='comma-separated list of output files or formats (default: .gif)')
parser.add_option('-c', '--cpu', dest='nCpu', type='int', default=2, help='number of CPUs to use for RooFit')
parser.add_option('-v', '--verbose', dest='verbose', action='store_true', default=False, help='verbose output')
parser.add_option('-i', '--interactive', dest='interactive', action='store_true', default=False, help='interactive')
parser.add_option('-b', '--batch', dest='batch', action='store_true', default=False, help='run in batch mode')
(options,args) = parser.parse_args()
if len(args)<1:
    parser.error('wrong number of command line arguments')
if options.srcNtName and not 'extract' in args:
    parser.error('must give extract command when using -e or --srcnt')
if options.interactive:
    os.environ['PYTHONINSPECT'] = '1'
cmdList = args

# Reset DISPLAY if in batch
if options.batch:
    os.unsetenv('DISPLAY')

# Place after option parsing to avoid conflict with ROOT option parsing
import ROOT
import ROOTUtils
ROOTUtils.setStyle()
ROOT.gStyle.SetOptStat(options.optstat)


#
# Utilities
#
def logresult(cmd='',fitresult=None):
    log = open('%s.txt' % options.name,'w')
    if cmd:
        log.write(cmd)
        log.write('\n')
    if fitresult:
        out = ROOT.std.stringstream()
        fitresult.printStream(out,fitresult.defaultPrintContents(''),fitresult.defaultPrintStyle(''))
        log.write(out.str())
        del out
    log.close()


def tmpHist(what,wmin=-1e10,wmax=+1e10):
    vtxData.Draw(what,'%s > %s && %s < %s' % (what,wmin,what,wmax),'goff')
    h = ROOT.gROOT.FindObject('htemp')
    print 'Histogram for %s: mean = %7.4f  rms = %1.4f' % (what,h.GetMean(),h.GetRMS())
    return h.Clone('tmp-'+what)


#
# Definition of plots
#
class Plots(ROOTUtils.PlotLibrary):

    def __init__(self, name = options.name, whatList = []):
        ROOTUtils.PlotLibrary.__init__(self,name)
        self.whatList = whatList
        self.allCanvasSize = 'landscape'
        self.allCanvasDivs = (2,2)
        self.singleCanvasSize = options.canvas

    def prepareMainPad(self):
        mainPad = self.protect( ROOT.TPad("mainPad","mainPad",0.,0.3,1.,1.) )
        mainPad.SetBottomMargin(0.03) # with space between the two pads
        #mainPad.SetBottomMargin(0.00) # w/o space between the two pads
        mainPad.Draw()
        mainPad.cd()
        return mainPad

    def drawRatioPad(self,frame,hmin,hmax):
        ROOT.gROOT.ProcessLine(".L ratioPlot.C+")
        axisTitle = frame.GetXaxis().GetTitle()
        frame.GetXaxis().SetTitle('')
        frame.GetXaxis().SetLabelSize(0.)
        frame.GetYaxis().SetTitleOffset(1.0)
        canvas = ROOT.gPad.GetCanvas()
        canvas.cd()
        ratioPad = self.protect( ROOT.TPad("ratioPad","ratioPad",0.,0.,1.,0.3) )
        ratioPad.SetTopMargin(0.)
        ratioPad.SetBottomMargin(0.3)
        ratioPad.Draw()
        ratioPad.cd()
        dataHist = frame.getObject(0)
        fitCurve = frame.getObject(1)
        #global pull
        #pull = dataHist.makeResidHist(fitCurve,True,True)
        global ratio
        ratio = ROOT.ratioPlot(dataHist,fitCurve)
        ratio.Draw('AP')
        # NOTE: SetRangeUser chooses bin edges, so ratio plot would not be perfectly aligned
        ratio.GetXaxis().SetLimits(hmin,hmax)
        scale = 2.5
        size = scale*options.lsize
        ratio.GetXaxis().SetLabelSize(size)
        ratio.GetXaxis().SetTitle(axisTitle)
        ratio.GetXaxis().SetTitleSize(size)
        ratio.GetXaxis().SetTitleOffset(1.)
        ratio.GetYaxis().SetRangeUser(0.3,1.7)
        ratio.GetYaxis().SetLabelSize(size)
        ratio.GetYaxis().SetTitle('Data / fit')
        #ratio.GetYaxis().SetTitleSize(scale*ratio.GetYaxis().GetTitleSize())
        ratio.GetYaxis().SetTitleSize(size)
        ratio.GetYaxis().SetTitleOffset(1.0/scale)
        ratio.GetYaxis().SetNdivisions(504)
        ratio.SetMarkerSize(0.7)
        line = self.protect( ROOT.TLine(hmin,1,hmax,1) )
        line.Draw()


    def labels(self):
        # ATLAS labels and other text
        if options.prelim:
            ROOTUtils.atlasLabel(options.atlasx,options.atlasy,True,offset=options.atlasdx,energy=8,size=options.lsize)
        if options.approval:
            ROOTUtils.atlasLabel(options.atlasx,options.atlasy,False,offset=options.atlasdx,isForApproval=True,energy=8,size=options.lsize)
        if options.published:
            ROOTUtils.atlasLabel(options.atlasx,options.atlasy,False,offset=options.atlasdx,energy=8,size=options.lsize)
        if options.comment:
            self.protect( ROOTUtils.drawText(options.commentx,options.commenty,0.06,options.comment,font=42) )

    def legend(self,frame):
        legendList = []
        legendList.append([frame.getObject(0),'Data','P'])
        legendList.append([frame.getObject(1),'Fit projection','L'])
        if options.showbs:
            legendList.append([frame.getObject(2),'Beam spot','L'])
        legendMinY = max(options.legendy-options.lsize*1.4*len(legendList),0.2)
        self.protect( ROOTUtils.drawLegend(options.legendx,legendMinY,0.92,options.legendy,legendList,textSize=options.lsize) )

    def hist(self,what):
        if not what:
            return
        if options.hmin is not None or options.hmax is not None or options.hnbins is not None:
            options.hmin = options.hmin if options.hmin is not None else 0.
            options.hmax = options.hmax if options.hmax is not None else 1.
            options.hnbins = options.hnbins if options.hnbins is not None else 40
            h = ROOTUtils.protect( ROOT.TH1F('tmp','%s;%s' % (what,what),
                                             options.hnbins,options.hmin,options.hmax))
            vtxData.Draw('%s >> tmp' % what)
        else:
            vtxData.Draw(what)
        if options.logy:
            ROOT.gPad.SetLogy(options.logy)

    def x(self):
        if options.showratio:
            self.prepareMainPad()
        dx = options.hrange/2 if options.hrange else max(options.minrange/2.,8.*sx.getVal())
        hmin = options.hmin if options.hmin is not None else mx.getVal()-dx
        hmax = options.hmax if options.hmax is not None else mx.getVal()+dx
        global xframe
        xframe = self.protect( x.frame(hmin,hmax) )
        if options.hnbins is not None:
            binning = ROOT.RooBinning(hmin, hmax)
            binning.addUniform(options.hnbins, hmin, hmax)
            data.plotOn(xframe,RooFit.Binning(binning))
            xbinWidth = (hmax-hmin)/float(options.hnbins)
        else:
            data.plotOn(xframe)
            xbinWidth = xframe.GetXaxis().GetBinWidth(1)
        #xframe.GetYaxis().SetTitle("Number of primary vertices per %3.1f #mum" % round(1000.*xbinWidth,1))
        xframe.GetYaxis().SetTitle("Events / %1.0f #mum" % round(1000.*xbinWidth,1))
        xframe.GetYaxis().SetLabelSize(options.lsize)
        xframe.GetYaxis().SetTitleSize(options.lsize)
        if options.stats:
            data.statOn(xframe)
        cov = RooArgSet(vxx,vyy,vxy)
        fitmodel.plotOn(xframe,RooFit.ProjWData(cov,data),
                        RooFit.NumCPU(options.nCpu))
        if options.showbs:
            k.setVal(0.0)
            fitmodel.plotOn(xframe,RooFit.ProjWData(cov,data),
                            RooFit.NumCPU(options.nCpu),
                            RooFit.LineStyle(RooFit.kDashed))
        xframe.Draw()
        if options.logy:
            ROOT.gPad.SetLogy(options.logy)
        self.labels()
        self.legend(xframe)
        if options.showratio:
            self.drawRatioPad(xframe,hmin,hmax)

    def y(self):
        if options.showratio:
            self.prepareMainPad()
        dy = options.hrange/2 if options.hrange else max(options.minrange/2.,8.*sy.getVal())
        hmin = options.hmin if options.hmin is not None else my.getVal()-dy
        hmax = options.hmax if options.hmax is not None else my.getVal()+dy
        global yframe
        yframe = self.protect( y.frame(hmin,hmax) )
        if options.hnbins is not None:
            binning = ROOT.RooBinning(hmin, hmax)
            binning.addUniform(options.hnbins, hmin, hmax)
            data.plotOn(yframe,RooFit.Binning(binning))
            ybinWidth = (hmax-hmin)/float(options.hnbins)
        else:
            data.plotOn(yframe)
            ybinWidth = yframe.GetXaxis().GetBinWidth(1)
        #yframe.GetYaxis().SetTitle("Number of primary vertices per %3.1f #mum" % round(1000.*ybinWidth,1))
        yframe.GetYaxis().SetTitle("Events / %1.0f #mum" % round(1000.*ybinWidth,1))
        yframe.GetYaxis().SetLabelSize(options.lsize)
        yframe.GetYaxis().SetTitleSize(options.lsize)
        if options.stats:
            data.statOn(yframe)
        cov = RooArgSet(vxx,vyy,vxy)
        fitmodel.plotOn(yframe,RooFit.ProjWData(cov,data),
                        RooFit.NumCPU(options.nCpu))
        if options.showbs:
            k.setVal(0.0)
            fitmodel.plotOn(yframe,RooFit.ProjWData(cov,data),
                            RooFit.NumCPU(options.nCpu),
                            RooFit.LineStyle(RooFit.kDashed))
        yframe.Draw()
        if options.logy:
            ROOT.gPad.SetLogy(options.logy)
        self.labels()
        self.legend(yframe)
        if options.showratio:
            self.drawRatioPad(yframe,hmin,hmax)

    def z(self):
        if options.showratio:
            self.prepareMainPad()
        dz = options.hrange/2 if options.hrange else max(options.minrange/2.,8.*sz.getVal())
        hmin = options.hmin if options.hmin is not None else mz.getVal()-dz
        hmax = options.hmax if options.hmax is not None else mz.getVal()+dz
        global zframe
        zframe = self.protect( z.frame(hmin,hmax) )
        if options.hnbins is not None:
            binning = ROOT.RooBinning(hmin, hmax)
            binning.addUniform(options.hnbins, hmin, hmax)
            data.plotOn(zframe,RooFit.Binning(binning))
            zbinWidth = (hmax-hmin)/float(options.hnbins)
        else:
            data.plotOn(zframe)
            zbinWidth = zframe.GetXaxis().GetBinWidth(1)
        #zframe.GetYaxis().SetTitle("Number of primary vertices per %3.1f mm" % round(zbinWidth,1))
        zframe.GetYaxis().SetLabelSize(options.lsize)
        zframe.GetYaxis().SetTitleSize(options.lsize)
        zframe.GetYaxis().SetTitle("Events / %1.0f mm" % round(zbinWidth,1))
        if options.stats:
            data.statOn(zframe)
        cov = RooArgSet(vxx,vyy,vxy)
        fitmodel.plotOn(zframe,RooFit.ProjWData(cov,data),
                        RooFit.NumCPU(options.nCpu))   # could also use e.g. RooFit.Range(-100.,100.)
        if options.showbs:
            k.setVal(0.0)
            fitmodel.plotOn(zframe,RooFit.ProjWData(cov,data),
                            RooFit.NumCPU(options.nCpu),
                            RooFit.LineStyle(RooFit.kDashed))
        zframe.Draw()
        if options.logy:
            ROOT.gPad.SetLogy(options.logy)
        self.labels()
        self.legend(zframe)
        if options.showratio:
            self.drawRatioPad(zframe,hmin,hmax)


#
# If extract command is given, must process it here, before setting up
# RooFit
#
if cmdList[0] == 'extract':
    cmdList.pop(0)
    if not options.srcNtName:
        parser.error('must specify --srcnt for extract command')
    if not os.path.exists(options.srcNtName):
        sys.exit('ERROR: source ROOT file %s not found' % options.srcNtName)
    srcFile = ROOT.TFile(options.srcNtName)
    srcNt = srcFile.Get(options.srcTreeName)
    nEntries = srcNt.GetEntries()
    print '\nCuts for extracting vertices from:   %s:' % options.srcNtName
    print '  %i <= LB <= %i' % (options.lbMin, options.lbMax)
    if options.bcid:
        print '  BCID == %i' % options.bcid
    if options.vType:
        print '  vertex type == %i' % options.vType
    if options.nTracks:
        print '  nTracks >= %i' % options.nTracks
    if options.passed:
        print '  passed == %i' % options.passed
    if options.chi2ndf:
        print '  chi2/ndf <= %s' % options.chi2ndf
    if options.chi2prob:
        print '  chi2prob >= %s' % options.chi2prob
    if options.minvtxerr:
        print '  transverse vertex errors >= %s' % options.minvtxerr
    if options.maxvtxerr:
        print '  transverse vertex errors  < %s' % options.maxvtxerr
    if options.rndFrac is not None:
        print '  using fraction %1.3f of selected vertices' % options.rndFrac
    if options.nblk is not None:
        print '  using block %i of blocks of %i selected vertices' % (options.iblk,options.nblk)
    if options.prescale:
        print '  prescaling selected vertices by factor %i' % options.prescale
    if options.xmin is not None or options.xmax is not None:
        print '  %s <= x <= %s' % (options.xmin,options.xmax)
    if options.ymin is not None or options.ymax is not None:
        print '  %s <= y <= %s' % (options.ymin,options.ymax)
    if options.zmin is not None or options.zmax is not None:
        print '  %s <= z <= %s' % (options.zmin,options.zmax)

    print '\nProcessing %i entries found in source ntuple ...' % nEntries
    dstFile = ROOT.TFile(options.ntName,'recreate')
    dstNt = ROOT.TTree(options.ntTree,'Vertices')
    bx = array('d',[0]); dstNt.Branch('x',bx,'x/D')
    by = array('d',[0]); dstNt.Branch('y',by,'y/D')
    bz = array('d',[0]); dstNt.Branch('z',bz,'z/D')
    bvxx = array('d',[0]); dstNt.Branch('vxx',bvxx,'vxx/D')
    bvyy = array('d',[0]); dstNt.Branch('vyy',bvyy,'vyy/D')
    bvxy = array('d',[0]); dstNt.Branch('vxy',bvxy,'vxy/D')
    bvxz = array('d',[0]); dstNt.Branch('vxz',bvxz,'vxz/D')
    bvyz = array('d',[0]); dstNt.Branch('vyz',bvyz,'vyz/D')
    bvzz = array('d',[0]); dstNt.Branch('vzz',bvzz,'vzz/D')
    nSelected = 0
    nWritten = 0
    if options.maxvtxerr:
        maxVtxErr2 = options.maxvtxerr*options.maxvtxerr
    if options.minvtxerr:
        minVtxErr2 = options.minvtxerr*options.minvtxerr
    for j in xrange(nEntries):
        srcNt.GetEntry(j)
        if srcNt.lb < options.lbMin:
            continue
        if srcNt.lb > options.lbMax:
            continue
        if options.bcid and srcNt.tck!=options.bcid:
            continue
        if options.vType and srcNt.vType!=options.vType:
            continue
        if options.nTracks and srcNt.nTracks<options.nTracks:
            continue
        if options.passed and srcNt.passed!=options.passed:
            continue
        if options.chi2ndf and srcNt.chi2/srcNt.ndf>options.chi2ndf:
            continue
        # NOTE: The strange Prob calculation mimicks what BeamSpotFinder does - see also
        #       FitQuality::numberDoF() and FitQuality::doubleNumberDoF() in
        #       Tracking/ TrkEvent/ TrkEventPrimitives/ TrkEventPrimitives/ FitQuality.h
        if options.chi2prob and ROOT.TMath.Prob(srcNt.chi2,int(floor(srcNt.ndf+0.5)))<options.chi2prob:
            continue
        if options.maxvtxerr and (srcNt.vxx>=maxVtxErr2 or srcNt.vyy>=maxVtxErr2):
            continue
        if options.minvtxerr and (srcNt.vxx<minVtxErr2 or srcNt.vyy<minVtxErr2):
            continue
        if options.xmin is not None and srcNt.x<float(options.xmin):
            continue
        if options.xmax is not None and srcNt.x>float(options.xmax):
            continue
        if options.ymin is not None and srcNt.y<float(options.ymin):
            continue
        if options.ymax is not None and srcNt.y>float(options.ymax):
            continue
        if options.zmin is not None and srcNt.z<float(options.zmin):
            continue
        if options.zmax is not None and srcNt.z>float(options.zmax):
            continue

        # No explicit Chi2/ndf cut - implicit in passed flag
        nSelected += 1
        if options.rndFrac is not None:
            if random.random() > options.rndFrac:
                continue
        if options.prescale and (nSelected % options.prescale):
            continue
        if options.nblk is not None and not (options.iblk*options.nblk <= nSelected < (options.iblk+1)*options.nblk):
            continue
        nWritten += 1

        bx[0] = srcNt.x
        by[0] = srcNt.y
        bz[0] = srcNt.z
        bvxx[0] = srcNt.vxx
        bvyy[0] = srcNt.vyy
        bvxy[0] = srcNt.vxy
        bvxz[0] = srcNt.vxz
        bvyz[0] = srcNt.vyz
        bvzz[0] = srcNt.vzz
        dstNt.Fill()
    dstFile.Write()
    dstFile.Close()
    srcFile.Close()
    print '%i vertices selected, %i vertices after prescales and block selection written to %s\n' % (nSelected,nWritten,options.ntName)


#
# RooFit setup common to all fits
#
from ROOT import kTRUE
from ROOT import RooFit, RooRealVar, RooDataSet, RooFormulaVar, RooArgList, RooArgSet
from ROOT import RooAddPdf

# Observables and data
#x = RooRealVar("x","Primary vertex x [mm]",-3,3)
#y = RooRealVar("y","Primary vertex y [mm]",-3,3)
#z = RooRealVar("z","Primary vertex z [mm]",-300,300)
x = RooRealVar("x","x [mm]",-3,3)
y = RooRealVar("y","y [mm]",-3,3)
z = RooRealVar("z","z [mm]",-300,300)
vxx = RooRealVar("vxx","vxx",-3,3)
vyy = RooRealVar("vyy","vyy",-3,3)
vxy = RooRealVar("vxy","vxy",-3,3)
vxz = RooRealVar("vxz","vxz",-3,3)
vyz = RooRealVar("vyz","vyz",-3,3)
vzz = RooRealVar("vzz","vzz",-3,3)

vtxDataFile = ROOT.TFile(options.ntName)
vtxData = vtxDataFile.Get(options.ntTree)
print '\nUsing %i input vertices from ntuple %s ...\n' % (vtxData.GetEntries(),options.ntName)
hx = tmpHist('x',-3.,3.)
hy = tmpHist('y',-3.,3.)
hz = tmpHist('z',-300.,300.)
data = RooDataSet("data","data", vtxData, RooArgSet(x,y,z,vxx,vyy,vxy,vzz))

# Model (parameters and PDF)
mx = RooRealVar("mx","Mean x",   hx.GetMean(),     -2,     2)
sx = RooRealVar("sx","Sigma x",   hx.GetRMS(),      0,     1)
ax = RooRealVar("ax","Tilt x",              0,  -1e-3,  1e-3)
my = RooRealVar("my","Mean y",   hy.GetMean(),     -2,     2)
sy = RooRealVar("sy","Sigma y",   hy.GetRMS(),      0,     1)
ay = RooRealVar("ay","Tilt y",              0,  -1e-3,  1e-3)
mz = RooRealVar("mz","Mean z",   hz.GetMean(),   -300,   300)
sz = RooRealVar("sz","Sigma z",   hz.GetRMS(),      0,   100)
k  = RooRealVar("k","Error scale factor",   1,    0.0,     3)
if options.fixK:
    k.setVal(options.fixK)
    k.setConstant(kTRUE)
rho = RooRealVar("rho","Correlation coefficient", 0, -1, 1)

# Double Gaussian parameters
f = RooRealVar("f", "Fraction of 1st Gauss",  0.9, 0., 1)
mx2 = RooRealVar("mx2","Mean x",  -0.05,     -2,     2)
sx2 = RooRealVar("sx2","Sigma x",  0.10,      0,     1)
ax2 = RooRealVar("ax2","Tilt x",      0,  -1e-3,  1e-3)
my2 = RooRealVar("my2","Mean y",      1,     -2,     2)
sy2 = RooRealVar("sy2","Sigma y",  0.10,      0,     1)
ay2 = RooRealVar("ay2","Tilt y",      0,  -1e-3,  1e-3)
mz2 = RooRealVar("mz2","Mean z",     -3,   -300,   300)
sz2 = RooRealVar("sz2","Sigma z",    60,      0,   100)
rho2 = RooRealVar("rho2","Correlation coefficient", 0, -1, 1)

# Intermediate parameters
mux = RooFormulaVar("mux","@0+@1*(@2-@3)",RooArgList(mx,ax,z,mz))
muy = RooFormulaVar("muy","@0+@1*(@2-@3)",RooArgList(my,ay,z,mz))
cxx = RooFormulaVar("cxx","@0*@0+@1*@1*@2",RooArgList(sx,k,vxx))
cxy = RooFormulaVar("cxy","@0*@1*@2+@3*@3*@4",RooArgList(rho,sx,sy,k,vxy))
cyy = RooFormulaVar("cyy","@0*@0+@1*@1*@2",RooArgList(sy,k,vyy))
czz = RooFormulaVar("czz","@0*@0",RooArgList(sz))
czzv = RooFormulaVar("czz","@0*@0+@1",RooArgList(sz,vzz))

# Double Gaussian parameters
mux2 = RooFormulaVar("mux2","@0+@1*(@2-@3)",RooArgList(mx2,ax,z,mz))
muy2 = RooFormulaVar("muy2","@0+@1*(@2-@3)",RooArgList(my2,ay,z,mz))
cxx2 = RooFormulaVar("cxx2","@0*@0+@1*@1*@2",RooArgList(sx2,k,vxx))
cxy2 = RooFormulaVar("cxy2","@0*@1*@2+@3*@3*@4",RooArgList(rho,sx2,sy2,k,vxy))
cyy2 = RooFormulaVar("cyy2","@0*@0+@1*@1*@2",RooArgList(sy2,k,vyy))
czz2 = RooFormulaVar("czz","@0*@0",RooArgList(sz))


# Dummy matrix needed by some models
dummyMatrix = ROOT.TMatrixDSym(3)
dummyMatrix[0][0] = 1
dummyMatrix[1][1] = 1
dummyMatrix[2][2] = 1


#
# Command processing (except extract, see above)
#
for cmd in args:
    cmdOk = False

    if cmd == 'none':
        # Useful to end up in interactive mode
        cmdOk = True

    if cmd == 'stdfit':
        cmdOk = True
        ROOT.gROOT.ProcessLine(".L GenGauss3D.cxx+")
        fitmodel = ROOT.GenGauss3D("fitmodel","Full beam spot PDF",
                                   RooArgList(x,y,z),
                                   RooArgList(mux,muy,mz),
                                   cxx,cxy,RooFit.RooConst(0),cyy,RooFit.RooConst(0),czz,
                                   dummyMatrix)
        # NOTE: need RooFit.Save() to get fit results
        fitresult = fitmodel.fitTo(data,RooFit.ConditionalObservables(RooArgSet(vxx,vyy,vxy)),
                                   RooFit.NumCPU(options.nCpu), RooFit.Timer(kTRUE), RooFit.Save())
        logresult(qcmd,fitresult)

    if cmd == 'mypdf':
        cmdOk = True
        ROOT.gROOT.ProcessLine(".L BeamSpotPdf.cxx+")
        fitmodel = ROOT.BeamSpotPdf("fitmodel","Full beam spot PDF",
                                    x,y,z,vxx,vyy,vxy,mx,sx,ax,my,sy,ay,mz,sz,k,rho)
        fitresult = fitmodel.fitTo(data,RooFit.ConditionalObservables(RooArgSet(vxx,vyy,vxy)),
                                   RooFit.NumCPU(options.nCpu), RooFit.Timer(kTRUE), RooFit.Save())
        logresult(qcmd,fitresult)

    if cmd == 'stdfitvzz':
        cmdOk = True
        ROOT.gROOT.ProcessLine(".L GenGauss3D.cxx+")
        fitmodel = ROOT.GenGauss3D("fitmodel","Full beam spot PDF",
                                   RooArgList(x,y,z),
                                   RooArgList(mux,muy,mz),
                                   cxx,cxy,RooFit.RooConst(0),cyy,RooFit.RooConst(0),czzv,
                                   dummyMatrix)
        fitresult = fitmodel.fitTo(data,RooFit.ConditionalObservables(RooArgSet(vxx,vyy,vxy,vzz)),
                                   RooFit.NumCPU(options.nCpu), RooFit.Timer(kTRUE), RooFit.Save())
        logresult(qcmd,fitresult)

    if cmd == '2gauss':
        cmdOk = True
        ROOT.gROOT.ProcessLine(".L GenGauss3D.cxx+")
        ROOT.gROOT.ProcessLine(".L GenGauss3Dclone.cxx+")
        g1 = ROOT.GenGauss3D("g1","Full beam spot PDF",
                             RooArgList(x,y,z),
                             RooArgList(mux,muy,mz),
                             cxx,cxy,RooFit.RooConst(0),cyy,RooFit.RooConst(0),czz,
                             dummyMatrix)
        g2 = ROOT.GenGauss3Dclone("g2","Full beam spot PDF",
                             RooArgList(x,y,z),
                             RooArgList(mux,muy,mz),
                             cxx2,cxy2,RooFit.RooConst(0),cyy2,RooFit.RooConst(0),czz2,
                             dummyMatrix)
        #fitmodel = RooAddPdf("fitmodel","sum",RooArgList(g1,g2),RooArgList(f))
        fitmodel = RooAddPdf("fitmodel","sum",g1,g2,f)

        fitresult = fitmodel.fitTo(data,RooFit.ConditionalObservables(RooArgSet(vxx,vyy,vxy)),
                                   RooFit.NumCPU(options.nCpu), RooFit.Timer(kTRUE), RooFit.Save())
        logresult(qcmd,fitresult)

    if cmd == 'plot':
        cmdOk = True
        plots = Plots(whatList = ['x','y','z'])
        plots.saveAsList = options.output.split(',')
        plots.gPadSaveAsList = options.output.split(',')
        plots.genPlot(options.plot)

    if cmd=='plotx' or cmd=='ploty' or cmd=='plotz':
        cmdOk = True
        plots = Plots()
        plots.saveAsList = options.output.split(',')
        plots.genPlot(cmd[-1])

    if cmd == 'data':
        cmdOk = True
        plots = Plots(whatList = ['x','y','z'])
        plots.saveAsList = ['%s-data%s' % (options.name,ext) for ext in options.output.split(',')]
        #plots.gPadSaveAsList = options.output.split(',')
        plots.genPlot(options.plot,'hist')

    if cmd == 'cov':
        cmdOk = True
        plots = Plots(whatList = ['vxx','vxy','vxz',None,'vyy','vyz',None,None,'vzz'])
        plots.allCanvasDivs = (3,3)
        plots.saveAsList = ['%s-cov%s' % (options.name,ext) for ext in options.output.split(',')]
        #plots.gPadSaveAsList = options.output.split(',')
        plots.genPlot(options.plot,'hist')

    if cmd == 'vtxerr':
        cmdOk = True
        plots = Plots(whatList = ['sqrt(vxx)', 'sqrt(vyy)', 'sqrt(vzz)'])
        plots.allCanvasSize = 'wide'
        plots.allCanvasDivs = (3,1)
        plots.saveAsList = ['%s-vtxerr%s' % (options.name,ext) for ext in options.output.split(',')]
        #plots.gPadSaveAsList = options.output.split(',')
        plots.genPlot(options.plot,'hist')

    if cmd == 'vtxerrxy':
        cmdOk = True
        plots = Plots(whatList = ['sqrt(vxx)', 'sqrt(vyy)'])
        plots.allCanvasSize = 'wide'
        plots.allCanvasDivs = (2,1)
        plots.saveAsList = ['%s-vtxerr%s' % (options.name,ext) for ext in options.output.split(',')]
        #plots.gPadSaveAsList = options.output.split(',')
        plots.genPlot(options.plot,'hist')

    if cmd == 'debug':
        cmdOk = True

    if not cmdOk:
        print 'ERROR: unknown command: %s' % cmd
        sys.exit(1)
