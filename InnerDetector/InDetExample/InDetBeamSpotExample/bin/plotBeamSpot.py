#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Plot beam spot parameters from beam spot fit ntuple.
Using the time axis option (--timeaxis) requires a connection to Oracle.
You may need to set e.g. FRONTIER_SERVER to "(serverurl=http://squid-frontier.usatlas.bnl.gov:23128/frontieratbnl)".
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: plotBeamSpot.py 613578 2014-08-26 20:53:31Z btamadio $'
__usage__   = '%prog [-p var] [options] nt.root [nt.root ...]'

import sys, os
import re
import time
from math import *
from array import array

from InDetBeamSpotExample.Utils import getRunFromName
from InDetBeamSpotExample.LHCInfoUtils import lhcFillData
from InDetBeamSpotExample.LHCInfoUtils import lhcEnergyData
from InDetBeamSpotExample.BeamSpotData import *

ntDef = {
    'nev': {'var': 'nEvents', 'atit': ';Luminosity Block Number;Number of events', 'title': 'Number of events in Fit'},
    'x': {'var': 'posX', 'cov': 'posXErr', 'atit': ';Luminosity Block Number;x (mm)', 'title': 'Beam Spot x'},
    'y': {'var': 'posY', 'cov': 'posYErr', 'atit': ';Luminosity Block Number;y (mm)', 'title': 'Beam Spot y'},
    'z': {'var': 'posZ', 'cov': 'posZErr', 'atit': ';Luminosity Block Number;z (mm)', 'title': 'Beam Spot z'},
    'sigx': {'var': 'sigmaX', 'cov': 'sigmaXErr', 'atit': ';Luminosity Block Number;#sigma_{x} (mm)', 'title': 'Beam Spot #sigma_{x}'},
    'sigy': {'var': 'sigmaY', 'cov': 'sigmaYErr', 'atit': ';Luminosity Block Number;#sigma_{y} (mm)', 'title': 'Beam Spot #sigma_{y}'},
    'sigz': {'var': 'sigmaZ', 'cov': 'sigmaZErr', 'atit': ';Luminosity Block Number;#sigma_{z} (mm)', 'title': 'Beam Spot #sigma_{z}'},
    'tiltx': {'var': 'tiltX', 'cov': 'tiltXErr', 'atit': ';Luminosity Block Number;Tilt in x-z (rad)', 'title': 'Beam Spot Tilt in x-z'},
    'tilty': {'var': 'tiltY', 'cov': 'tiltYErr', 'atit': ';Luminosity Block Number;Tilt in y-z (rad)', 'title': 'Beam Spot Tilt in y-z'},
    'rho': {'var': 'rhoXY', 'cov': 'rhoXYErr', 'atit': ';Luminosity Block Number;#rho', 'title': 'Beam Spot #rho_{xy}'},
    'k': {'var': 'k', 'cov': 'kErr', 'atit': ';Luminosity Block Number;Error scale factor k', 'title': 'Error Scale Factor k'},
    'pv': {'var': 'lb', 'nt': 'Vertices', 'drawOnly': True, 'atit': ';Luminosity Block Number;# Primary Vertices / LB'},
    'scanx': {'var': 'posX', 'cov': 'posXErr', 'atit': ';Pseudo-Luminosity Block Number;x (mm)', 'title': 'Beam Spot x'},
    'scany': {'var': 'posY', 'cov': 'posYErr', 'atit': ';Pseudo-Luminosity Block Number;y (mm)', 'title': 'Beam Spot y'},
    'scansigx': {'var': 'sigmaX', 'cov': 'sigmaXErr', 'atit': ';Pseudo-Luminosity Block Number;#sigma_{x} (mm)', 'title': 'Beam Spot #sigma_{x}'},
    'scansigy': {'var': 'sigmaY', 'cov': 'sigmaYErr', 'atit': ';Pseudo-Luminosity Block Number;#sigma_{y} (mm)', 'title': 'Beam Spot #sigma_{y}'},
    }

def getNtDef(what,property,default=''):
    try:
        return ntDef[what][property]
    except:
        return default
