#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Average beam spot parameters from beam spot fit ntuple and produce beam spot
COOL SQLite file with proper per-run average.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id$'
__usage__   = '%prog [options] nt.root [nt.root ...]'

import sys
from math import *
from numpy import *
from InDetBeamSpotExample.COOLUtils import *
from InDetBeamSpotExample.BeamSpotData import *
#from InDetBeamSpotExample.DQUtilities import *

# Alg number to fitID translation
fitId = {
    59: 3, 107:6
}

# List of quantities
vars = [ {'par': 'posX',    'ntvar': 'posX', 'ntcov': 'posXErr', 'fmt': '%10.4f', 'minVal': -2., 'maxVal': 2.},
         {'par': 'posY',    'ntvar': 'posY', 'ntcov': 'posYErr', 'fmt': '%10.4f', 'minVal': -2., 'maxVal': 2.},
         {'par': 'posZ',    'ntvar': 'posZ', 'ntcov': 'posZErr', 'fmt': '%10.2f', 'minVal': -40., 'maxVal': 40.},
         {'par': 'sigmaX',  'ntvar': 'sigmaX', 'ntcov': 'sigmaXErr', 'fmt': '%10.4f', 'minVal': 0.01, 'maxVal': 0.5},
         {'par': 'sigmaY',  'ntvar': 'sigmaY', 'ntcov': 'sigmaYErr', 'fmt': '%10.4f', 'minVal': 0.01, 'maxVal': 0.5},
         {'par': 'sigmaZ',  'ntvar': 'sigmaZ', 'ntcov': 'sigmaZErr', 'fmt': '%10.2f', 'minVal': 1., 'maxVal': 200.},
         {'par': 'tiltX',   'ntvar': 'tiltX', 'ntcov': 'tiltXErr', 'fmt': '%10.6f', 'minVal': -0.01, 'maxVal': 0.01},
         {'par': 'tiltY',   'ntvar': 'tiltY', 'ntcov': 'tiltYErr', 'fmt': '%10.6f', 'minVal': -0.01, 'maxVal': 0.01},
         {'par': 'rhoXY',   'ntvar': 'rhoXY', 'ntcov': 'rhoXYErr', 'fmt': '%10.3f', 'minErr': .005},
         {'par': 'k',       'ntvar': 'k',  'ntcov': 'kErr',   'fmt': '%10.3f', 'minVal': 0.5, 'maxVal': 1.3}
]
def idx(what):
    for i in range(len(vars)):
        if vars[i]['par']==what:
            return i
    return -1


# Argument parsing
from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-b', '--batch', dest='batch', action='store_true', default=False, help='run in batch mode')
parser.add_option('-i', '--interactive', dest='interactive', action='store_true', default=False, help='interactive')
parser.add_option('-e', '--extend', dest='extend', action='store_true', default=False, help='extend existing SQLite file (default: delete any existing file)')
parser.add_option('', '--globaldefault', dest='globaldefault', action='store_true', default=False, help='write global default beam spot of (0,0,0) with large width')
parser.add_option('', '--noaverage', dest='noaverage', action='store_true', default=False, help='do not write per-run average beam spot with large width')
parser.add_option('', '--simpleaverage', dest='simpleaverage', action='store_true', default=False, help='use simple instead of weighted average (for checks only)')
parser.add_option('', '--statlist', dest='statlist', default='59,107', help='comma separated list of status word values to accept (must have known fitId)')
parser.add_option('', '--lbmin', dest='lbmin', type='int', default=None, help='Minimum LB to consider')
parser.add_option('', '--lbmax', dest='lbmax', type='int', default=None, help='Maximum LB to consider')
parser.add_option('-o', '--output', dest='output', default='beamspot.db', help='name of output COOL SQLite file (default: beamspot.db')
parser.add_option('-d', '--dqoutput', dest='dqoutput', default='', help='name of output COOL SQLite file for DQ (default: "" -> no dq determination)')
parser.add_option('-t', '--tag', dest='tag', default='nominal', help='COOL tag (default: nominal)')
parser.add_option('', '--avealgcode', dest='avealgcode', default='83', help='Status code for average beam spot (default: 83 = 0b1010011)')

(options,args) = parser.parse_args()
if len(args) < 1:
    parser.error('wrong number of command line arguments')


# Setup ROOT
if options.batch:
    os.unsetenv('DISPLAY')
import ROOT
from InDetBeamSpotExample import ROOTUtils
#ROOTUtils.setStyle()


# Prepare list of algorithms to consider
statList = []
fitIdList = []
for a in options.statlist.split(','):
    statList.append(int(a))
    try:
        fitIdList.append(fitId[int(a)])
    except:
        print 'ERROR: Status word value of %i has no known fitId entry' % int(a)
        sys.exit(1)


