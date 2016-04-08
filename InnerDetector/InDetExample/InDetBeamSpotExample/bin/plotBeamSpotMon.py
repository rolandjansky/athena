#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Plot histograms from beam spot monitoring file.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id $'
__usage__   = '%prog [options] beamspotmonitoring.root'

import os
#from math import *
import re

from InDetBeamSpotExample.Utils import getRunFromName


# Histogram definition
hDef = {
    'trkDPhi': {'opts': 'COLZ', 'stats': 'n'},
    'trkDPhiCorr': {'opts': 'COLZ', 'stats': 'n'},
    'pvYX': {'opts': 'COLZ', 'stats': 111},
    'pvXZ': {'opts': 'COLZ', 'stats': 111},
    'pvYZ': {'opts': 'COLZ', 'stats': 111},
    'pvX': {'stats': 111111},
    'pvY': {'stats': 111111},
    'pvZ': {'stats': 111111},
    'pvChiSqDof': {'stats': 101111},
    'pvNTracks': {'stats': 101111},
    'pvTrackPt': {'stats': 101111},
}


# Argument parsing
from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-i', '--interactive', dest='interactive', action='store_true', default=False, help='interactive')
parser.add_option('-b', '--batch', dest='batch', action='store_true', default=False, help='run in batch mode')
parser.add_option('-o', '--output', dest='output', default='.gif', help='comma-separated list of output files or formats (default: .gif')
(options,args) = parser.parse_args()
if len(args) != 1:
    parser.error('wrong number of command line arguments')
filename = args[0]
run = getRunFromName(filename)
if run:
    basename = run+'-'
else:
    run = 'UNKNOWN'
    basename = ''


# Setup ROOT
if options.batch:
    os.unsetenv('DISPLAY')
import ROOT
from InDetBeamSpotExample import ROOTUtils
ROOTUtils.setStyle()
ROOT.gStyle.SetPalette(1)
c = ROOT.TCanvas('BeamSpotMonitoring','Beam Spot Monitoring',750,1000)
c.Divide(3,4)


# Open monitoring file
mon = ROOT.TFile(filename)
mondir = 'InDetGlobal/BeamSpot/'


# Summary frame
c.cd(1)
ROOTUtils.drawText(0.14,0.9,0.06,'Beam spot monitoring for run %s:' % run)
yline = 0.83
for f in args:
    b = f.split('/')[-1]
    dsname = b.split('-')[0]
    taskname = '-'.join(b.split('-')[1:-1])
    ROOTUtils.drawText(0.14,yline,0.05,'- %s,;   task %s' % (dsname,taskname),font=42)
    yline -= 0.11

nc = 2
for hname in ['trkDPhi', 'trkDPhiCorr',
              'pvYX', 'pvXZ', 'pvYZ',
              'pvX', 'pvY', 'pvZ',
              'pvChiSqDof', 'pvNTracks', 'pvTrackPt'
              ]:
    c.cd(nc)
    print 'Processing histogram',hname
    ROOT.gStyle.SetOptStat(hDef[hname].get('stats',0))
    h = mon.Get(mondir+hname)
    if h != None:
        h.Draw(hDef[hname].get('opts',''))
    else:
        print "... no histogram with name %s" % hname
    c.Modified()
    c.Update()
    nc += 1

# Save and finalize
if options.output:
    for o in options.output.split(','):
        if o[0]=='.':
            c.SaveAs(basename+'beamspotmon'+o)
        else:
            c.SaveAs(o)

print
if options.interactive:
    os.environ['PYTHONINSPECT'] = '1'