__usage__ += '\n\nPossible variables to plot are:\n'
__usage__ += ' '.join(sorted(ntDef.keys()))


# Argument parsing
from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-p', '--plotvar', dest='plotvar', default='', help='variable to plot')
parser.add_option('', '--compare', dest='compare', action='store_true', default=False, help='compare/overaly data from several files')
parser.add_option('-i', '--interactive', dest='interactive', action='store_true', default=False, help='interactive')
parser.add_option('-b', '--batch', dest='batch', action='store_true', default=False, help='run in batch mode')
parser.add_option('-d', '--debug', dest='debug', action='store_true', default=False, help='debug')
parser.add_option('', '--lbbugfix', dest='lbbugfix', action='store_true', default=False, help='bug fix for LB offset of 1 in December beam spot ntuples')
parser.add_option('', '--lbmin', dest='lbmin', type='int', default=None, help='Minimum LB to consider')
parser.add_option('', '--lbmax', dest='lbmax', type='int', default=None, help='Maximum LB to consider')
parser.add_option('-y', '--yscale', dest='yscale', type='float', default=1.0, help='y axis scale factor')
parser.add_option('', '--xmin', dest='xmin', default=None, help='x axis minimum (LB number or in UTC in form "Apr 12 13:23:08 2010"')
parser.add_option('', '--xmax', dest='xmax', default=None, help='x axis maximum (LB number or in UTC in form "Apr 12 13:23:08 2010"')
parser.add_option('', '--ymin', dest='ymin', type='float', default=None, help='y axis minimum')
parser.add_option('', '--ymax', dest='ymax', type='float', default=None, help='y axis maximum')
parser.add_option('-a', '--average', dest='average', default=None, help='average')
parser.add_option('-e', '--error', dest='error', default='0', help='error')
parser.add_option('', '--energy', dest='energy', action='store_true', default=False, help='add energy label')
parser.add_option('-u', '--unit', dest='unit', default='mm', help='unit (default: mm)')
parser.add_option('-t', '--title', dest='title', default='', help='bold text e.g. for run info line (default: what plus run# from 1st file name)')
parser.add_option('-c', '--comment', dest='comment', default='', help='additional text (use semicolon to indicate line breaks)')
parser.add_option('', '--alegend', dest='alegend', default='Result from fit to all data:', help='legend text for average')
parser.add_option('', '--plegend', dest='plegend', default='Fit to groups of LBs', help='legend text for points')
parser.add_option('-m', '--multiruns', dest='multiruns', action='store_true', default=False, help='multiple runs - use run number as x axis label')
parser.add_option('', '--timeaxis', dest='timeaxis', action='store_true', default=False, help='use time on x axis instead of run or LB numbers')
parser.add_option('-l', '--localtime', dest='localtime', action='store_true', default=False, help='use CERN time zon instead of UTC when --timeaxis is given')
parser.add_option('-o', '--output', dest='output', default='.gif', help='comma-separated list of output files or formats (default: .gif)')
parser.add_option('', '--tgraphfile', dest='tgraphfile', default='', help='optional root file to save TGraph into')
parser.add_option('-w', '--wide', dest='wide', action='store_true', default=False, help='make wide plots')
parser.add_option('', '--extrawide', dest='extrawide', action='store_true', default=False, help='make extra wide plots')
parser.add_option('-s', '--square', dest='square', action='store_true', default=False, help='make square plots')
parser.add_option('', '--ndivs', dest='ndivs', type='int', default=None, help='Number of x axis divisions (ROOT format)')
parser.add_option('-f', '--fitgraph', dest='fitgraph', action='store_true', default=False, help='fit points')
parser.add_option('', '--postplot', dest='postplot', default='', help='call routine from PlotLibrary for additional plotting (single plots only)')
parser.add_option('', '--approved', dest='approved', action='store_true', default=False, help='use labelling for approved plots')
parser.add_option('', '--prelim', dest='prelim', action='store_true', default=False, help='add ATLAS Preliminary to figure')
parser.add_option('', '--published', dest='published', action='store_true', default=False, help='add ATLAS to figure')
parser.add_option('', '--atlasx', dest='atlasx', type='float', default=None, help='x position for drawing ATLAS (Preliminary) label')
parser.add_option('', '--atlasy', dest='atlasy', type='float', default=None, help='y position for drawing ATLAS (Preliminary) label')
parser.add_option('', '--atlasdx', dest='atlasdx', type='float', default=None, help='x position offset for drawing Preliminary label')