# First loop over all input files: calculate average beam spot parameters
sumw = zeros(len(vars))
sumwx = zeros(len(vars))
minRun = 1E10
maxRun =-1
nEntries = 0
nWarnings = 0
totVtx = 0
totEvents = 0
minDQRun = 1E10
maxDQRun =-1


for filename in args:
    print '\nAveraging beam spot data from',filename,':'

    f = ROOT.TFile(filename)
    if f.Get('BeamSpotNt'):
        bsNt = BeamSpotNt(filename)
    elif f.Get('Beamspot/Beamspots'):
        bsNt = BeamSpotFinderNt(filename)
    print '***********',bsNt.treeName,'***********'


    # Get number of vertices from from beamspot tree if variable exists, else from vertex tree (e.g. for older files)
    # Problem with getting number of vertices from beamspot tree:
    # Now that we have multiple fit tools running over the same data, this method could easily double-count the number of vertices
    numVtx = 0
    
    # try:
    # for j in range(bsNt.nt.GetEntries()):
    #     bsNt.nt.GetEntry(j)
    #     numVtx += bsNt.nt.nVtxPrim
    #  except AttributeError:

    if f.Get('Vertices'):
        numVtx = f.Get('Vertices').GetEntriesFast()
    elif f.Get('Beamspot/Vertices'):
        numVtx = f.Get('Beamspot/Vertices').GetEntriesFast()

    totVtx += numVtx
    print '... Found total of %i primary vertices in file (no cuts applied)\n' % numVtx


    for bs in bsNt.allData():
        # Skip fits outside of desired LB range
        if options.lbmin and options.lbmin > bs.lbStart:
            continue
        if options.lbmax and options.lbmax < bs.lbEnd+1:
            continue
        
        fitStatusInt = int( format( bs.status, '08b')[-2:], 2)
        fitIDInt = int( format( bs.status, '08b')[:4], 2)

        print '[%i, %3i - %3i]:  %5i vertices, fitStatus = %s, fitID = %i' % (bs.run,bs.lbStart,bs.lbEnd+1,bs.nEvents,fitStatusInt,fitIDInt)

        print 'totEvents %i'% totEvents

        minDQRun = min(bs.run,minDQRun)
        maxDQRun = max(bs.run,maxDQRun)

        # Skip undesired fits
        if fitStatusInt != 3:
            print '*** Skipping unsuccessful fit ...\n'
            continue
        if not fitIDInt in fitIdList:
            print '*** Skipping fit with ID =',fitIDInt,'\n'
            continue

        nEntries += 1
        minRun = min(bs.run,minRun)
        maxRun = max(bs.run,maxRun)

        for i in range(len(vars)):
            parName = vars[i]['par']
            cov = getattr(bs,vars[i]['ntcov'])
            err = cov
            val = getattr(bs,vars[i]['ntvar'])
            minErr = vars[i].get('minErr',0.)
            minVal = vars[i].get('minVal',-1e10)
            maxVal = vars[i].get('maxVal',1e10)
            fmt = '%%7s:  %s +- %s' % (vars[i]['fmt'],vars[i]['fmt'])
            print '...',fmt % (parName,val,err)
            if err<minErr:
                print '... WARNING: --- NOT USED --- Error of %s too small:  %f    (minErr = %f)' % (parName,err,minErr)
                nWarnings += 1
                continue
            if val<minVal:
                print '... WARNING: Value of %s too small:  %f    (min = %f)' % (parName,val,minVal)
                nWarnings += 1
            if val>maxVal:
                print '... WARNING: Value of %s too large:  %f    (max = %f)' % (parName,val,maxVal)
                nWarnings += 1
            try:
                if options.simpleaverage:
                    w = 1.
                else:
                    w = 1./(cov*cov)
            except:
                w = 0.
                print '... WARNING: Divison by zero for parameter %s   (cov = %f)' % (parName,cov)
                nWarnings +=1
            sumw[i] += w
            sumwx[i] += w*val
        print

    f.Close()

ave = sumwx/sumw
err = 1./numpy.sqrt(sumw)

# Set DQ defect to 'ID_BS_NOBEAMSPOT' if there is no average beamspot or 'ID_BS_RUNAVERAGE' if
# there is (i.e. at least one succesfull fit and not options.noaverage)

if options.dqoutput:
    from InDetBeamSpotExample.DQUtilities import IDBSDefectWriter
    idbsDefects = IDBSDefectWriter(options.dqoutput)    

    if nEntries>0 and not options.noaverage:
        print '\nIntially Setting DQ defect to ID_BS_RUNAVERAGE for runs %s ... %s with average beamspot available' %(minDQRun, maxDQRun)
        idbsDefects.defectType('ID_BS_RUNAVERAGE')
    else:
        print '\nInititally setting DQ defect to ID_BS_NOBEAMSPOT for runs %s ... %s without average beamspot available' % (minDQRun, maxDQRun)
        idbsDefects.defectType('ID_BS_NOBEAMSPOT')

