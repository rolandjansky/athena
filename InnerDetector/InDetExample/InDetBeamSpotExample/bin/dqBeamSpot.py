#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
CAlculate beamspot DQ defects automatically from result of beamspot fit and
produce locally sqlite file that can later be merged into HEAD.  Also dump
results to txt file for linking to web page

"""
__author__  = 'Carl Gwilliam'
__version__ = '$Id $'
__usage__   = '%prog [options] nt.root [nt.root ...]'

import sys,os
from math import *
from InDetBeamSpotExample.BeamSpotData import *

# Argument parsing
from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-b', '--batch', dest='batch', action='store_true', default=False, help='run in batch mode')
parser.add_option('-i', '--interactive', dest='interactive', action='store_true', default=False, help='interactive')
parser.add_option('-e', '--extend', dest='extend', action='store_true', default=False, help='extend existing SQLite file (default: delete any existing file)')
parser.add_option('', '--noaverage', dest='noaverage', action='store_true', default=False, help='do not write per-run average beam spot with large width')
parser.add_option('', '--statlist', dest='statlist', default='59', help='comma separated list of status word values to accept (must have known fitId)')
parser.add_option('', '--lbmin', dest='lbmin', type='int', default=None, help='Minimum LB to consider')
parser.add_option('', '--lbmax', dest='lbmax', type='int', default=None, help='Maximum LB to consider')
parser.add_option('-o', '--output', dest='output', default='dqflags.db', help='name of output COOL SQLite file for DQ (default: dqflags.db')
parser.add_option('-t', '--tag', dest='tag', default='nominal', help='COOL tag (default: nominal)')
parser.add_option('-a', '--absent', dest='absent', action='store_true', default=False, help='Write absent (as well as present) defects to the DB (default: False)')

(options,args) = parser.parse_args()
if len(args) < 1:
    parser.error('wrong number of command line arguments')


# Setup ROOT
if options.batch:
    os.unsetenv('DISPLAY')
import ROOT
from InDetBeamSpotExample import ROOTUtils
#ROOTUtils.setStyle()

from InDetBeamSpotExample.Utils import getRunFromName

# Prepare list of algorithms to consider
statList = []
fitIdList = []
for a in options.statlist.split(','):
    statList.append(int(a))
    try:
        fitIDInt = int( format( int(a), '08b')[:4], 2)
        fitIdList.append( fitIDInt )
    except:
        print 'ERROR: Status word value of %i has no known fitId entry' % int(a)
        sys.exit(1)

# First loop over all input files to determine if an everage is available
# (i.e. nEntires with correct fit status > 0)

# Set initial min and max run numbers from file names
#try:
runs = [int(getRunFromName(f)) for f in args]
minRun = min(runs)
maxRun = max(runs)
#except:
#    minRun = 1E10
#    maxRun = -1
nEntries = 0

print '\nDetermining if average beamspot parameters available'
for filename in args:
    print '\nExtracting from file ',filename,':'

    f = ROOT.TFile(filename)
    if f.Get('BeamSpotNt'):
        bsNt = BeamSpotNt(filename)
    elif f.Get('Beamspot/Beamspots'):
        bsNt = BeamSpotFinderNt(filename)

    for bs in bsNt.allData():

        # Skip fits outside of desired LB range
        if options.lbmin and options.lbmin>bs.lbStart:
            continue
        if options.lbmax and options.lbmax<bs.lbEnd+1:
            continue
        if not bs.status in statList:
            continue
        #Convert status word to ints for fitStatus and fitID
        fitStatusInt = int( format( bs.status, '08b')[-2:], 2)
        fitIDInt = int( format( bs.status, '08b')[:4], 2)

        print '[%i, %3i - %3i]:  %5i vertices, fitStatus = %i, fitID = %i' % (bs.run,bs.lbStart,bs.lbEnd+1,bs.nEvents,fitStatusInt,fitIDInt)

        minRun = min(bs.run,minRun)
        maxRun = max(bs.run,maxRun)

        # Skip undesired fits
        if fitStatusInt!=3:
            print '*** Skipping unsuccessful fit ...\n'
            continue
        if not fitIDInt in fitIdList:
            print '*** Skipping fit with ID =',fitIDInt,'\n'
            continue

        nEntries += 1
        print

    f.Close()

# Set DQ defect to 'ID_BS_NOBEAMSPOT' if there is no average beamspot or 'ID_BS_RUNAVERAGE' if
# there is (i.e. at least one succesfull fit and not options.noaverage)

from InDetBeamSpotExample.DQUtilities import IDBSDefectWriter
idbsDefects = IDBSDefectWriter(options.output, not options.extend)    

if nEntries>0 and not options.noaverage:
    print '\nInitially Setting DQ defect to ID_BS_RUNAVERAGE for runs %s ... %s with average beamspot available' %(minRun, maxRun)
    idbsDefects.defectType('ID_BS_RUNAVERAGE')
else:
    print '\nInitially setting DQ defect to ID_BS_NOBEAMSPOT for runs %s ... %s without average beamspot available' % (minRun, maxRun)
    idbsDefects.defectType('ID_BS_NOBEAMSPOT')
    
# for protection against multiple rows with same LB range
good_lbs={}
if minRun==maxRun:
    good_lbs[minRun]={}
else:
    for i in range(minRun,maxRun):
        good_lbs[i]={}
# Loop over ntuple and set LBs for which beamspot is correctly determined (i.e. no defects)
for filename in args:
    print 'Copying beam spot data from',filename
    f = ROOT.TFile(filename)
    if f.Get('BeamSpotNt'):
        bsNt = BeamSpotNt(filename)
    elif f.Get('Beamspot/Beamspots'):
        bsNt = BeamSpotFinderNt(filename)

    for bs in bsNt.allData():
        # Skip entries outside of desired LB range
        if options.lbmin and options.lbmin>bs.lbStart:
            continue
        if options.lbmax and options.lbmax<bs.lbEnd+1:
            continue

        if bs.status in statList and bs.lbStart not in good_lbs[bs.run]:
            #  Fill GOOD Lbs where defect is NOT present (lbEnd for DQ is exclusive)
            idbsDefects.add(minRun, maxRun, bs.lbStart, bs.lbEnd+1)
            good_lbs[bs.run][bs.lbStart]=1
            
        else:
            print 'WARNING: Skipping entry with status word %3s, lbStart=%4d, lbEnd=%4d' % (bs.status, bs.lbStart, bs.lbEnd)

    f.Close()

# Set defects for those LBs where beamspot is not correctly determined by inverting good LB list
idbsDefects.complete(minRun, maxRun)
idbsDefects.dump()

# Write to sqlite db and text file
if options.output:
    idbsDefects.writeDefects(nonpresent = options.absent)
    idbsDefects.dump(options.output)

if options.lbmin or options.lbmax:
    print '\n**** WARNING: ONLY CONSIDERED ENTRIES IN RANGE [%s,%s] ****\n' % (options.lbmin,options.lbmax)
    
# Enter interactive mode if desired
if options.interactive:
    os.environ['PYTHONINSPECT'] = '1'