(options,args) = parser.parse_args()
if len(args) < 1:
    parser.error('wrong number of command line arguments')
if options.multiruns and options.timeaxis:
    print 'ERROR: --multiruns and --timeaxis are mutually exclusive options'
    sys.exit(1)


# Graphics defaults
if not options.atlasx:
    options.atlasx = 0.72 if options.wide else 0.645
if not options.atlasy:
    options.atlasy = 0.2
if not options.atlasdx:
    options.atlasdx = 0.08 if options.wide else 0.115



if options.approved:
    ntDef = {
        'nev': {'var': 'nEvents', 'atit': ';Luminosity block Number;Number of events', 'title': '# events used in fit'},
        'x': {'var': 'posX', 'cov': 'posXErr', 'atit': ';Luminosity Block Number;Luminous Centroid x (mm)', 'title': 'Beam Spot Position'},
        'y': {'var': 'posY', 'cov': 'posYErr', 'atit': ';Luminosity Block Number;Luminous Centroid y (mm)', 'title': 'Beam Spot Position'},
        'z': {'var': 'posZ', 'cov': 'posZErr', 'atit': ';Luminosity Block Number;Luminous Centroid z (mm)', 'title': 'Beam Spot Position'},
        'sigx': {'var': 'sigmaX', 'cov': 'sigmaXErr', 'atit': ';Luminosity Block Number;Luminous Size #sigma_{x} (mm)', 'title': 'Beam Spot Size'},
        'sigy': {'var': 'sigmaY', 'cov': 'sigmaYErr', 'atit': ';Luminosity Block Number;Luminous Size #sigma_{y} (mm)', 'title': 'Beam Spot Size'},
        'sigz': {'var': 'sigmaZ', 'cov': 'sigmaZErr', 'atit': ';Luminosity Block Number;Luminous Size #sigma_{z} (mm)', 'title': 'Beam Spot Size'},
        'tiltx': {'var': 'tiltX', 'cov': 'tiltXErr', 'atit': ';Luminosity Block Number;Luminous Region Tilt in x-z (rad)', 'title': 'Beam Spot Tilt'},
        'tilty': {'var': 'tiltY', 'cov': 'tiltYErr', 'atit': ';Luminosity Block Number;Luminous Region Tilt in y-z (rad)', 'title': 'Beam Spot Tilt'},
        'rho': {'var': 'rhoXY', 'cov': 'rhoXYErr', 'atit': ';Luminosity Block Number;#rho', 'title': 'Beam Spot #rho_{xy}'},
        'k': {'var': 'k', 'cov': 'kErr', 'atit': ';Luminosity Block Number;Error scale factor k', 'title': 'Error Scale Factor k'},
        'pv': {'var': 'lb', 'nt': 'Vertices', 'drawOnly': True, 'atit': ';Luminosity Block Number;# Primary Vertices / LB'},
        }


# Make sure times are in UTC
if options.localtime:
    timeLabel = 'CERN Local Time'
    os.environ['TZ'] = 'CET'
else:
    timeLabel = 'Time (UTC)'
    os.environ['TZ'] = 'UTC'
time.tzset()

# Setup ROOT
if options.batch:
    os.unsetenv('DISPLAY')
import ROOT
from InDetBeamSpotExample import ROOTUtils
ROOTUtils.setStyle()


# Setup tool to query start and end times of runs and LBs
if options.timeaxis:
    from InDetBeamSpotExample.COOLUtils import COOLQuery
    runQuery = COOLQuery()