if nEntries>0:
    print '\nAverage from run(s):  %s ... %s  (%i entries, %i vertices)\n' % (minRun,maxRun,nEntries,totEvents)
    if nWarnings:
        print 'WARNING: THERE WERE',nWarnings,'WARNING(S) - PLEASE CHECK\n'
    for i in range(len(vars)):
        if options.simpleaverage:
            fmt = '%%7s:  %s' % (vars[i]['fmt'])
            print fmt % (vars[i]['par'], ave[i])
        else:
            fmt = '%%7s:  %s +- %s' % (vars[i]['fmt'],vars[i]['fmt'])
            print fmt % (vars[i]['par'], ave[i], err[i])
else:
    if totVtx>0:
        print '\nERROR: No beam spot entries found\n'
        sys.exit(2)
    else:
        print '\nERROR: Neither primary vertices nor beam spot found\n'
        sys.exit(3)


# Write COOL files for beam spot
if options.output and nEntries>0:

    print '\nUpdating COOL SQLite file %s:\n' % options.output
    folderHandle = openBeamSpotDbFile(options.output,not options.extend)

    # Global default - this will erase any existing contents!
    if options.globaldefault:
        print '\nWriting global default beam spot\n'
        writeBeamSpotEntry(folderHandle,tag=options.tag)        

    # Average
    if not options.noaverage:
        print '\nWriting average beam spot centroid position w/large width for runs  %s ... %s\n' % (minRun,maxRun)
        writeBeamSpotEntry(folderHandle,tag=options.tag,
                           runMin=minRun,runMax=maxRun,
                           status=options.avealgcode,
                           posX = ave[idx('posX')], posY = ave[idx('posY')], posZ = ave[idx('posZ')],
                           # We use only the position and tilt for the default average beam spot - defaults
                           # are large widths for a non-constraining beam spot and no error
                           tiltX = ave[idx('tiltX')],  tiltY = ave[idx('tiltY')],
                           posXErr = err[idx('posX')], posYErr = err[idx('posY')], posZErr = err[idx('posZ')],                           
                           tiltXErr = err[idx('tiltX')], tiltYErr = err[idx('tiltY')],
                           )

    # Loop over ntuple and merge in all beam spots from COOLBeamspot branch
    # Keep list of those LBs where the fit was succesful for DQ flags
    fittedLBs = {}
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
            try:
                runEndInt = bs.runEnd
            except AttributeError:
                runEndInt = bs.run
                
            if bs.status in statList:
                writeBeamSpotEntry(folderHandle,tag=options.tag,
                                   runMin=bs.run, runMax=runEndInt,
                                   lbMin=bs.lbStart, lbMax=bs.lbEnd,    # Beware - lbMax is inclusive
                                   status=bs.status,
                                   posX=bs.posX, posY=bs.posY, posZ=bs.posZ,
                                   sigmaX=bs.sigmaX, sigmaY=bs.sigmaY, sigmaZ=bs.sigmaZ,
                                   tiltX=bs.tiltX, tiltY=bs.tiltY,
                                   sigmaXY=bs.rhoXY*bs.sigmaX*bs.sigmaY,
                                   posXErr=bs.posXErr, posYErr=bs.posYErr, posZErr=bs.posZErr,
                                   sigmaXErr=bs.sigmaXErr, sigmaYErr=bs.sigmaYErr, sigmaZErr=bs.sigmaZErr,
                                   tiltXErr=bs.tiltXErr, tiltYErr=bs.tiltYErr,
                                   sigmaXYErr=sqrt( (bs.sigmaX*bs.sigmaX) * (bs.sigmaY*bs.sigmaY) * (bs.rhoXYErr*bs.rhoXYErr) +(bs.sigmaX*bs.sigmaX) * (bs.sigmaYErr*bs.sigmaYErr) * (bs.rhoXY*bs.rhoXY) + (bs.sigmaXErr*bs.sigmaXErr) * (bs.sigmaY*bs.sigmaY) * (bs.rhoXY*bs.rhoXY) ) )


                if options.dqoutput:
                    #  Fill GOOD Lbs into defects
                    idbsDefects.add(minDQRun, maxDQRun, bs.lbStart, bs.lumiEnd+1)

            else:
                print 'WARNING: Skipping entry with status word %s' % bs.status

        f.Close()

    # Set the DQ flgs to green for the run-lBs where the fit was succesful.  Group contigous LB ranges.
    if options.dqoutput:
        # Invert to get bad LBs
        idbsDefects.invert(minDQRun, maxDQRun)
        # Write to sqlite dba nd text file
        idbsDefects.writeDefects()
        idbsDefects.dump(options.dqoutput)
    
    if options.lbmin or options.lbmax:
        print '\n**** WARNING: ONLY COPIED ENTRIES IN RANGE [%s,%s] ****\n' % (options.lbmin,options.lbmax)

# Enter interactive mode if desired
if options.interactive:
    os.environ['PYTHONINSPECT'] = '1'
