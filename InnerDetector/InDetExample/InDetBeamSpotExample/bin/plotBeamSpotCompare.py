#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Compare two sets of beam spots for the same run; can read results from
ntuples,CSV, or from COOL; use BeamSpotData class
"""
__author__  = 'Martina Hurwitz'
__version__ = '$Id $'
__usage__   = '%prog [options] file1/tag1 file2/tag2'

import sys, os
import re
import time
from math import *
from array import array

from InDetBeamSpotExample.BeamSpotData import *
from InDetBeamSpotExample.Utils import getRunFromName

# Define plot properties for different ntuple variables
varDef = {
    'nEvents': {'xtit': 'Luminosity Block Number', 'ytit': '#Delta(N_{vert})', 'ytit2': 'Number of vertices', 'title': 'Difference in number of vertices', 'bigchange': 300, 'cannr':2},
    'k': {'xtit': 'Luminosity Block Number', 'ytit': '#Delta(k)', 'ytit2': 'Error scale factor k', 'title': 'Difference in error scale factor k', 'bigchange': 0.2, 'cannr':3},
    'posX': {'xtit': 'Luminosity Block Number', 'ytit': '#Delta(x) [mm]', 'ytit2': 'X position [mm]', 'title': 'Difference in beamspot x', 'bigchange': 10, 'cannr':7},
    'posY': {'xtit': 'Luminosity Block Number', 'ytit': '#Delta(y) [mm]', 'ytit2': 'Y position [mm]', 'title': 'Difference in beamspot y', 'bigchange': 10, 'cannr':8},
    'posZ': {'xtit': 'Luminosity Block Number', 'ytit': '#Delta(z) [mm]', 'ytit2': 'Z position [mm]', 'title': 'Difference in beamspot z', 'bigchange': 3, 'cannr':9},
    'sigmaX': {'xtit': 'Luminosity Block Number', 'ytit': '#Delta(#sigma_{x}) [mm]', 'ytit2': '#sigma_{x} [mm]', 'title': 'Difference in beamspot #sigma_{x}', 'bigchange': 3,'cannr':13},
    'sigmaY': {'xtit': 'Luminosity Block Number', 'ytit': '#Delta(#sigma_{y}) [mm]', 'ytit2': '#sigma_{y} [mm]', 'title': 'Difference in beamspot #sigma_{y}', 'bigchange': 3,'cannr':14},
    'sigmaZ': {'xtit': 'Luminosity Block Number', 'ytit': '#Delta(#sigma_{z}) [mm]', 'ytit2': '#sigma_{z} [mm]', 'title': 'Difference in beamspot #sigma_{z}', 'bigchange': 2, 'cannr':15},
    'tiltX': {'xtit': 'Luminosity Block Number', 'ytit': '#Delta(Tilt_{x-z}) [rad]', 'ytit2': 'Tilt in x-z [rad]', 'title': 'Difference in beamspot tilt in x-z', 'bigchange': 0.15,'cannr':19},
    'tiltY': {'xtit': 'Luminosity Block Number', 'ytit': '#Delta(Tilt_{y-z}) [rad]', 'ytit2': 'Tilt in y-z [rad]', 'title': 'Difference in beamspot tilt in y-z', 'bigchange': 0.15,'cannr':20},
    'rhoXY': {'xtit': 'Luminosity Block Number', 'ytit': '#Delta(#rho_{xy})', 'ytit2': '#rho_{xy}', 'title': 'Difference in #rho', 'bigchange': 0.15,'cannr':21}
        }
def getVarDef(what,property,default=''):
    try:
        return varDef[what][property]
    except:
        return default
    __usage__ += '\n\nPossible variables to plot are:\n'
    __usage__ += ' '.join(sorted(ntDef.keys()))
                    
# Argument parsing
from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-b', '--batch', dest='batch', action='store_true', default=False, help='run in batch mode')
parser.add_option('-o', '--output', dest='output', default='.gif', help='comma-separated list of output files or formats (default: .gif)')
parser.add_option('-r', '--runNumber', dest='runNumber', default=0, help='run number')
parser.add_option('', '--rl', dest='runMin', type='int', default=0, help='minimum run number (inclusive)')
parser.add_option('', '--ru', dest='runMax', type='int', default=0, help='maximum run number (inclusive)')
parser.add_option('-p', '--plot', dest='plot', default='', help='quantity to plot, only make one plot')
parser.add_option('', '--finder1', dest='finder1', action='store_true', default=False, help='First ntuple is BeamSpotFinder ntuple')
parser.add_option('', '--finder2', dest='finder2', action='store_true', default=False, help='Second ntuple is BeamSpotFinder ntuple')
parser.add_option('', '--online1', dest='online1', action='store_true', default=False, help='First COOL tag is for online folder')
parser.add_option('', '--online2', dest='online2', action='store_true', default=False, help='Second COOL tag is for online folder')
parser.add_option('', '--config', dest='config', default='', help='Use known configuration (OnlineOffline or Reproc)')
parser.add_option('', '--plotHistos', dest='plotHistos', action='store_true', default=False, help='Plot histograms instead of graphs')
parser.add_option('', '--label1', dest='label1', default='', help='Description of first argument')
parser.add_option('', '--label2', dest='label2', default='', help='Description of second argument')
parser.add_option('', '--lbmin', dest='lbmin', type='int', default=0, help='Miminum LB used in comparison')
parser.add_option('', '--lbmax', dest='lbmax', type='int', default=0, help='Maximum LB used in comparison')
parser.add_option('', '--smallChanges', dest='smallChanges', action='store_true', default=False, help='Print out changes larger than 0.1 percent')
parser.add_option('', '--RooFit', dest='RooFit',action='store_true',default=False,help='Compare Run 1 Fit Method with RooFit result in same file')
parser.add_option('', '--status', dest='status',action='store',default=59,help='default fit status to use for first file')
parser.add_option('', '--multicanv', dest='multicanv', action='store_true',default=False,help="create multiple canvases")
parser.add_option('', '--outtag', dest='outtag', action='store', default='', help="string to prepend to output file names.")
parser.add_option('', '--html', dest='html', action='store_true', default=False, help="create an HTML page to see plots more easily.  Only really useful with multicanv.")
parser.add_option('', '--htmltag', dest='htmltag', action='store', default='', help="tag to distinguish different HTML pages")
(options,args) = parser.parse_args()

tag1=''
tag2=''

if options.RooFit:
    if len(args) != 1:
        parser.error('wrong number of command line arguments')
    tag1=args[0]
    tag2=args[0]

else:
    if len(args) < 2:
        parser.error('wrong number of command line arguments')
    tag1=args[0]
    tag2=args[1]

if options.runNumber :
    runNumber = int(options.runNumber)
    print 'Doing comparison for run %i' % runNumber
elif options.config=='OnlineOffline':
    runNumber = getRunFromName(tag1,'0',True)
    print 'Doing comparison for run %i' % runNumber
elif options.config=='Reproc':
    runNumber = getRunFromName(tag2,'0',True)
    print 'Doing comparison for run %i' % runNumber
else :
    runNumber = 0
    #print 'Doing comparison for all runs in %s' % tag1

# Precision
ndp=5
ndptilt=8

# Setup ROOT
if options.batch:
    ROOT.gROOT.SetBatch(1)
import ROOT
from InDetBeamSpotExample import ROOTUtils
ROOTUtils.setStyle()

# Flag for DB info
fromDB = False
fromCSV = False

# Determine type of input
if tag1.find('.root') > -1:
    # It's a root file
    if options.finder1:
        BSData1 = BeamSpotFinderNt(tag1)
    else:
        BSData1 = BeamSpotNt(tag1)
elif tag1.find('.csv') > -1:
    # comma-separated format
    BSData1 = BeamSpotCSV(tag1)
    fromCSV = True
else :
    # It's a COOL tag
    if options.online1:
        BSData1 = BeamSpotCOOL(tag1, 'COOLONL_INDET/CONDBR2', '/Indet/Onl/Beampos') #, "sqlite://;schema=beamspot.db;dbname=BEAMSPOT")
    else:
        BSData1 = BeamSpotCOOL(tag1)
    fromDB = True
        
if tag2.find('.root') > -1:
    # root file
    if options.finder2:
        BSData2 = BeamSpotFinderNt(tag2)
    else:
        BSData2 = BeamSpotNt(tag2)
elif tag2.find('.csv') > -1:
    # CSV file
    BSData2 = BeamSpotCSV(tag2)
    fromCSV = True
else :
    # COOL
    if options.online2:
        BSData2 = BeamSpotCOOL(tag2, 'COOLONL_INDET/CONDBR2', '/Indet/Onl/Beampos')
    else:
        BSData2 = BeamSpotCOOL(tag2)
    fromDB = True
        
# What variables to look at
varColl = []
if options.plot:
    doOnePlot = True
    varColl.append(options.plot)
else:
    doOnePlot = False
    varColl = ['posX', 'posY', 'posZ', 'sigmaX', 'sigmaY', 'sigmaZ', 'tiltX', 'tiltY']
    if not fromCSV:
        varColl.append('rhoXY')
    if not fromDB and not fromCSV:
        varColl = ['nEvents', 'k', 'posX', 'posY', 'posZ', 'sigmaX', 'sigmaY', 'sigmaZ', 'tiltX', 'tiltY', 'rhoXY']
            
# Now run over them; take diff and make TGraphs at same time
            
if runNumber != 0 :
    BSData1.runMin = runNumber
    BSData1.runMax = runNumber
    BSData2.runMin = runNumber
    BSData2.runMax = runNumber

if options.runMin != 0 :
    BSData1.runMin = options.runMin
    BSData2.runMin = options.runMin

if options.runMax != 0 :
    BSData1.runMax = options.runMax
    BSData2.runMax = options.runMax

if options.lbmin != 0:
    BSData1.lbmin = options.lbmin
    BSData2.lbmin = options.lbmin

if options.lbmax != 0:
    BSData1.lbmax = options.lbmax
    BSData2.lbmax = options.lbmax


BS2_status = options.status
if options.RooFit:
    if options.status==107:
        BS2_status=59
    else:
        BS2_status=107
if not options.config=='OnlineOffline':
    BSData2.statusList=[BS2_status]
BS2Dict = BSData2.getDataCache()
    
if(len(BS2Dict)==0):
    if options.config == 'OnlineOffline':
        print 'No entries found in Online DB!'
    elif options.config == 'Reproc':
        print 'No entries in reprocessed sample!'
    else:
        print 'No entries found for second tag!'
        
    sys.exit(1)

numNew = 0
numOld = 0

x1 = array('d')
ex1 = array('d')
x2 = array('d')
ex2 = array('d')
xd = array('d')
exd = array('d')

y1Dict = {}
ey1Dict = {}
y2Dict = {}
ey2Dict = {}
ydDict = {}
eydDict = {}

for BS1 in BSData1:
    pass1 = False
    if BS1.status == options.status: pass1 = True

    run = BS1.run
    lbmin = BS1.lbStart
    lbmax = BS1.lbEnd
    # This is taken into account correctly for COOL, not for ROOT
    if ".csv" in tag1 or ".root" in tag1:
        lbmax+=1
    
    if lbmax-lbmin > 100:
        lbmax = lbmin+10

    if options.lbmin != 0 and lbmin < options.lbmin:
        lbmin = options.lbmin
    if options.lbmax != 0 and lbmax > options.lbmax:
        lbmax = options.lbmax
        
    for lb in range(lbmin, lbmax) :

        if pass1:
            x1.append(lb)
            ex1.append(0.0)
            
            for var in varColl:
                if not var in y1Dict:
                    y1Dict[var] = array('d')
                    ey1Dict[var] = array('d')
                y1Dict[var].append(getattr(BS1, var))
                try:
                    ey1Dict[var].append(getattr(BS1, var+'Err'))
                except:
                    ey1Dict[var].append(0.0)
                    
        if run in BS2Dict:
            if lb in BS2Dict[run]:
                BS2 = BS2Dict[run][lb]
                
                pass2 = False
                if options.RooFit:
                    if options.status==59:
                        if BS2.status == 107: pass2 = True
                    elif options.status==107:
                        if BS2.status == 59: pass2 = True
                elif options.config=='OnlineOffline':
                    if (BS2.status%8)==7: pass2=True
                else:
                    if BS2.status == options.status: pass2 = True

                if pass1 and not pass2:
                    print "Warning: run %s lumiBlock %s doesn't have good fit anymore" %(run,lb)
                    numOld += 1
                    
                if pass2 and not pass1: numNew += 1
                if not pass1 or not pass2: continue
                
                xd.append(lb)
                exd.append(0.0)
                
                for var in varColl:
                    if not var in ydDict:
                        ydDict[var] = array('d')
                        eydDict[var] = array('d')
                    diff = getattr(BS1, var) - getattr(BS2, var)
                    try:
                        ediff = max(getattr(BS1, var+'Err'), getattr(BS2, var+'Err'))
                    except:
                        ediff = 0.0
                    ydDict[var].append(diff)
                    eydDict[var].append(ediff)

                    try:
                        percdiff = diff / max(getattr(BS1, var), getattr(BS2, var))
                    except:
                        percdiff = 0
                    
                    try:
                        sigmaChange = diff/ediff
                    except:
                        sigmaChange = 0
                        
                    if sigmaChange > 3 and diff > getVarDef(var, 'bigchange') :
                        print "WARNING: Big change in value of %s for lumiBlock %i" % (var, lb)
                        print "      change in value is %f microns, or %f sigma" % (diff, diff/ediff)

                    if options.smallChanges and percdiff > 0.001 :
                        print "WARNING: value of %s in run %s in lumiBlock %i changed by more than 0.1 percent" % (var, run, lb)

                            
            else:
                if pass1:
                    print "Warning: run %s lumiBlock %s doesn't have beamspot anymore" %(run, lb)
                    numOld += 1
                    
for run in BS2Dict:
    for lb in BS2Dict[run]:
        BS2 = BS2Dict[run][lb]
        
        if options.RooFit:
            if options.status==59:
                if BS2.status == 107: pass2 = True
            elif options.status==107:
                if BS2.status == 59: pass2 = True
        elif options.config=="OnlineOffline":
            if (BS2.status%8)==7: pass2=True
        else:
            if BS2.status == options.status: pass2 = True

        x2.append(lb)
        ex2.append(0.0)
        
        for var in varColl:
            if not var in y2Dict:
                y2Dict[var] = array('d')
                ey2Dict[var] = array('d')
            y2Dict[var].append(getattr(BS2, var))
            try:
                ey2Dict[var].append(getattr(BS2, var+'Err'))
            except:
                ey2Dict[var].append(0.0)
                
print "Number of LBs where beam spot is good but was bad before: %i" % numNew
print "Number of LBs where beam spot is bad but was good before: %i" % numOld

if len(xd)==0:
    print "ERROR: No overlapping LB range found"
    sys.exit(1)

# Plotting
if doOnePlot:
    canvas = ROOT.TCanvas('BeamSpotComparison', 'BeamSpotComparison', 600, 500)
    if options.plotHistos:
        ROOT.gStyle.SetOptStat(1110)
        ROOT.gStyle.SetStatW(0.3)
        ROOT.gStyle.SetStatH(0.2)
    else:
        canvas.Divide(1,2,0,0)
elif options.plotHistos:
    canvas = ROOT.TCanvas('BeamSpotComparison', 'BeamSpotComparison', 750, 1000)
    canvas.Divide(3, 4)
elif options.multicanv:
    canvases = {}
    ratiopad = {}
    primarypad = {}
    zeroline = {}
else:
    canvas = ROOT.TCanvas('BeamSpotComparison', 'BeamSpotComparison', 750, 1400)
    ROOT.gPad.SetLeftMargin(0.0)
    ROOT.gPad.SetBottomMargin(0.0)
    canvas.Divide(3, 8, 0.01, 0)


ivar = 0
graphColl = []
dummyColl = []
histColl = []

for var in varColl:
    #for var in ['posX'] :
    
    gr1 = ROOT.TGraphErrors(len(x1), x1, y1Dict[var], ex1, ey1Dict[var])
    graphColl.append(gr1)
    gr2 = ROOT.TGraphErrors(len(x2), x2, y2Dict[var], ex2, ey2Dict[var])
    graphColl.append(gr2)
    grdiff = ROOT.TGraphErrors(len(xd), xd, ydDict[var], exd, eydDict[var])
    graphColl.append(grdiff)
    
    xmin = min(min(x1), min(x2))
    xmax = max(max(x1), max(x2))
    ymin = min(min(y1Dict[var]), min(y2Dict[var]))
    ymax = max(max(y1Dict[var]), max(y2Dict[var]))
    
    h = (ymax-ymin)
    ymin -= 0.25*h
    ymax += 0.25*h
    h = (xmax-xmin)
    xmin -= 0.05*h

    diffmin = min(ydDict[var])
    diffmax = max(ydDict[var])
    h = diffmax - diffmin
    diffmin -= 0.25*h
    diffmax += 0.25*h
    
    histo = ROOT.TH1F(var, var, 40, diffmin, diffmax)
    histo.GetXaxis().SetTitle(getVarDef(var,'ytit'))
    histo.GetYaxis().SetTitle('LumiBlocks')
    
    for i in xrange(0, len(ydDict[var])):
        histo.Fill(ydDict[var][i])
        
    histColl.append(histo)
    
    if options.plotHistos:
        if doOnePlot:
            histo.GetXaxis().SetTitleOffset(1.7)
            histo.DrawCopy()
        else:
            canvas.cd(ivar+2)
            histo.DrawCopy()
    else:
        hdummy = ROOT.TH2D('hd'+var, 'hd'+var, 10, xmin, xmax, 10, ymin, ymax)
        hdummy.SetTitle("")
        hdummy.GetYaxis().SetTitle(getVarDef(var, 'ytit2'))
        hdummy.GetYaxis().SetTitleSize(0.07)
        hdummy.GetXaxis().SetLabelSize(0)
        
        dummyColl.append(hdummy)
        
        gr1.SetMarkerSize(0.8)
        gr1.SetMarkerStyle(22)
        gr1.SetMarkerColor(4)
        gr1.SetLineColor(4)
        gr1.SetLineWidth(1)
        gr2.SetMarkerSize(0.8)
        gr2.SetMarkerStyle(21)
        gr2.SetMarkerColor(2)
        gr2.SetLineColor(2)
        gr2.SetLineWidth(1)
        grdiff.SetMarkerSize(0.8)
        grdiff.SetLineWidth(1)
        
        if options.batch :
            gr1.SetMarkerSize(0.3)
            gr2.SetMarkerSize(0.3)
            grdiff.SetMarkerSize(0.3)
        gr1.GetYaxis().SetTitleOffset(1.7)
        grdiff.GetXaxis().SetTitle(getVarDef(var, 'xtit'))
        grdiff.GetYaxis().SetTitle(getVarDef(var, 'ytit'))
        grdiff.GetXaxis().SetTitleSize(0.07)
        grdiff.GetYaxis().SetTitleSize(0.07)
        grdiff.GetXaxis().SetLimits(xmin, xmax)
        
        if doOnePlot:
            canvas.cd(1)
            ROOT.gPad.SetTopMargin(0.1)
            ROOT.gPad.SetLeftMargin(0.2)
            ROOT.gPad.SetRightMargin(0.1)
            hdummy.Draw()
            gr1.Draw("P")
            gr2.Draw("P")
            
            canvas.cd(2)
            ROOT.gPad.SetTopMargin(0.05)
            ROOT.gPad.SetBottomMargin(0.2)
            ROOT.gPad.SetLeftMargin(0.2)
            ROOT.gPad.SetRightMargin(0.1)
            grdiff.Draw("AP")
            
        elif options.multicanv:
            ROOT.gStyle.SetGridColor(ROOT.kGray)
            ROOT.gStyle.SetGridWidth(1)
            canvases[var]=ROOT.TCanvas('BeamSpotComparison_%s'%var,
                                       'BeamSpotComparison_%s'%var,
                                       800,600)
            canvases[var].cd()
            
            primarypad[var] = ROOT.TPad("primarypad","primarypad",0,0.35,1,1)
            primarypad[var].SetBottomMargin(0.03)
            primarypad[var].Draw()
            primarytextscale=1./(primarypad[var].GetWh()*primarypad[var].GetAbsHNDC());
            
            ratiopad[var] = ROOT.TPad("ratiopad","ratiopad",0,0.03,1,0.35)
            ratiopad[var].SetTopMargin(0.03)
            ratiopad[var].SetBottomMargin(0.3)
            ratiopad[var].SetGridy(1)
            ratiopad[var].Draw()
            ratiotextscale=1./(ratiopad[var].GetWh()*ratiopad[var].GetAbsHNDC())

            primarypad[var].cd()
            hdummy.Draw()
            gr1.Draw("P")
            gr2.Draw("P")

            hdummy.GetYaxis().SetTitleSize(0.085)
            hdummy.GetYaxis().SetTitleOffset(0.75)
            hdummy.GetYaxis().SetLabelSize(0.06)
            hdummy.GetXaxis().SetLabelSize(0)

            minyaxis=hdummy.GetYaxis().GetXmin()
            maxyaxis=hdummy.GetYaxis().GetXmax()
            rangeyaxis=abs(minyaxis-maxyaxis)
            hdummy.GetYaxis().SetLimits(minyaxis,maxyaxis+0.50*rangeyaxis)
            primarypad[var].Update()
            
            ratiopad[var].cd()
            grdiff.Draw("AP")

            diffymax=grdiff.GetYaxis().GetXmax()
            diffymin=grdiff.GetYaxis().GetXmin()
            if diffymin>0.:
                grdiff.GetYaxis().SetRangeUser(0.-0.1*abs(diffymax-diffymin),diffymax)
            elif diffymax<0.:
                grdiff.GetYaxis().SetRangeUser(diffymin,0.+0.1*abs(diffymax-diffymin))
            
            grdiff.GetYaxis().SetTitleSize(0.16)
            grdiff.GetYaxis().SetTitleOffset(.39)
            grdiff.GetXaxis().SetTitleOffset(0.9)
            grdiff.GetXaxis().SetTitleSize(0.16)
            grdiff.GetYaxis().SetLabelSize(0.12)
            grdiff.GetYaxis().SetNdivisions(8)
            grdiff.GetXaxis().SetLabelSize(0.12)

            zeroline[var]=ROOT.TLine(xmin, 0.0, xmax+1, 0.0)
            zeroline[var].SetLineWidth(2)
            zeroline[var].Draw()

            grdiff.Draw("P")
            
            ratiopad[var].Update()
            
            canvases[var].Update()
        else:
            canvas.cd(getVarDef(var, 'cannr'))
            ROOT.gPad.SetTopMargin(0.1)
            ROOT.gPad.SetLeftMargin(0.2)
            ROOT.gPad.SetRightMargin(0.1)
            ROOT.gPad.SetBottomMargin(0.02)
            hdummy.Draw()
            gr1.Draw("P")
            gr2.Draw("P")
            
            canvas.cd(getVarDef(var, 'cannr')+3)
            ROOT.gPad.SetTopMargin(0.05)
            ROOT.gPad.SetBottomMargin(0.2)
            ROOT.gPad.SetLeftMargin(0.2)
            ROOT.gPad.SetRightMargin(0.1)
            grdiff.Draw("AP")
            
    ivar += 1
    
# summary frame
def drawSummary(var=''):
    # description
    descrText=''
    if options.config == 'OnlineOffline':
        descrText += 'Online - Offline comparison'
        legText1 = 'Offine'
        legText2 = 'Online'
    elif options.config == 'Reproc':
        if not options.multicanv:
            descrText += 'Reprocessed - Tier0 comparison'
        if options.label1 == '':
            legText1 = 'Tier0'
        if options.label2 == '':
            legText2 = 'Reproc'
    else:
        legText1 = 'Beamspots 1'
        legText2 = 'Beamspots 2'
         
    if options.label1 != '':
        legText1 = options.label1

    if options.label2 != '':
        legText2 = options.label2


    if options.multicanv:
        primarypad[var].cd()
    if not options.multicanv:
        ROOTUtils.drawText(0.14, 0.95, 0.1, descrText)
    else:
        ROOTUtils.drawText(0.14, 0.95, 0.1, descrText)
    
    # legend
    legendList = []
    
    legendList.append([gr1, legText1, 'LP'])
    legendList.append([gr2, legText2, 'LP'])
    legendList.append([grdiff, 'Difference', 'LP'])
    if options.multicanv:
        ROOTUtils.drawLegend(0.25, 0.6, 0.60, 0.92, legendList)
    elif not options.plotHistos:
        ROOTUtils.drawLegend(0.14, 0.67, 0.9, 0.9, legendList)
    if runNumber != 0:
        if options.multicanv:
            ROOTUtils.drawText(0.6, 0.7, 0.1, 'Run = %i' %runNumber)
        else:
            ROOTUtils.drawText(0.14, 0.55, 0.1, 'Run = %i' %runNumber)
    if options.multicanv:
        primarypad[var].Update()
        
if not doOnePlot:
    if options.plotHistos:
        canvas.cd(1)
        drawSummary()
    elif options.multicanv:
        for var,canv in canvases.iteritems():
            canv.cd()
            drawSummary(var)
    else:
        canvas.cd(4)
        drawSummary()
        
    
    if not options.multicanv:
        newText = 'New fits: %i' % numNew
        oldText = 'Missing fits: %i' %numOld
        ROOTUtils.drawText(0.14, 0.19, 0.1, newText)
        ROOTUtils.drawText(0.14, 0.07, 0.1, oldText)
    
if options.output:
    basename=options.outtag
    if options.config == "Reproc":
        basename=tag2.replace(".BeamSpotNt-nt.root",".PlotBeamSpotCompareReproc").replace(".MergeNt-nt.root",".PlotBeamSpotCompareReproc")
        basename=basename[basename.rfind('/')+1:]
        if options.html and options.outtag.find('/'):
            basename=options.outtag[:options.outtag.rfind('/')+1]+basename+options.htmltag+"."
    elif options.runNumber>0:
        basename+=str(options.runNumber)
        basename+="_"
    elif options.runMin>0:
        basename+=str(options.runMin)
        basename+="_"
        basename+=str(options.runMax)
        basename+="_"
        
    for o in options.output.split(','):
        if options.multicanv:
            if options.html:
                htmlstart=0
                if basename.find('/')>0:
                    htmlstart=basename.rfind('/')+1
                htmlfilename=basename[htmlstart:-1]+".html"
                html=open(htmlfilename,'w')
                print "writing html file %s" % htmlfilename
            for var,canv in canvases.iteritems():
                canv.Print(basename+var+o)
            if o==".png" and options.html:
                for var in ['posX', 'posY', 'posZ', 'sigmaX', 'sigmaY', 'sigmaZ', 'rhoXY', 'tiltX', 'tiltY', 'k', 'nEvents']:
                    if var in canvases:
                        if "pdf" in options.output:
                            html.write("<a href=\""+basename+var+".pdf\"><img src=\""+basename+var+o+"\" width=\"33%\"></a>")
                        else:
                            html.write("<img src=\""+basename+var+o+"\" width=\"33%\">")
            if options.html:
                html.close()
        else:
            canvas.SaveAs(o)
            

if not options.batch:
    while(True):
        time.sleep(1)
                    