# Function to create a single plot
def plot(what,fileList):

    # Determine what to plot
    var = getNtDef(what,'var')
    cov = getNtDef(what,'cov')
    if not var:
        print 'ERROR: Unknown quantity to plot',what
        print 'Possible choices are:'
        print ' '.join(sorted(ntDef.keys()))
        print
        sys.exit(1)

    runlist = []

    if getNtDef(what,'drawOnly',False):
        # Simple draw
        #oldstat = ROOT.gStyle.GetOptStat()
        #ROOT.gStyle.SetOptStat(10)
        f = ROOT.TFile(fileList[0])
        if f.Get('Vertices'):
            ntName = 'Vertices'
        elif f.Get('Beamspot/Vertices'):
            ntName = 'Beamspot/Vertices'
        chain = ROOT.TChain(ntName)
        for filename in fileList:
            chain.Add(filename)
            runlist.append([0,getRunFromName(filename),filename])
        if chain.GetEntries()>0:
            var = getNtDef(what,'var')
            xmin = options.xmin if options.xmin else -1e10
            xmax = options.xmax if options.xmax else 1e10
            chain.Draw(var,'%s >= %s && %s <= %s' % (var,xmin,var,xmax) )
            h = ROOT.gPad.GetPrimitive('htemp')
            h.SetTitle(getNtDef(what,'atit'))
            ROOT.gPad.Update()
            #ROOT.gStyle.SetOptStat(oldstat)
        else:
            print 'ERROR: No entries found in %s (variable %s)' % (ntName,var)
            h = ROOTUtils.protect( ROOT.TH2F('dummy','No Entries Found'+getNtDef(what,'atit'),1,0,1,1,0,1) )
            h.Draw()
        frame = None
        gr = None

    else:
        # Graph with error bars
        # Accumulate data from all input files
        x = array('d')
        y = array('d')
        ex = array('d')
        ey = array('d')
        ymin = 1E10
        ymax = -1E10
        xmin = 1E10
        xmax = -1E10
        xoffset = 0
        xstart=0
        xwidth=0
        
        for filename in fileList:
            print 'Processing',filename
            run = getRunFromName(filename)   # TODO: what about merged nt files from multiple runs?
            if options.timeaxis:
                lbDict = runQuery.getLbTimes(int(run))
                lastLb = max(lbDict.keys())
            runlist.append([xoffset,run,filename])
            f = ROOT.TFile(filename)
            if f.Get('BeamSpotNt'):
                bsNt = BeamSpotNt(filename)
            elif f.Get('Beamspot/Beamspots'):
                bsNt = BeamSpotFinderNt(filename)

            for bs in bsNt.allData():
                xstart = bs.lbStart + xoffset
                if options.lbmin!=None and xstart<options.lbmin:
                    continue
                if options.lbmax!=None and xstart>options.lbmax:
                    continue
                xwidth = bs.lbEnd - bs.lbStart + 1
                if xwidth<1:
                    xstart = xoffset
                    xwidth = 1.

                if options.timeaxis:
                    xwidth = bs.lbEnd - bs.lbStart + 1
                    if xwidth<1:
                        xstart = runQuery.getRunStartTime(int(run))
                        xend = runQuery.getRunEndTime(int(run))
                    else:
                        if options.lbbugfix:
                            xstart = lbDict[bs.lbStart+1][0]   # Fix for LB offset bug in ntuples created before InDetBeamSpotFinder-01-00-28
                        else:
                            xstart = lbDict[bs.lbStart][0]
                        try:
                            if options.lbbugfix:
                                xend = lbDict[bs.lbStart+xwidth][1]
                            else:
                                xend = lbDict[bs.lbStart+xwidth-1][1]
                        except:
                            xend = lbDict[lastLb][1]
                    xwidth = xend-xstart

                yval = getattr(bs,var)
                yerr = getattr(bs,cov) if cov else 0
                #Gets a fitStatus integer from bs.status, by converting to binary and slicing
                fitStatusInt = int( format( bs.status, '08b')[-2:], 2)
                if fitStatusInt == 3:
                    # Use only data from successful full fits
                    xmin = min(xstart,xmin)
                    xmax = max(xstart+xwidth,xmax)
                    x.append(xstart+xwidth/2.)
                    ex.append(xwidth/2.)
                    y.append(yval)
                    ey.append(yerr)
                    ymax = max(yval+yerr,ymax)
                    ymin = min(yval-yerr,ymin)
                if options.debug:
                    if fitStatusInt == 3:
                        print '[%3i,%3i):   %f +- %f' % (xstart,xstart+xwidth,yval,yerr)
                    else:
                        print '[%3i,%3i):   NO FIT RESULT' % (xstart,xstart+xwidth)

            f.Close()
            xoffset = xstart+xwidth
        runlist.append([xoffset,''])

        # Check if there's anything to draw
        if len(x)<=0:
            print 'ERROR: No successful beam spot determination found; no entries for %s' % var
            h = ROOTUtils.protect( ROOT.TH2F('dummy','No Entries Found'+getNtDef(what,'atit'),1,0,1,1,0,1) )
            h.Draw()
            return (runlist,h,None)

        # Determine axis range
        # The following prevents stripping LBs w/o data in a run from the x axis
        if options.multiruns:
            xmin = 0
        xminData = xmin
        xmaxData = xmax
        h = (xmax-xmin)
        xmax += .05*h
        xmin -= .05*h
        h = (ymax-ymin)/2*options.yscale
        ymax += 1.3*h
        ymin -= 0.5*h
        if options.xmin!=None:
            xmin = time.mktime(time.strptime(options.xmin,'%b %d %H:%M:%S %Y')) if options.timeaxis else float(options.xmin)
        if options.xmax!=None:
            xmax = time.mktime(time.strptime(options.xmax,'%b %d %H:%M:%S %Y')) if options.timeaxis else float(options.xmax)
        if options.ymin!=None:
            ymin = options.ymin
        if options.ymax!=None:
            ymax = options.ymax

        # Make plot
        frame = ROOTUtils.drawAxisFrame(xmin,xmax,ymin,ymax,getNtDef(what,'atit'))
        if options.average:
            dx = (xmax-xmin)/30
            b = ROOTUtils.drawHorizontalBand(xmin+dx,xmax-dx,float(options.average),float(options.error),centralLine=True)

        gr = ROOTUtils.protect( ROOT.TGraphErrors(len(x),x,y,ex,ey) )
        gr.SetName(what)
        gr.SetTitle(what)
        # Bug fix for symbol size in batch mode on lxplus
        if options.batch:
            gr.SetMarkerSize(0.3)
            #gr.SetMarkerStyle(8)
        else:
            gr.SetMarkerSize(0.8)
        gr.Draw('P')
        if options.tgraphfile:
            print 'Saving data as TGraphErrors with name',what,'to file',options.tgraphfile
            tgraphfile = ROOT.TFile(options.tgraphfile,'UPDATE')
            gr.Write()
            tgraphfile.Close()

        if options.fitgraph:
            gr.Fit('pol1','','SAME',xminData,xmaxData)
            # TODO: appropriate labelling

        legendList = []
        if options.average:
            legendList.append([b,options.alegend,'FL'])
            legendList.append([None,'%s %s #pm %s %s' % (options.average,options.unit,options.error,options.unit),''])
            #legendList.append([b,'Fit to full run','F'])
            #legendList.append([b,'Fit to LB 340 - 370','F'])
        if options.plegend:
            legendList.append([gr,options.plegend,'PLE'])
        #legendList.append([None,'blocks (2 * 116s)',''])

        # Overlay additional info if desired
        postPlotLegend = None
        if options.postplot:
            for p in options.postplot.split(','):
                print '\nExecuting %s from PlotLibrary ...' % p
                from InDetBeamSpotExample import PlotLibrary
                if hasattr(PlotLibrary,p):
                    postPlotLegends = getattr(PlotLibrary,p)(what)
                    if postPlotLegends:
                        for l in postPlotLegends:
                            legendList.append(l)
                else:
                    print 'ERROR: No method %s in PlotLibrary' % options.postplot
                print

        # Draw legend
        if legendList:
            if 'LHCEmittanceFill1022' in options.postplot:
                legend = ROOTUtils.drawLegend(0.55,0.73,0.87,0.91,legendList)
            else:
                legend = ROOTUtils.drawLegend(0.6,0.76,0.92,0.91,legendList)

        title = options.title
        if not title:
            title = getNtDef(what,'title')
        comment = options.comment
        if not comment:
            if options.approved:
                if len(runlist)>2:
                    comment += 'LHC Fills %s - %s;' % (lhcFillData.get(runlist[0][1],'') if lhcFillData.get(runlist[0][1],'') else '?',
                                                       lhcFillData.get(runlist[-2][1],'') if lhcFillData.get(runlist[-2][1],'') else '?')
                else:
                    comment += 'LHC Fill %s;' % lhcFillData.get(run,'') if lhcFillData.get(run,'') else ''
            else:
                lhcFillInfo = ' (LHC Fill %s)' % lhcFillData.get(run,'') if lhcFillData.get(run,'') else ''
                comment += 'Run '+runlist[0][1]+lhcFillInfo
                if len(runlist)>2:
                    comment += ' - '+runlist[-2][1]
                comment += ';'
            if options.energy:
                comment += '#sqrt{s} = %s;' % lhcEnergyData.get(run,'') if  lhcEnergyData.get(run,'') else ''
            if options.timeaxis:
                #startTime = time.gmtime(runQuery.getRunStartTime(int(runlist[0][1])))
                #endTime = time.gmtime(runQuery.getRunEndTime(int(runlist[-2][1])))
                startTime = time.gmtime(max(xminData,xmin))
                endTime = time.gmtime(min(xmaxData,xmax))
                #comment += ', starting;'+time.strftime('%c',startTime)
                comment += time.strftime('%a %b %d',startTime)
                if time.strftime('%a %b %d',startTime)!=time.strftime('%a %b %d',endTime):
                    comment += time.strftime(' - %a %b %d',endTime)
                comment += time.strftime(', %Y',endTime)
        if options.multiruns:
            xAxis = frame.GetXaxis()
            xAxis.SetTitle('Run Number')
            for i in range(len(runlist)-1):
                lx = (runlist[i][0]+runlist[i+1][0])/2   # label centered
                #lx = runlist[i][0]
                l = runlist[i][1]
                xAxis.SetBinLabel(xAxis.FindBin(lx),l)
                tickmark = ROOT.TLine(runlist[i][0],ymin,runlist[i][0],ymin+.1*h)
                tickmark.Draw()
                ROOTUtils.protect(tickmark)

            frame.LabelsOption('d','X')

        if options.timeaxis:
            xAxis = frame.GetXaxis()
            xAxis.SetTimeDisplay(1)
            if (xmax-xmin)/86400 < 1:
                xAxis.SetTimeFormat('%H:%M')
            else:
                #d0 = ceil(xmin/86400)  # day w/first tick mark
                #d1 = floor(xmax/86400) # day w/last tick mark
                #nTicks = d1-d0+1
                #nDays = floor((xmax-xmin)/86400)
                xAxis.SetTimeFormat('#splitline{%b %d}{%H:%M}')
                xAxis.SetLabelOffset(0.025)
                xAxis.SetTitleOffset(1.6)
            if options.ndivs:
                xAxis.SetNdivisions(options.ndivs)
            xAxis.SetTitle(timeLabel)
            frame.LabelsOption('d','X')   # Doesn't seem to work

        t1 = ROOTUtils.drawText(0.2,0.86,0.06,title)
        t2 = ROOTUtils.drawText(0.2,0.79,0.06,comment,font=42)

        #s1 = ROOTUtils.drawText(0.21,0.21,0.035,'900 GeV;Full Inner Detector', font=42)
        #s2 = ROOTUtils.drawText(0.39,0.21,0.033,'2.36 TeV;Pixels, SCT @ 20V', font=42)
        #s3 = ROOTUtils.drawText(0.57,0.21,0.033,'900 GeV;Pixels, SCT @ 20V', font=42)
        #s4 = ROOTUtils.drawText(0.75,0.21,0.033,'900 GeV;Full Inner Detector', font=42)

        if options.prelim:
            ROOTUtils.atlasLabel(options.atlasx,options.atlasy,True,offset=options.atlasdx)
        if options.published:
            ROOTUtils.atlasLabel(options.atlasx,options.atlasy,False,offset=options.atlasdx)

        ROOT.gPad.Update()

    return (runlist,frame,gr)


# What to plot
what = options.plotvar
if len(args)==1 or options.compare:
    run = getRunFromName(args[0])
    if run:
        basename = run+'-'
    else:
        basename = ''
else:
    basename = 'beamspothistory-'
if what:
    if options.wide:
        c = ROOT.TCanvas('BeamSpotPlot','Beam Spot Plot',1000,500)
    if options.extrawide:
        c = ROOT.TCanvas('BeamSpotPlot','Beam Spot Plot',1500,500)
    if options.square:
        c = ROOT.TCanvas('BeamSpotPlot','Beam Spot Plot',700,700)
    if options.compare:
        plegendList = options.plegend.split(';')
        options.plegend = ''
        plotList = [ ]
        for f in args:
            plotList.append(plot(what,[f]))

        plotList[0][1].Draw()
        title = options.title or getNtDef(what,'title')
        t1 = ROOTUtils.drawText(0.2,0.86,0.06,title)
        t2 = ROOTUtils.drawText(0.2,0.79,0.06,options.comment,font=42)

        colorSequence = [ 1,  2,  4,  6,  8, 1,  2,  4,  6,  8, 1,  2,   4,  6]
        styleSequence = [ 8, 21, 22, 23, 29, 4, 25, 26, 27, 28, 30,  2,  3,  6]
        idx = 0
        legendList = [ ]
        for p in plotList:
            gr = p[2]
            gr.SetMarkerSize(1.2)
            gr.SetMarkerColor(colorSequence[idx])
            gr.SetMarkerStyle(styleSequence[idx])
            gr.Draw('P')
            if idx>=len(plegendList):
                legendList.append([gr,args[idx],'PLE'])
            else:
                legendList.append([gr,plegendList[idx],'PLE'])
            idx += 1
            
        #legend = ROOTUtils.drawLegend(0.55,0.73,0.87,0.91,legendList)
        legend = ROOTUtils.drawLegend(0.7,0.7,0.92,0.91,legendList)

    else:
        (runlist,h,gr) = plot(what,args)
    if options.output:
        for o in options.output.split(','):
            if o[0]=='.':
                ROOT.gPad.SaveAs(basename+what+o)
            else:
                ROOT.gPad.SaveAs(o)

else:
    if options.compare:
        sys.exit('ERROR: Comparison only supported for individual plots - use -p option')
    options.average = None
    options.error = None
    c = ROOT.TCanvas('BeamSpotSummary','Beam Spot Summary',750,1000)
    c.Divide(3,4)

    # Summary frame
    c.cd(1)
    ROOTUtils.drawText(0.14,0.9,0.06,'Data from %i run(s):' % len(args))
    yline = 0.83
    for f in args:
        b = f.split('/')[-1]
        dsname = b.split('-')[0]
        taskname = '-'.join(b.split('-')[1:-1])
        ROOTUtils.drawText(0.14,yline,0.05,'- %s,;   task %s' % (dsname,taskname),font=42)
        yline -= 0.11

    nc = 2
    for w in ['pv','k','x','y','z','sigx','sigy','sigz','tiltx','tilty','rho']:
        c.cd(nc)
        nc += 1
        (runlist,h,gr) = plot(w,args)
    if options.output:
        for o in options.output.split(','):
            if o[0]=='.':
                c.SaveAs(basename+'beamspot'+o)
            else:
                c.SaveAs(o)


# New line, so we get to see all ROOT messages
print

if options.interactive:
    os.environ['PYTHONINSPECT'] = '1'
