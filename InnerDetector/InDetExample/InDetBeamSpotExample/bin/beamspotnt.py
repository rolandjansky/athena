#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
beamspotnt is a command line utility for beam spot ntuples.
"""
__author__  = 'Juerg Beringer'
__version__ = 'beamspotnt.py atlas/athena'
__usage__   = '''%prog [options] command [args ...]

Commands are:

dump                  Dump contents of a beam spot ntuple
maketable             Make LaTeX table of selected entries
inspect               Inspect ntuple and provide summary of contents
merge SRCNT           Merge source beam spot ntuple into master ntuple
                      (use --srcnt to specify type if not BeamSpotFinderNt)
ave                   Calculate average of beam spot parameters
hist var              Histogram of var
histvspileup var      Profile histogram of var vs pileup
pull var:value        Pull of var with respect to true value
plot var              History of var
summary               Summary history plots
ascii filename        Write ASCII file with data in Massi\'s format

Examples:

beamspotnt -t BeamSpotCOOL -f IndetBeampos-ES1-UPD2 --ru 165815 --rl 165815 dump
'''

#periodDef = '/afs/cern.ch/atlas/www/GROUPS/DATAPREPARATION/DataPeriods'
periodDef = '/afs/cern.ch/user/a/atlidbs/nt/DataPeriods'

import sys, os, time, glob, re, copy, math

# Create a properly quoted string of the command line to save
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

from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-f', '--nt', dest='ntname', default='beamspotnt.root', help='master ntuple file name or COOL tag (default: beamspotnt.root)')
parser.add_option('-t', '--type', dest='type', default='BeamSpotNt', help='master ntuple type (default: BeamSpotNt)')
parser.add_option('-s', '--srctype', dest='srctype', default='BeamSpotNt', help='source ntuple type for merging (default: BeamSpotNt)')
parser.add_option('-v', '--verbose', dest='verbose', action='store_true', default=False, help='verbose output')
parser.add_option('-d', '--debug', dest='debug', action='store_true', default=False, help='debug output')
parser.add_option('-q', '--quiet', dest='quiet', action='store_true', default=False, help='reduce output')
parser.add_option('-r', '--run', dest='run', type='int', default=None, help='run number (single run)')
parser.add_option('', '--rl', dest='runMin', type='int', default=None, help='minimum run number (inclusive)')
parser.add_option('', '--ru', dest='runMax', type='int', default=None, help='maximum run number (inclusive)')
parser.add_option('', '--re', dest='runExclude', default=None, help='comma seperated list of runs to exclude')
parser.add_option('', '--fill', dest='fill', type='int', default=None, help='fill number (single fill)')
parser.add_option('', '--fl', dest='fillMin', type='int', default=None, help='minimum fill number (inclusive)')
parser.add_option('', '--fu', dest='fillMax', type='int', default=None, help='maximum fill number (inclusive)')
parser.add_option('', '--bcid', dest='bcid', type='int', default=None, help='BCID number (single BCID)')
parser.add_option('', '--bcl', dest='bcidMin', type='int', default=None, help='minimum BCID number (inclusive)')
parser.add_option('', '--bcu', dest='bcidMax', type='int', default=None, help='maximum BCID number (inclusive)')
parser.add_option('', '--lbl', dest='lbMin', type='int', default=None, help='minimum LB number (inclusive)')
parser.add_option('', '--lbu', dest='lbMax', type='int', default=None, help='maximum LB number (inclusive)')
parser.add_option('', '--tl', dest='timeMin', default=None, help='minimum start time')
parser.add_option('', '--tu', dest='timeMax', default=None, help='maximum end time')
parser.add_option('', '--status', dest='status', default='59', help='comma-separated list of accepted fit status values (default: 59)')
parser.add_option('', '--energy', dest='energy', default='13', help='center-of-mass energy')
parser.add_option('-p', '--period', dest='period', default=None, help='comma-separated list of accepted run periods (e.g. data10_7TeV.G)')
parser.add_option('', '--perioddef', dest='periodDef', default=periodDef, help='location of run period definition files (default: %s)' % periodDef)
parser.add_option('-g', '--grl', dest='grl', default=None, help='GRL to select runs/Lbs')
parser.add_option('', '--mc', dest='mc', action='store_true', default=False, help='flag to indicate beam spot results are from MC')
parser.add_option('', '--fillCOOL', dest='fillcooldata', action='store_true', default=False, help='fill data from COOL (time, fill#)')
parser.add_option('', '--fillDQ', dest='filldqdata', action='store_true', default=False, help='fill data from DataQualtiy')
parser.add_option('', '--pLbFile', dest='pseudoLbFile', default=None, help='File for pseudo LB info from scan')
parser.add_option('', '--pLbTimeUnit', dest='pseudoLbTimeUnit', default=1., help='Time unit for pseudo LB relative to seconds, e.g. 1e-9 for ns')
parser.add_option('', '--tz', dest='timezone', default='CET', help='time zone to use for time represenation (default: CERN local time)')
parser.add_option('', '--simpleaverage', dest='simpleaverage', action='store_true', default=False, help='use simple instead of weighted average (for checks only)')
parser.add_option('', '--lumicalcnt', dest='lumicalcnt', default=None, help='ave: use luminosity ntuple (from iLumiCalc.exe) for weighted average')
parser.add_option('', '--latex', dest='latex', action='store_true', default=False, help='output results in LaTeX')
parser.add_option('', '--splittable', dest='splittable', type='int', default=None, help='latex: split table, put first n cols into first table')
parser.add_option('', '--hist', dest='hist', action='store_true', default=False, help='ave: produce validation histograms')
parser.add_option('', '--summary', dest='summary', action='store_true', default=False, help='ave: produce validation summary plots')
parser.add_option('', '--rms', dest='rms', action='store_true', default=False, help='Use RMS instead of error on mean')
parser.add_option('', '--varlist', dest='varlist', default=None, help='list of beam spot variables used by ave')
parser.add_option('-n', '--nbins', dest='nbins', type='int', default=None, help='number of bins')
parser.add_option('', '--xmin', dest='xmin', default=None, help='x axis minimum')
parser.add_option('', '--xmax', dest='xmax', default=None, help='x axis maximum')
parser.add_option('', '--timeaxis', dest='timeaxis', action='store_true', default=False, help='use time on x axis instead of run or LB numbers')
parser.add_option('', '--talabel', dest='talabel', default=None, help='time axis label (default: Time (timezone))')
parser.add_option('', '--perbcid', dest='perbcid', action='store_true', default=False, help='plot per BCID instead of vs time/LB')
parser.add_option('', '--separation', dest='separation', action='store_true', default=False, help='plot versus beam separation for scans')
parser.add_option('', '--vsbunchpos', dest='vsbunchpos', action='store_true', default=False, help='plot versus bunch position in train')
parser.add_option('', '--ndivs', dest='ndivs', type='int', default=None, help='Number of x axis divisions (ROOT format)')
parser.add_option('', '--ymin', dest='ymin', type='float', default=None, help='y axis minimum')
parser.add_option('', '--ymax', dest='ymax', type='float', default=None, help='y axis maximum')
parser.add_option('-y', '--yscale', dest='yscale', type='float', default=1.0, help='y axis scale factor')
parser.add_option('-m', '--msize', dest='msize', type='float', default=None, help='set marker size')
parser.add_option('', '--lsize', dest='lsize', type='float', default=None, help='set axis label size')
parser.add_option('', '--ydef', dest='ydef', action='store_true', default=False, help='use default y range for given quantity')
parser.add_option('', '--defaults', dest='defaults', default='Gen', help='choose varDef defauls (default: Gen')
parser.add_option('', '--logy', dest='logy', action='store_true', default=False, help='log scale')
parser.add_option('', '--fit', dest='fit', default='', help='fit function (e.g. pol1)')
parser.add_option('', '--optfit', dest='optfit', default='', help="fit options (default: '')")
parser.add_option('-c', '--canvas', dest='canvas', default='', help='canvas size: default, page, wide, extrawide or square')
parser.add_option('', '--xdivs', dest='xdivs', type='int', default=1, help='number of pads on canvas in x (needs -c, default: 1)')
parser.add_option('', '--ydivs', dest='ydivs', type='int', default=1, help='number of pads on canvas in y (needs -c, default: 1)')
parser.add_option('', '--optstat', dest='optstat', default='emruo', help='default OptStat value (Default: emruo)')
parser.add_option('', '--ave', dest='ave', action='store_true', default=False, help='show average on plot')
parser.add_option('', '--newave', dest='newave', action='store_true', default=False, help='replicate the functionality of the old aveBeamSpot.py script.')
parser.add_option('', '--title', dest='title', default='', help='title text on plots')
parser.add_option('', '--comment', dest='comment', default='', help='additional text (use semicolon to indicate line breaks)')
parser.add_option('', '--datefmt', dest='datefmt', default='', help='date format')
parser.add_option('', '--adatefmt', dest='adatefmt', default='', help='axis date format')
parser.add_option('', '--plegend', dest='plegend', default='fit to groups of LBs', help='legend text for points')
parser.add_option('', '--alegend', dest='alegend', default='average of data shown:', help='legend text for average')
parser.add_option('', '--maxlegends', dest='maxlegends', type='int', default=10, help='maximum number of legend entries')
parser.add_option('', '--omitfill', dest='omitfill', action='store_true', default=False, help='remove any fill info from legend')
parser.add_option('', '--omitrun', dest='omitrun', action='store_true', default=False, help='remove any run info from legend')
parser.add_option('', '--omittime', dest='omittime', action='store_true', default=False, help='remove any time info from legend')
parser.add_option('', '--public', dest='public', action='store_true', default=False, help='use labelling for public plots')
parser.add_option('', '--prelim', dest='prelim', action='store_true', default=False, help='add ATLAS Preliminary to figure')
parser.add_option('', '--approval', dest='approval', action='store_true', default=False, help='Label figure ATLAS for approval')
parser.add_option('', '--published', dest='published', action='store_true', default=False, help='add ATLAS to figure')
parser.add_option('', '--customlabel', dest='customlabel', default='', help='add custom label after ATLAS to figure')
parser.add_option('', '--xtitoffset', dest='xtitoffset', type='float', default=None, help='x axis title offset')
parser.add_option('', '--ytitoffset', dest='ytitoffset', type='float', default=None, help='y axis title offset')
parser.add_option('', '--atlasx', dest='atlasx', type='float', default=None, help='x position for drawing ATLAS (Preliminary) label')
parser.add_option('', '--atlasy', dest='atlasy', type='float', default=None, help='y position for drawing ATLAS (Preliminary) label')
parser.add_option('', '--atlasdx', dest='atlasdx', type='float', default=None, help='x position offset for drawing Preliminary label')
parser.add_option('-o', '--output', dest='output', default='.gif', help='comma-separated list of output files or formats (default: .gif)')
parser.add_option('', '--name', dest='name', default='BeamSpotNtPlots', help='base name for plots (default: BeamSpotNtPlots)')
parser.add_option('', '--tgraphfile', dest='tgraphfile', default='', help='optional root file to save TGraph into')
parser.add_option('', '--tag', dest='cooltag', default='', help='COOL tag for writing average into beam spot SQLite file (also determines file name)')
parser.add_option('-i', '--interactive', dest='interactive', action='store_true', default=False, help='interactive')
parser.add_option('-b', '--batch', dest='batch', action='store_true', default=False, help='run in batch mode')
parser.add_option('', '--fullCorrelations', dest='fullCorrelations', action='store_true', default=False, help='Get all correlations')
parser.add_option('', '--scans', dest='scans', default='', help='comma-separated list of hypenated lb ranges for lumi scans')
parser.add_option('', '--acqFlag', dest='acqFlag', default = False, action='store_true', help='Cut on AcquistionFlag=1.0 for stationary points of VdM scan')
parser.add_option('', '--overlayScans', dest='overlayScans', default = False, action='store_true', help='overlay VdM scans on same plot')
parser.add_option('', '--useAve', dest='useAve', action='store_true', default=False, help='Average over poor fits in the beamspot -- performed during merging step')
parser.add_option('', '--database', dest='database', default='COOLOFL_INDET/CONDBR2', help='DB to read beamspot info from when -s BeamSpotCOOL is enabled and running merge')
parser.add_option('', '--dbfolder', dest='dbfolder', default='/Indet/Beampos', help='Folder to read beamspot info from when -s BeamSpotCOOL is enabled and running merge')

(options,args) = parser.parse_args()
if len(args) < 1:
    parser.error('wrong number of command line arguments')
cmd = args[0]

if options.fillcooldata and options.pseudoLbFile:
    sys.exit('Options fillColl and pseudoLbFile are mutually exclusive')

# Reset DISPLAY if in batch
if options.batch:
    os.unsetenv('DISPLAY')

# Place after OptionParse to avoid conflict with ROOT option parsing
import ROOT
import InDetBeamSpotExample

# Get BeamSpotData class and choose beam spot variable defaults
from InDetBeamSpotExample import BeamSpotData
if options.defaults:
    BeamSpotData.varDefs = getattr(BeamSpotData,'varDefs'+options.defaults)
from InDetBeamSpotExample.BeamSpotData import *

#from InDetBeamSpotExample.COOLUtils import *
from InDetBeamSpotExample.Utils import getUserName, getHostName
from InDetBeamSpotExample import ROOTUtils
ROOTUtils.setStyle()
from math import *

cmdOk = False
if options.interactive:
    os.environ['PYTHONINSPECT'] = '1'

if options.timezone:
    os.environ['TZ'] = options.timezone
    time.tzset()
    timeLabel = 'Time (%s)' % options.timezone
else:
    timeLabel = 'Local time'
if options.talabel:
    timeLabel = options.talabel

if not options.datefmt:
    if options.public:
        options.datefmt = '%b %Y'
    else:
        options.datefmt = '%a %b %d'

# Graphics defaults
if options.atlasx==None:
    if options.published:
        options.atlasx = 0.83 if 'wide' in options.canvas else 0.8
    else:
        options.atlasx = 0.72 if 'wide' in options.canvas else 0.645
if options.atlasy==None:
    if options.public:
        options.atlasy = 0.86
if options.atlasdx==None:
    options.atlasdx = 0.08 if 'wide' in options.canvas else 0.115

ntClass = locals()[options.type]

def setCuts(nt):
    if options.run!=None:
        nt.runMin = options.run
        nt.runMax = options.run
    if options.runMin!=None:
        nt.runMin = options.runMin
    if options.runMax!=None:
        nt.runMax = options.runMax
    if options.runExclude!=None:
        nt.runListExclude = [int(r) for r in options.runExclude.split(',')]
    if options.fill!=None:
        nt.fillMin = options.fill
        nt.fillMax = options.fill
    if options.fillMin!=None:
        nt.fillMin = options.fillMin
    if options.fillMax!=None:
        nt.fillMax = options.fillMax
    if options.bcid!=None:
        nt.bcidMin = options.bcid
        nt.bcidMax = options.bcid
    if options.bcidMin!=None:
        nt.bcidMin = options.bcidMin
    if options.bcidMax!=None:
        nt.bcidMax = options.bcidMax
    if options.lbMin!=None:
        nt.lbMin = options.lbMin
    if options.lbMax!=None:
        nt.lbMax = options.lbMax
    if options.timeMin!=None:
        nt.timeMin = time.mktime(time.strptime(options.timeMin,'%b %d %H:%M:%S %Y'))
    if options.timeMax!=None:
        nt.timeMax = time.mktime(time.strptime(options.timeMax,'%b %d %H:%M:%S %Y'))
    if options.status:
        nt.statusList = [int(x) for x in options.status.split(',')]
    if options.period:
        for p in options.period.split(','):                        
            project,period = p.split('.')
            location = os.path.normpath(options.periodDef + '/' + project)
            try:
                fileName = glob.glob('%s/%s.runs.list' % (location,period))[0]
                nt.runList.extend([int(r) for r in open(fileName).read().split()])
            except:
                sys.exit('ERROR: Definition file for period %s not found in directory %s' % (options.period, location))

        if not nt.runList:
            sys.exit('ERROR: no runs found for requested periods')

    if options.grl:
        nt.grl = options.grl

    if options.acqFlag:
        nt.acqFlag = 1.0
    
def getNt():
    try:
        if options.fullCorrelations:
            nt = ntClass(options.ntname,fullCorrelations=True)
        else:
            nt = ntClass(options.ntname)
    except Exception,e:
        sys.exit('ERROR: '+str(e))

    setCuts(nt)
    print nt.summary()
    print nt.cutSummary()
    return nt
    
def cleanUpLowStat( allBSResultsInNt, averagenVtx, lbSize ):
    i=0 
    while  i < len( allBSResultsInNt ):
      b = allBSResultsInNt[i]
      if b.status < 70 and  b.sigmaZErr == 0:
        print "Will change Z error for  lb's " + str(b.lbStart) +" to " + str(b.lbEnd) + " which has " + str(b.nValid) + " verticies"
        b.sigmaZErr = b.sigmaZ * 0.5
      i += 1 
   
    i=0
    while  i < len( allBSResultsInNt ):
      b = allBSResultsInNt[i]

      if b.status < 70 and b.nValid < 2000 and b.nValid < averagenVtx:
        print "Will take an average for  lb's " + str(b.lbStart) +" to " + str(b.lbEnd) + " which has " + str(b.nValid) + " verticies" 
        lastGoodEntry = b
        nextGoodEntry = b
        iNeg = i-1;
        # Find previous good entry
        while iNeg >= 0:
          if allBSResultsInNt[iNeg].status == 59 and allBSResultsInNt[iNeg].nValid > 2000 :
            lastGoodEntry = allBSResultsInNt[iNeg]
            print " --- Starting with lb : " + str(lastGoodEntry.lbStart) +" to " + str(lastGoodEntry.lbEnd)
            break
          iNeg -= 1 
    
        # Find the next good entry
        iPos = i+1;
        while iPos < len(allBSResultsInNt):
          if allBSResultsInNt[iPos].status == 59 and allBSResultsInNt[iPos].nValid > 2000:
            nextGoodEntry = allBSResultsInNt[iPos]
            print " --- Ending with lb   : " + str(nextGoodEntry.lbStart) +" to " + str(nextGoodEntry.lbEnd)
            break
          iPos += 1 
    
        #if all entries are useless then we are in trouble dont do anything
        if lastGoodEntry == b and nextGoodEntry == b :
          print "Failed to do average - no good entries were found"
          i+=1
          continue

        #check the entries are reasonablly close to each other
        if( ( nextGoodEntry == b or abs(nextGoodEntry.lbStart - b.lbEnd) > abs(lbSize) ) and (lastGoodEntry == b or abs(b.lbStart - lastGoodEntry.lbEnd) > abs(lbSize) ) ):
          print "Failed to do average - entries were too far away"
          i+=1
          continue 

        #Calculate the average beamspot position for the following parameters
        varList = ['posX','posY','posZ','sigmaX','sigmaY','sigmaZ','tiltX','tiltY','rhoXY','sigmaXY']
        calc = BeamSpotAverage(varList ,weightedAverage=True)
        #Add current entry if it is reliable 
        if( b.status == 59 and b.posXErr != 0 and not math.isnan(b.posX) and not math.isnan(b.posZ) and not math.isnan(b.sigmaZ) ):
          calc.add(b)
        #Add previous entry if it is not too far away in time
        if lastGoodEntry != b and abs(b.lbStart - lastGoodEntry.lbEnd) <= abs(lbSize) :
          calc.add(lastGoodEntry)
        #Add next entry if it is not too far away in time
        if nextGoodEntry != b and abs(nextGoodEntry.lbStart - b.lbEnd) <= abs(lbSize) :
          calc.add(nextGoodEntry)
        calc.average()

        ave = calc.ave
        err = calc.err
        b.status = 666 #b.status << 4
        bcopy = copy.deepcopy(b)

        for var in varList:
          #print "Var,index: {:>10} ,  {:>3}".format( var,  calc.varList.index(var))
          setattr(bcopy, var,       ave[calc.varList.index(var)])
          setattr(bcopy, var+"Err", err[calc.varList.index(var)])
        
        bcopy.status = 59        
        i += 1
        allBSResultsInNt.insert(i, bcopy)
      i += 1




def fillInMissingLbs(allBSResultsInNt, lbSize):
      i=0
      lastValidEntry  = -1
      nextValidEntry  = -1
      while  i < len( allBSResultsInNt ):
        if allBSResultsInNt[i].status != 59: 
          i += 1
          continue

        nextValidEntry = i
          
        if(lastValidEntry >= 0):
          if allBSResultsInNt[nextValidEntry].lbStart !=  allBSResultsInNt[lastValidEntry].lbEnd + 1:
            print "Missing Lumi block from {:>5d} to {:>5d}".format( allBSResultsInNt[lastValidEntry].lbEnd + 1 , allBSResultsInNt[nextValidEntry].lbStart)
            
            
            if allBSResultsInNt[nextValidEntry].lbStart -  allBSResultsInNt[lastValidEntry].lbEnd + 1 > lbSize:
              print "--Lumi block gap too large wont fill in the gap"           
            elif (allBSResultsInNt[nextValidEntry].lbStart-1) -  (allBSResultsInNt[lastValidEntry].lbEnd+1) < 0 :
              print "Missing Lumi block is invalid from {:>5d} to {:>5d}".format( allBSResultsInNt[lastValidEntry].lbEnd+1, allBSResultsInNt[nextValidEntry].lbStart -1)
            else:
              varList = ['posX','posY','posZ','sigmaX','sigmaY','sigmaZ','tiltX','tiltY','rhoXY','sigmaXY']
              calc = BeamSpotAverage(varList ,weightedAverage=True)
              calc.add(allBSResultsInNt[nextValidEntry])
              calc.add(allBSResultsInNt[lastValidEntry])
              calc.average()

              ave = calc.ave
              err = calc.err

              bcopy = copy.deepcopy(b)
              
              for var in varList:
                #print "Var,index: {:>10} ,  {:>3}".format( var,  calc.varList.index(var))
                setattr(bcopy, var,       ave[calc.varList.index(var)])
                setattr(bcopy, var+"Err", err[calc.varList.index(var)])

              bcopy.status    = 59
              bcopy.timeStart = 0 
              bcopy.timeEnd   = 0   
              bcopy.nEvents   = 1   
              bcopy.nValid    = 1    
              bcopy.nVtxAll   = 1   
              bcopy.nVtxPrim  = 1  
              bcopy.lbStart   = allBSResultsInNt[lastValidEntry].lbEnd + 1    
              bcopy.lbEnd     = allBSResultsInNt[nextValidEntry].lbStart-1
              allBSResultsInNt.insert(lastValidEntry+1, bcopy)
              i += 1
              nextValidEntry += 1

        lastValidEntry = nextValidEntry
        i += 1  



class Plots(ROOTUtils.PlotLibrary):

    def __init__(self,nt):
        ROOTUtils.PlotLibrary.__init__(self,options.name)
        self.nt = nt
        self.whatList = ['posX','posY','posZ','sigmaX','sigmaY','sigmaZ','tiltX','tiltY','rhoXY','k']

    def writeText(self, var, legendList=[],
                  showRuns=True, showFills=True, showTime=True, otherTitle=None, otherComment=None,
                  legendX=0.6):
        # Title and comment lines with optional run, fill and time/date info
        if otherTitle!=None:
            self.protect( ROOTUtils.drawText(0.2,0.86,0.06,otherTitle) )
        else:
            title = options.title or varDef(var,'title',var)
            self.protect( ROOTUtils.drawText(0.2,0.86,0.06,title) )
        comments = []
        if otherComment:
            comments.append(otherComment)
        if options.comment:
            comments.append(options.comment)
        if showRuns and not options.omitrun:
            if self.nt.selRunMin==self.nt.selRunMax:
                comments.append('Run %i' % self.nt.selRunMin)
            else:
                comments.append('Runs %i - %i' % (self.nt.selRunMin,self.nt.selRunMax))
        if showFills and not options.omitfill:
            if self.nt.selFillMin==self.nt.selFillMax:
                comments.append('Fill %i' % self.nt.selFillMin)
            else:
                comments.append('Fills %i - %i' % (self.nt.selFillMin,self.nt.selFillMax))
        if showTime and not options.omittime:
            if options.public:
                t1 = time.strftime(options.datefmt,time.localtime(self.nt.selTimeMin))
                t2 = time.strftime(options.datefmt,time.localtime(self.nt.selTimeMax))
            else:
                t1 = time.strftime(options.datefmt,time.localtime(self.nt.selTimeMin))
                t2 = time.strftime(options.datefmt,time.localtime(self.nt.selTimeMax))
            if t1==t2:
                comments.append(t1)
            else:
                comments.append('%s - %s' % (t1,t2))

        self.protect( ROOTUtils.drawText(0.2,0.79,0.06,';'.join(comments),font=42) )
        # ATLAS (preliminary) label
        logoSize = options.lsize if options.lsize else 0.5
        if options.prelim:
            ROOTUtils.atlasLabel(options.atlasx,options.atlasy,True,offset=options.atlasdx,energy=options.energy,size=logoSize)
        if options.approval:
            ROOTUtils.atlasLabel(options.atlasx,options.atlasy,False,offset=options.atlasdx,isForApproval=True,energy=options.energy,size=logoSize)
        if options.published:
            ROOTUtils.atlasLabel(options.atlasx,options.atlasy,False,offset=options.atlasdx,energy=options.energy,size=logoSize)
        if options.customlabel!="":
            ROOTUtils.atlasLabel(options.atlasx,options.atlasy,False,offset=options.atlasdx,isForApproval=False,customstring=options.customlabel,energy=options.energy,size=logoSize)
        # Legend
        if legendList:
            legendMinY = max(0.91-0.07*len(legendList),0.2)
            self.protect( ROOTUtils.drawLegend(legendX,legendMinY,0.92,0.91,legendList) )

    def hist(self,what):
        """Histogram of variable what.

        If what is of the form NAME:VALUE, VALUE will be subtracted
        from each entry in order to produce plots of measured minus
        true value."""
        shift = 0.
        try:
            (what,shift) = what.split(':')
            shift = float(shift)
        except:
            pass
        ROOT.gStyle.SetOptStat(options.optstat) # emruo to add underflow, overflow
        title = varDef(what,'title',what)+';'+varDef(what,'atit',what)
        nBins = options.nbins if options.nbins else varDef(what,'nBins',100)
        xmin = float(options.xmin) if options.xmin!=None else varDef(what,'min',-100)
        xmax = float(options.xmax) if options.xmax!=None else varDef(what,'max',+100)
        h = self.protect( ROOT.TH1F(what,title,nBins,xmin,xmax) )
        if options.fit:
            h.Sumw2()
        h.GetYaxis().SetTitle('Number of fit results')
        arescale = varDef(what,'arescale',1.0)
        try:
            for b in nt:
                h.Fill(arescale*(getattr(b,what)-shift))
        except Exception, e:
            print 'ERROR filling histogram:',str(e)
        h.Draw()
        if options.fit:
            ROOT.gStyle.SetOptFit(1111)
            h.Fit(options.fit,options.optfit)
        if options.logy:
            ROOT.gPad.SetLogy(options.logy)
        ndivs = varDef(what,'ndivs',override=options.ndivs)
        if ndivs:
            h.GetXaxis().SetNdivisions(ndivs,0)
        if options.mc:
            self.writeText(what,[],False,False,False)
        else:
            self.writeText(what,[],not options.public,options.public)

    def histvspileup(self,what):
        """Profile histogram of variable what vs pileup.

        If what is of the form NAME:VALUE, VALUE will be subtracted
        from each entry in order to produce plots of measured minus
        true value."""
        shift = 0.
        try:
            (what,shift) = what.split(':')
            shift = float(shift)
        except:
            pass
        ROOT.gStyle.SetOptStat(options.optstat) # emruo to add underflow, overflow
        title = varDef(what,'title',what)+';Number of pileup vertices;'+varDef(what,'atit',what)
        nBins = options.nbins if options.nbins else varDef(what,'nBins',100)
        xmin = float(options.xmin) if options.xmin!=None else varDef(what,'min',-100)
        xmax = float(options.xmax) if options.xmax!=None else varDef(what,'max',+100)
        h = self.protect( ROOT.TProfile(what,title,nBins,0,nBins) )
        if options.fit:
            h.Sumw2()
        h.GetYaxis().SetRangeUser(xmin,xmax)
        arescale = varDef(what,'arescale',1.0)
        try:
            for b in nt:
                h.Fill(getattr(b,'pileup'),arescale*(getattr(b,what)-shift))
        except Exception, e:
            print 'ERROR filling histogram:',str(e)
        h.Draw()
        if options.fit:
            ROOT.gStyle.SetOptFit(1111)
            h.Fit(options.fit,options.optfit)
        if options.logy:
            ROOT.gPad.SetLogy(options.logy)
        ndivs = varDef(what,'ndivs',override=options.ndivs)
        if ndivs:
            h.GetYaxis().SetNdivisions(ndivs,0)
        if options.mc:
            self.writeText(what,[],False,False,False)
        else:
            self.writeText(what,[],not options.public,options.public)

    def pull(self,what):
        """Pull histogram of variable what.

        what must be of the form name:value, where value is the
        correct value of variable name."""
        try:
            (what,value) = what.split(':')
            value = float(value)
        except:
            print('ERROR: Illegal input %s for pull variable - must be of the form NAME:VALUE' % what)
            return
        ROOT.gStyle.SetOptStat(options.optstat)
        title = ';Pull'
        nBins = options.nbins if options.nbins else varDef(what,'nBins',100)
        xmin = float(options.xmin) if options.xmin!=None else -4.
        xmax = float(options.xmax) if options.xmax!=None else +4.
        h = self.protect( ROOT.TH1F(what,title,nBins,xmin,xmax) )
        if options.fit:
            h.Sumw2()
        h.GetYaxis().SetTitle('Number of fit results')
        whatErr = what+'Err'
        try:
            for b in nt:
                h.Fill((getattr(b,what)-value)/getattr(b,whatErr))
        except Exception, e:
            print 'ERROR filling histogram:',str(e)
        h.Draw()
        if options.fit:
            ROOT.gStyle.SetOptFit(1111)
            h.Fit(options.fit,options.optfit)
        if options.logy:
            ROOT.gPad.SetLogy(options.logy)
        if options.ndivs:
            h.GetXaxis().SetNdivisions(options.ndivs,0)
        if options.mc:
            self.writeText(what,[],False,False,False)
        else:
            self.writeText(what,[],not options.public,options.public)

    def plot(self,what):
        """History of variable what vs LB or time."""
        ROOT.gStyle.SetOptStat(0)
        if options.ave:
            calc = BeamSpotAverage([what],weightedAverage=not options.simpleaverage)
        grDict = {}

        arescale = varDef(what,'arescale',1.0)
        for b in nt:
            if b.sigmaX < 0.01 or b.sigmaY < 0.01:
                print "OUTLIER ", b.run, b.lbStart, b.lbEnd, b.sigmaX, b.sigmaY
            if not b.bcid in grDict:
                grDict[b.bcid] = BeamSpotGraph(timeAxis=options.timeaxis, separationAxis=options.separation)
            # What was this for? Why ignore if no time info available?
            #if b.timeStart>0 and b.timeEnd>0:
            grDict[b.bcid].add(b,what,arescale)
            if options.ave:
                calc.add(b)

        print 'Plotting data from %i BCID ...\n' % len(grDict)

        # Determine axis range
        xmin = 1E10
        xmax = -1E10
        ymin = 1E10
        ymax = -1E10
        for gr in [grDict.get(bcid) for bcid in grDict]:
            xmin = min(xmin,gr.xmin)
            xmax = max(xmax,gr.xmax)
            ymin = min(ymin,gr.ymin)
            ymax = max(ymax,gr.ymax)

        # Prevent xmin>xmax or ymin>ymax if no succesful fits (but still allow user to override)
        if xmin>xmax:
            xmin,xmax=0,1
        if ymin>ymax:
            ymin,ymax=0,1            

        h = (xmax-xmin)
        xmax += .05*h if len(grDict)==1 else 0.3*h
        xmin -= .05*h
        h = (ymax-ymin)/2*options.yscale
        ymax += 1.3*h
        ymin -= 0.5*h
        if options.xmin!=None:
            xmin = time.mktime(time.strptime(options.xmin,'%b %d %H:%M:%S %Y')) if options.timeaxis else float(options.xmin)
        if options.xmax!=None:
            xmax = time.mktime(time.strptime(options.xmax,'%b %d %H:%M:%S %Y')) if options.timeaxis else float(options.xmax)
        if options.ydef:
            ymin = varDef(what,'min',-100)
            ymax = varDef(what,'max',+100)
        if options.ymin!=None:
            ymin = options.ymin
        if options.ymax!=None:
            ymax = options.ymax

        # Axis settings
        if options.timeaxis:
            atit = ';%s;%s' % (timeLabel,varDef(what,'atit',what))
        elif options.separation:
            atit = ';%s;%s' % ('Nominal Separation [mm]',varDef(what,'atit',what))
        else:
            atit = ';%s;%s' % ('Luminosity block number',varDef(what,'atit',what))
        frame = ROOTUtils.drawAxisFrame(xmin,xmax,ymin,ymax,atit)
        if options.logy:
            ROOT.gPad.SetLogy(options.logy)
        if options.timeaxis:
            xAxis = frame.GetXaxis()
            yAxis = frame.GetYaxis()
            xAxis.SetTimeDisplay(1)

            # this may or may not be needed depending on the ROOT version...  not sure whether to specify it or not.  
            # Setting this option in 5.34 seems to work, in 5.30 it offsets things by an hour, but then not setting it
            # appears to result in bogus x-axis values.
            xAxis.SetTimeOffset(0)   # FIXME: this ignores options.timezone!

            if (xmax-xmin)/86400 < 1:
                xAxis.SetTimeFormat('%H:%M')
            else:
                #d0 = ceil(xmin/86400)  # day w/first tick mark
                #d1 = floor(xmax/86400) # day w/last tick mark
                #nTicks = d1-d0+1
                #nDays = floor((xmax-xmin)/86400)
                if options.public:
                    xAxis.SetTimeFormat('%b %d')
                else:
                    xAxis.SetTimeFormat('#splitline{%b %d}{%H:%M}')
            xAxis.SetLabelOffset(0.02)
            xAxis.SetTitleOffset(1.6)
            if options.lsize:
                xAxis.SetLabelSize(options.lsize)
                xAxis.SetTitleSize(options.lsize)
                yAxis.SetLabelSize(options.lsize)
                yAxis.SetTitleSize(options.lsize)
            else:
                xAxis.SetLabelSize(0.044)
            if options.adatefmt:
                xAxis.SetTimeFormat(options.adatefmt)
            if options.ndivs:
                xAxis.SetNdivisions(options.ndivs,0)
            frame.LabelsOption('d','X')   # Doesn't seem to work

        if options.xtitoffset:
            xAxis.SetTitleOffset(options.xtitoffset)
        if options.ytitoffset:
            yAxis.SetTitleOffset(options.ytitoffset)

        legendList = []

        # Show average
        if options.ave:
            calc.average()
            dx = (xmax-xmin)/30
            mean = calc.ave[0]
            if options.rms:
                error = calc.rms[0]
            else:
                error = calc.err[0]
            b = ROOTUtils.drawHorizontalBand(xmin+dx,xmax-dx,mean,error,centralLine=True)
            legendList.append([b,options.alegend,'FL'])
            legendList.append([None,'%s %s #pm %s %s' % (fmtVal(what,mean,True),varDef(what,'units'),
                                                         fmtVal(what,error,True),varDef(what,'units')),''])

        # Loop over graphs for all BCIDs
        styleFactory = ROOTUtils.StyleFactory()
        for bcid in sorted(grDict.keys()):
            gr = grDict[bcid] 

            # Get and save TGraphErrors, if desired
            name = what if bcid==0 else '%s-bcid%04i' % (what,bcid)
            tgraph = self.protect( gr.getTGraph(name) )
            if not tgraph:
                continue
            if options.tgraphfile:
                print 'Saving TGraphErrors',tgraph.GetName(),'to file',options.tgraphfile
                tgraphfile = ROOT.TFile(options.tgraphfile,'UPDATE')
                tgraph.Write()
                tgraphfile.Close()

            # Draw data points
            if len(grDict)==1:
                tgraph.SetMarkerSize(options.msize if options.msize else 0.8)
                if options.plegend:
                    legendList.append([tgraph,options.plegend,'PLE'])
            else:
                tgraph.SetMarkerColor(styleFactory.nextColorStyle())
                tgraph.SetMarkerStyle(styleFactory.nextMarkerStyle())
                tgraph.SetMarkerSize(options.msize if options.msize else 1.2)
                if len(legendList)<options.maxlegends:
                    text = 'All' if bcid==0 else 'BCID %i' % bcid
                    legendList.append([tgraph,text,'PLE'])
            tgraph.Draw('P')
            
        # Indicate if legend incomplete
        if len(grDict)>options.maxlegends:
            legendList.append([None,'...',''])

        # Fit data, if desired
        if options.fit and len(grDict)==1:
            ROOT.gStyle.SetOptFit(1111)
            legendList = []   # No legend to avoid crowding plot
            tgraph.Fit(options.fit,'','SAME', gr.xmin,gr.xmax) #-0.05, 0.05) #ZEBRA 

        # Draw title and legend
        legendX = 0.6 if len(grDict)==1 else 0.8
        self.writeText(what,legendList if not options.public else [],not options.public,legendX=legendX)

    def perbcid(self,what):
        """Variable what vs BCID."""
        ROOT.gStyle.SetOptStat(0)
        if options.ave:
            calc = BeamSpotAverage([what],weightedAverage=not options.simpleaverage)
        gr = BeamSpotGraph(bcidAxis=True)
        for b in nt:
            gr.add(b,what)
            if options.ave:
                calc.add(b)
        atit = ';%s;%s' % ('BCID',varDef(what,'atit',what))

        # Axis range and labels
        xmin = max(-100,gr.xmin-100)
        xmax = min(3600,gr.xmax+100) # 3564 is the max number of BCIDS + a little space
        ymin = gr.ymin
        ymax = gr.ymax

        # Prevent xmin>xmax or ymin>ymax if no succesful fits (but still allow user to override)
        if xmin>xmax:
            xmin,xmax=0,1
        if ymin>ymax:
            ymin,ymax=0,1            
        
        h = (ymax-ymin)/2*options.yscale
        ymax += 1.3*h
        ymin -= 0.5*h
        if options.ydef:
            ymin = varDef(what,'min',-100)
            ymax = varDef(what,'max',+100)
        if options.ymin!=None:
            ymin = options.ymin
        if options.ymax!=None:
            ymax = options.ymax
        frame = ROOTUtils.drawAxisFrame(xmin,xmax,ymin,ymax,atit)
        if options.logy:
            ROOT.gPad.SetLogy(options.logy)

        legendList = []

        # Show average
        if options.ave:
            calc.average()
            dx = (xmax-xmin)/30
            mean = calc.ave[0]
            if options.rms:
                error = calc.rms[0]
            else:
                error = calc.err[0]
            b = ROOTUtils.drawHorizontalBand(xmin+dx,xmax-dx,mean,error,centralLine=True)
            legendList.append([b,options.alegend,'FL'])
            legendList.append([None,'%s %s #pm %s %s' % (fmtVal(what,mean,True),varDef(what,'units'),
                                                         fmtVal(what,error,True),varDef(what,'units')),''])

        # Save and draw graph
        name = '%s-perBCID' % what
        tgraph = self.protect( gr.getTGraph(name) )
        if not tgraph:
            return
        if options.tgraphfile:
            print 'Saving TGraphErrors',tgraph.GetName(),'to file',options.tgraphfile
            tgraphfile = ROOT.TFile(options.tgraphfile,'UPDATE')
            tgraph.Write()
            tgraphfile.Close()
        tgraph.SetMarkerSize(options.msize if options.msize else 0.8)
        if options.plegend:
            legendList.append([tgraph,options.plegend,'PLE'])
        tgraph.Draw('P')

        # Draw title and legend
        self.writeText(what,legendList,not options.public)

    def vsBunchPos(self,what):
        """Variable what vs bunch position in train."""
        ROOT.gStyle.SetOptStat(0)

        lastb = -999
        startb = -999
        calcmap = {}
        bunchnrs = []
        trainGap = 36
        
        for b in nt:
            if b.bcid - lastb > trainGap:
                startb = b.bcid
            
            lastb = b.bcid
            bunchnumber = b.bcid - startb

            if bunchnumber in calcmap:
                calcmap[bunchnumber].add(b)
            else:
                calcmap[bunchnumber] = BeamSpotAverage([what], weightedAverage = not options.simpleaverage)
                bunchnrs.append(bunchnumber)

        gr = self.protect( ROOT.TGraphErrors(len(bunchnrs)))
        counter = 0
        xmin = -10
        xmax = 1
        ymin = 1e9
        ymax = -1e9
        
        for bunchnr in bunchnrs:
            calcmap[bunchnr].average()
            gr.SetPoint(counter, bunchnr, calcmap[bunchnr].ave[0])
            gr.SetPointError(counter, 0.5, calcmap[bunchnr].err[0])
            xmax = max(xmax, bunchnr)
            ymin = min(ymin, calcmap[bunchnr].ave[0])
            ymax = max(ymax, calcmap[bunchnr].ave[0])
            counter += 1

        if xmin>xmax:
            xmin,xmax=0,1
        if ymin>ymax:
            ymin,ymax=0,1
 
        xmax = xmax + 10
        h = (ymax - ymin)/2*options.yscale
        ymax += 2.0*h
        ymin -= 1.0*h

        if options.xmin != None:
            xmin = options.xmin
        if options.xmax != None:
            xmax = options.xmax
        if options.ymin != None:
            ymin = options.ymin
        if options.ymax != None:
            ymax = options.ymax

        gr.SetMarkerSize(options.msize if options.msize else 0.8)
        
        atit = ';%s;%s' % ('Bunch position in train',varDef(what,'atit',what))
        frame = ROOTUtils.drawAxisFrame(xmin,xmax,ymin,ymax,atit)
        if gr.GetN() == 0:
            return
        
        legendList = []
        if options.plegend:
            legendList.append([gr,options.plegend,'PLE'])
        gr.Draw('P')
        self.writeText(what,legendList,not options.public)

    def plotscan(self,what):
        """History of variable what vs LB, time or separation.  Additionally overlap the results of more than one scan        
        """
        
        ROOT.gStyle.SetOptStat(0)
        if options.ave:
            calc = BeamSpotAverage([what],weightedAverage=not options.simpleaverage)
        grDict = {}

        scans = [(int(s.split('-')[0]), int(s.split('-')[1])) for s in options.scans.split(',')]

        for b in nt:

            if options.separation and options.overlayScans:
                try:
                    scanGroup = [s for s in scans if s[0] <= b.lbStart <= s[1]][0]
                except IndexError:
                    # plB outside of scan ranges
                    continue
            else:
                scanGroup = 0
            
            if not scanGroup in grDict:
                grDict[scanGroup] = BeamSpotGraph(timeAxis=options.timeaxis, separationAxis=options.separation)
            grDict[scanGroup].add(b,what)
            if options.ave:
                calc.add(b)

        print 'Plotting data from %i scans ...\n' % len(grDict)

        # Determine axis range
        xmin = 1E10
        xmax = -1E10
        ymin = 1E10
        ymax = -1E10
        for gr in [grDict.get(scan) for scan in grDict]:
            xmin = min(xmin,gr.xmin)
            xmax = max(xmax,gr.xmax)
            ymin = min(ymin,gr.ymin)
            ymax = max(ymax,gr.ymax)

        # Prevent xmin>xmax or ymin>ymax if no succesful fits (but still allow user to override)
        if xmin>xmax:
            xmin,xmax=0,1
        if ymin>ymax:
            ymin,ymax=0,1            

        h = (xmax-xmin)
        xmax += .05*h if len(grDict)==1 else 0.3*h
        xmin -= .05*h
        h = (ymax-ymin)/2*options.yscale
        ymax += 1.3*h
        ymin -= 0.5*h
        if options.xmin!=None:
            xmin = time.mktime(time.strptime(options.xmin,'%b %d %H:%M:%S %Y')) if options.timeaxis else float(options.xmin)
        if options.xmax!=None:
            xmax = time.mktime(time.strptime(options.xmax,'%b %d %H:%M:%S %Y')) if options.timeaxis else float(options.xmax)
        if options.ydef:
            ymin = varDef(what,'min',-100)
            ymax = varDef(what,'max',+100)
        if options.ymin!=None:
            ymin = options.ymin
        if options.ymax!=None:
            ymax = options.ymax

        # Axis settings
        if options.timeaxis:
            atit = ';%s;%s' % (timeLabel,varDef(what,'atit',what))
        elif options.separation:
            atit = ';%s;%s' % ('Nominal Separation [mm]',varDef(what,'atit',what))
        else:
            atit = ';%s;%s' % ('Luminosity block number',varDef(what,'atit',what))
        frame = ROOTUtils.drawAxisFrame(xmin,xmax,ymin,ymax,atit)
        if options.logy:
            ROOT.gPad.SetLogy(options.logy)
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
            if options.adatefmt:
                xAxis.SetTimeFormat(options.adatefmt)
            if options.ndivs:
                xAxis.SetNdivisions(options.ndivs,0)
            frame.LabelsOption('d','X')   # Doesn't seem to work

        legendList = []

        # Show average
        if options.ave:
            calc.average()
            dx = (xmax-xmin)/30
            mean = calc.ave[0]
            if options.rms:
                error = calc.rms[0]
            else:
                error = calc.err[0]
            b = ROOTUtils.drawHorizontalBand(xmin+dx,xmax-dx,mean,error,centralLine=True)
            legendList.append([b,options.alegend,'FL'])
            legendList.append([None,'%s %s #pm %s %s' % (fmtVal(what,mean,True),varDef(what,'units'),
                                                         fmtVal(what,error,True),varDef(what,'units')),''])

        # Loop over graphs for all scans
        styleFactory = ROOTUtils.StyleFactory()
        for scan in sorted(grDict.keys()):
            gr = grDict[scan] 

            # Get and save TGraphErrors, if desired
            name = what if scan==0 else '%s-scanoverlay' % (what)
            tgraph = self.protect( gr.getTGraph(name) )
            if not tgraph:
                continue
            if options.tgraphfile:
                print 'Saving TGraphErrors',tgraph.GetName(),'to file',options.tgraphfile
                tgraphfile = ROOT.TFile(options.tgraphfile,'UPDATE')
                tgraph.Write()
                tgraphfile.Close()

            # Draw data points
            if len(grDict)==1:
                tgraph.SetMarkerSize(options.msize if options.msize else 0.8)
                if options.plegend:
                    legendList.append([tgraph,options.plegend,'PLE'])
            else:
                tgraph.SetMarkerColor(styleFactory.nextColorStyle())
                tgraph.SetMarkerStyle(styleFactory.nextMarkerStyle())
                tgraph.SetMarkerSize(options.msize if options.msize else 1.2)
                if len(legendList)<options.maxlegends:
                    text = 'All' if scan==0 else 'scan %i - %i' % scan
                    legendList.append([tgraph,text,'PLE'])
            tgraph.Draw('P')                

        # Indicate if legend incomplete
        if len(grDict)>options.maxlegends:
            legendList.append([None,'...',''])

        # Fit data, if desired
        if options.fit and len(grDict)==1:
            ROOT.gStyle.SetOptFit(1111)
            legendList = []   # No legend to avoid crowding plot
            tgraph.Fit(options.fit,'','SAME',gr.xmin,gr.xmax)

        # Draw title and legend
        legendX = 0.6 if len(grDict)==1 else 0.8
        self.writeText(what,legendList,not options.public,legendX=legendX)        


        
#
# Dump beam spot ntuple
#
if cmd=='dump' and len(args)==1:
    print
    nt = getNt()
    for b in nt:
        b.dump(options.verbose)
    cmdOk = True


#
# Make LaTeX table of results
#

tableTemplate = r"""
\documentclass[12pt,twoside]{article}
\usepackage[landscape]{geometry}
\RequirePackage{xspace}
\def\lumposx    {\ensuremath{\overline{x}_{\mathcal{L}}}\xspace}
\def\lumposy    {\ensuremath{\overline{y}_{\mathcal{L}}}\xspace}
\def\lumposz    {\ensuremath{\overline{z}_{\mathcal{L}}}\xspace}
\def\lumsigx    {\ensuremath{\sigma_{x{\mathcal L}}}\xspace} 
\def\lumsigy    {\ensuremath{\sigma_{y{\mathcal L}}}\xspace} 
\def\lumsigz    {\ensuremath{\sigma_{z{\mathcal L}}}\xspace} 
\def\lumtiltx   {\ensuremath{\overline{x}_{\mathcal{L}}'}\xspace}
\def\lumtilty   {\ensuremath{\overline{y}_{\mathcal{L}}'}\xspace}
\def\lumrhoxy   {\ensuremath{\rho_{xy}}\xspace}
\begin{document}

\begin{table}[htbp]
\begin{center}
\begin{tabular}{%s}
\hline \hline
%s \\
\hline \hline

%s

\hline \hline
\end{tabular}
\end{center}
\end{table}

\end{document}
"""

if cmd=='maketable' and len(args)==1:
    if options.varlist is None:
        #varList = ['posX','posY','posZ','sigmaX','sigmaY','sigmaZ','tiltX','tiltY','rhoXY','k']
        varList = ['status','posX','posY','posZ','sigmaX','sigmaY','sigmaZ','k']
    else:
        varList = options.varlist.split(',')
    nt = getNt()
    rows = []
    for b in nt:
        cols = []
        for v in varList:
            try:
                cols.append('%s $\pm$ %s' % (fmtVal(v,getattr(b,v),True,useAlternate=True),
                                             fmtVal(v,getattr(b,v+'Err'),True,useAlternate=True)))
            except:
                cols.append('%10s' % (fmtVal(v,getattr(b,v),True,useAlternate=True)))
        rows.append('%s \\\\' % ' & '.join(cols))
    print
    print tableTemplate % (len(varList)*'c',
                           ' & '.join(['%s' % varDef(n,'latexheader',useAlternate=True) for n in varList]),
                           '\n'.join(rows))
    print
    cmdOk = True


#
# Dump beam spot ntuple
#
if cmd=='inspect' and len(args)==1:
    print
    nt = getNt()
    runInfo = {}
    for b in nt:
        r = b.run
        if not b.run in runInfo:
            runInfo[r] = { 
                'lbStart': None,
                'lbEnd': None,
                'status': set(),
                'hasErrors': False,
                'hasCOOL' : False
            }
        if runInfo[r]['lbStart'] is None or b.lbStart<runInfo[r]['lbStart']: runInfo[r]['lbStart'] = b.lbStart
        if runInfo[r]['lbEnd'] is None or b.lbEnd>runInfo[r]['lbEnd']: runInfo[r]['lbEnd'] = b.lbEnd
        runInfo[r]['status'].add(b.status)
        if b.timeStart and b.timeEnd and b.fill: runInfo[r]['hasCOOL'] = True
        if b.posXErr: runInfo[r]['hasErrors'] = True
    for r in sorted(runInfo.keys()):
        print '%6s   [ %10i, %10i ] %7s  %9s   fit status = %s' % (r,
                                                                   runInfo[r]['lbStart'],
                                                                   runInfo[r]['lbEnd'],
                                                                   'hasCOOL' if runInfo[r]['hasCOOL'] else '',
                                                                   'hasErrors' if runInfo[r]['hasErrors'] else '',
                                                                   sorted(list(runInfo[r]['status'])))
    print
    print '%i runs found\n' % len(runInfo.keys())
    cmdOk = True



#
# Merge beam spot finder ntuple into master beam spot ntuple
#
if cmd=='merge' and len(args)==2:
    srcNtClass = locals()[options.srctype]
    if (options.srctype == 'BeamSpotCOOL'):
      print '\n Reading in from database '
      srcNt = srcNtClass(args[1],database=options.database,folder=options.dbfolder,fullCorrelations=options.fullCorrelations)
    else :
      srcNt = srcNtClass(args[1],fullCorrelations=options.fullCorrelations)

    setCuts(srcNt)
    print '\nImporting from '+srcNt.summary()
    print srcNt.cutSummary()
    dstNt = ntClass(options.ntname,True,fullCorrelations=options.fullCorrelations)
    print '\nMerging into '+dstNt.summary()
    
    totalEntries = 0 
    totalVtxs = 0
    lbSize = 0 
    allBSResultsInNt = []
    for b in srcNt:
      allBSResultsInNt.append( b )
      if b.status == 59:
        totalEntries += 1
        totalVtxs += b.nValid
        lbSize += b.lbEnd - b.lbStart 
        
    if totalEntries == 0:
      totalEntries = 1  
    averagenVtx = totalVtxs/totalEntries  
    print 'Average Entries: '+ str(averagenVtx)
    averagenVtx *= 0.33
    
    lbSize = lbSize/totalEntries + 1
    print 'Average number of lb used for fit: '+ str( lbSize )
    
    #Sort entries in order of lb number       
    #allBSResultsInNt.sort(key=lambda x: x.lbStart, reverse=False)  
    allBSResultsInNt.sort()  
    if options.useAve:
        cleanUpLowStat( allBSResultsInNt, averagenVtx, lbSize * 10)
        allBSResultsInNt.sort()
        fillInMissingLbs(allBSResultsInNt, lbSize * 10)
                   
    for b in allBSResultsInNt:
        if options.fillcooldata:
            b.fillDataFromCOOL()
        if options.pseudoLbFile:
            b.fillDataFromPseudoLb(options.pseudoLbFile, float(options.pseudoLbTimeUnit))        
        if options.filldqdata:
            b.fillDataFromDQ()
        if not options.quiet:
            b.dump(options.verbose)
        dstNt.fill(b)
    cmdOk = True



#
# Calculate average of beam spot parameters
#
if cmd=='ave' and len(args)==1:
    nt = getNt()
    if options.varlist is None:
        #varList = 'posX,posY,posZ,sigmaX,sigmaY,sigmaZ,tiltX,tiltY,rhoXY,sigmaXY,k'
        varList = ['posX','posY','posZ','sigmaX','sigmaY','sigmaZ','tiltX','tiltY','rhoXY']
        if options.cooltag:
            varList.append('sigmaXY')
    else:
        varList = options.varlist.split(',')

    # Summary plot for checks
    plots = Plots(nt)
    plots.whatList = varList
    plots.saveAsList = options.output.split(',')
    plots.allCanvasSize = 'landscape'
    plots.allCanvasDivs = (3,3)
    if options.hist:
        plots.genPlot('all','hist')
    if options.summary:
        plots.genPlot('all','plot')
    print

    # Averages
    calc = BeamSpotAverage(varList,weightedAverage=not options.simpleaverage)
    if options.lumicalcnt is not None:
        calc.readLumiData(options.lumicalcnt)
    minrun=1e10
    maxrun=0
    for b in nt:
        if options.verbose or options.debug:
           b.dump(options.debug)
        calc.add(b)
        minrun = min(b.run,minrun)
        maxrun = max(b.run,maxrun)
    calc.average()
    ave = calc.ave
    err = calc.err
    rms = calc.rms
    if calc.nWarnings:
        print '... %i warnings detected' % calc.nWarnings
    if options.lumicalcnt is not None:
        print '... integrated luminosity of selected runs = %6.1f / pb' % (calc.sumw[0]/1.E6)
    print
    if options.splittable:
        varRanges = [ range(options.splittable),
                      range(options.splittable,len(calc.varList)) ]
    else:
        varRanges = [ range(len(calc.varList)) ]
    if options.latex:
        for r in varRanges:
            try:
                iTable += 1
            except:
                iTable = 1
            latexheader = 'Period '
            latexrow = '%s' % options.period.replace('_','\_')
            print '\nAverage beam spot parameters (part %i):\n' % iTable
            for i in r:
                parName = calc.varList[i]
                print '%7s:  %s +- %s %-3s     (RMS = %s)' % (parName,
                                                              fmtVal(parName,ave[i]),
                                                              fmtVal(parName,err[i]),
                                                              varDef(parName,'units'),
                                                              fmtVal(parName,rms[i]))
                latexheader += '& %s ' % varDef(parName,'latexheader',parName,useAlternate=True)
                if options.rms:
                    latexrow += ' & %s $\pm$ %s' % (fmtVal(parName,ave[i],useAlternate=True),fmtVal(parName,rms[i],useAlternate=True))
                else:
                    latexrow += ' & %s $\pm$ %s' % (fmtVal(parName,ave[i],useAlternate=True),fmtVal(parName,err[i],useAlternate=True))
            print
            print '\nLaTeX code for table %i:\n' % iTable
            print '\\begin{table}[htbp]\n\\begin{center}\n\\begin{tabular}{l%s}' % (len(r)*'c')
            print '\hline \hline'
            print latexheader,'\\\\ \hline'
            print
            print latexrow,' \\\\'
            print
            print '\hline \hline'
            print '\\end{tabular}\n\\end{center}\n\\caption{\\label{tab:}}\n\\end{table}'
            print
    if options.cooltag and minrun<1e10:
        from InDetBeamSpotExample.COOLUtils import *
        sqliteFile= options.cooltag+'.db'
        folderHandle = openBeamSpotDbFile(sqliteFile,True)

        if not options.newave:
            print '\nWriting average to COOL SQLite file %s (folder tag = %s) ...' % (sqliteFile,options.cooltag)
            writeBeamSpotEntry(folderHandle,tag=options.cooltag,
                               status=0,
                               posX=ave[calc.varList.index('posX')],
                               posY=ave[calc.varList.index('posY')],
                               posZ=ave[calc.varList.index('posZ')],
                               sigmaX=ave[calc.varList.index('sigmaX')],
                               sigmaY=ave[calc.varList.index('sigmaY')],
                               sigmaZ=ave[calc.varList.index('sigmaZ')],
                               tiltX=ave[calc.varList.index('tiltX')],
                               tiltY=ave[calc.varList.index('tiltY')],
                               sigmaXY=ave[calc.varList.index('sigmaXY')],
                               posXErr=err[calc.varList.index('posX')],
                               posYErr=err[calc.varList.index('posY')],
                               posZErr=err[calc.varList.index('posZ')],
                               sigmaXErr=err[calc.varList.index('sigmaX')],
                               sigmaYErr=err[calc.varList.index('sigmaY')],
                               sigmaZErr=err[calc.varList.index('sigmaZ')],
                               tiltXErr=err[calc.varList.index('tiltX')],
                               tiltYErr=err[calc.varList.index('tiltY')],
                               sigmaXYErr=err[calc.varList.index('sigmaXY')])
            
        else:
            print '\nWriting average and LB info to COOL SQLite file %s (folder tag = %s) ...' % (sqliteFile,"nominal")
            writeBeamSpotEntry(folderHandle,tag="nominal",
                               runMin=int(minrun),runMax=int(maxrun),
                               status=0,
                               posX=ave[calc.varList.index('posX')],
                               posY=ave[calc.varList.index('posY')],
                               posZ=ave[calc.varList.index('posZ')],
                               sigmaX=ave[calc.varList.index('sigmaX')],
                               sigmaY=ave[calc.varList.index('sigmaY')],
                               sigmaZ=ave[calc.varList.index('sigmaZ')],
                               tiltX=ave[calc.varList.index('tiltX')],
                               tiltY=ave[calc.varList.index('tiltY')],
                               sigmaXY=ave[calc.varList.index('sigmaXY')],
                               posXErr=err[calc.varList.index('posX')],
                               posYErr=err[calc.varList.index('posY')],
                               posZErr=err[calc.varList.index('posZ')],
                               sigmaXErr=err[calc.varList.index('sigmaX')],
                               sigmaYErr=err[calc.varList.index('sigmaY')],
                               sigmaZErr=err[calc.varList.index('sigmaZ')],
                               tiltXErr=err[calc.varList.index('tiltX')],
                               tiltYErr=err[calc.varList.index('tiltY')],
                               sigmaXYErr=err[calc.varList.index('sigmaXY')])

            print 'Copying beam spot data from',options.ntname
            for b in nt:
                if options.lbMin and options.lbMin>b.lbStart:
                    continue
                if options.lbMax and options.lbMax<b.lbEnd+1:
                    continue
                try:
                    runEndInt = b.runEnd
                except AttributeError:
                    runEndInt = b.run

                if b.status in nt.statusList:
                    writeBeamSpotEntry(folderHandle,tag='nominal',
                                       runMin=b.run, 
                                       runMax=runEndInt,
                                       lbMin=b.lbStart, 
                                       lbMax=b.lbEnd,    # Beware - lbMax is inclusive
                                       status=b.status,
                                       posX=b.posX, posY=b.posY, posZ=b.posZ,
                                       sigmaX=b.sigmaX, sigmaY=b.sigmaY, sigmaZ=b.sigmaZ,
                                       tiltX=b.tiltX, tiltY=b.tiltY,
                                       sigmaXY=b.rhoXY*b.sigmaX*b.sigmaY,
                                       posXErr=b.posXErr, posYErr=b.posYErr, posZErr=b.posZErr,
                                       sigmaXErr=b.sigmaXErr, sigmaYErr=b.sigmaYErr, sigmaZErr=b.sigmaZErr,
                                       tiltXErr=b.tiltXErr, tiltYErr=b.tiltYErr,
                                       sigmaXYErr=sqrt( (b.sigmaX*b.sigmaX) * (b.sigmaY*b.sigmaY) * (b.rhoXYErr*b.rhoXYErr) +(b.sigmaX*b.sigmaX) * (b.sigmaYErr*b.sigmaYErr) * (b.rhoXY*b.rhoXY) + (b.sigmaXErr*b.sigmaXErr) * (b.sigmaY*b.sigmaY) * (b.rhoXY*b.rhoXY) ) )

    cmdOk = True

#
# Different histograms of beam spot variables
#
if (cmd=='hist' or cmd=='histvspileup' or cmd=='pull') and len(args)==2:
    var = args[1]
    nt = getNt()
    plots = Plots(nt)
    plots.saveAsList = options.output.split(',')
    if options.canvas:
        plots.singleCanvasSize = options.canvas
        plots.allCanvasSize = options.canvas
        plots.allCanvasDivs = (options.xdivs,options.ydivs)
    if ',' in var:
        plots.whatList = var.split(',')
        plots.genPlot('all',cmd)
    else:
        plots.genPlot(var,cmd)
    cmdOk = True


#
# Plot of beam spot variables vs LB or time
#
if cmd=='plot' and len(args)==2:
    var = args[1]
    nt = getNt()
    plots = Plots(nt)
    plots.saveAsList = options.output.split(',')
    if options.canvas:
        plots.singleCanvasSize = options.canvas
        plots.allCanvasSize = options.canvas
        plots.allCanvasDivs = (options.xdivs,options.ydivs)
    if options.perbcid:
        plotType = 'perbcid'
    elif options.vsbunchpos:
        plotType = 'vsBunchPos'
    else:
        plotType = 'plot'
    if ',' in var:
        plots.whatList = var.split(',')
        plots.genPlot('all',plotType)
    else:
        plots.genPlot(var,plotType)
    cmdOk = True


#
# Summary history of beam spot variables
#
if cmd=='summary' and len(args)==1:
    nt = getNt()
    if not options.msize:
        options.msize = 0.5
    plots = Plots(nt)
    plots.saveAsList = options.output.split(',')
    plots.allCanvasSize = 'landscape'

    labels=[]
    if "COOL-Current" not in options.ntname:
        plots.allCanvasDivs = (4,3)
        plots.whatList = ['posX','posY','posZ','k','sigmaX','sigmaY','sigmaZ','nValid','tiltX','tiltY','rhoXY']
        datasetinfo=options.ntname.replace("-DB_BEAMSPOT","").split('.')
        labels=["%s.%s.%s.%s" % (datasetinfo[0],datasetinfo[1],datasetinfo[2],datasetinfo[3])]
        labels+=["Generated on %s, at %s (%s)" % (time.strftime("%x"), time.strftime("%X"), time.strftime("%Z"))]
    else:
        plots.allCanvasDivs = (3,3)
        plots.whatList = ['posX','posY','posZ','sigmaX','sigmaY','sigmaZ','tiltX','tiltY','rhoXY']

    # add some labels to the empty pad

    if options.perbcid:
        plots.genPlot('all','perbcid',labels=labels)
    elif options.vsbunchpos:
        plots.whatList.remove('nValid')
        plots.genPlot('all','vsBunchPos',labels=labels)
    else:
        plots.genPlot('all','plot',labels=labels)
    
    cmdOk = True


#
# Plot of beam spot variables vs LB or time
#
if cmd=='ascii' and len(args)==2:
    time0 = 1262304000    # unix time for Jan 1, 2010, 00:00:00 CET
    ascii = open(args[1],'w')
    ascii.write('# Beam spot data file\n')
    ascii.write('# Generated %s by %s on host %s using command:\n' % (time.asctime(),getUserName(),getHostName()))
    ascii.write('# %s\n' % qcmd)
    nt = getNt()
    for b in nt:
        ascii.write('1  ')                               # IP number
        ascii.write('%s  ' % str(b.fill))
        ascii.write('%s  ' % str(b.timeStart - time0 + int((b.timeEnd-b.timeStart)/2)))   # Time in sec since Jan 1, 2010
        ascii.write('%s  ' % str(b.posX))
        ascii.write('%s  ' % str(b.posXErr))
        ascii.write('%s  ' % str(b.posY))
        ascii.write('%s  ' % str(b.posYErr))
        ascii.write('%s  ' % str(b.posZ))
        ascii.write('%s  ' % str(b.posZErr))
        ascii.write('0 0 0 0 0 0  ')                     # No data from raw Gauss fit w/o subtracting vtx-resolution
        ascii.write('%s  ' % str(b.sigmaX))
        ascii.write('%s  ' % str(b.sigmaXErr))
        ascii.write('%s  ' % str(b.sigmaY))
        ascii.write('%s  ' % str(b.sigmaYErr))
        ascii.write('%s  ' % str(b.sigmaZ))
        ascii.write('%s  ' % str(b.sigmaZErr))
        ascii.write('0 0 0\n')                           # No RMS info
    ascii.close()
    cmdOk = True


if cmd=='scan' and len(args)==1:

    if not options.msize:
        options.msize = 0.5

    if options.timeaxis:
        xtype = 'time'
    elif options.separation:
        xtype = 'sep'
    else:
        xtype = 'lb'

    # Plot summary plots for full scan
    nt = getNt()

    plots = Plots(nt)
    plots.allCanvasSize = 'landscape'
    plots.allCanvasDivs = (3,3)
    plots.whatList = ['posX','posY','posZ','sigmaX','sigmaY','sigmaZ','tiltX','tiltY','rhoXY']
    plots.saveAsList = ['%s-scanfull-%s-all.gif' %(options.name,xtype),
                        '%s-scanfull-%s-all.eps' %(options.name,xtype),]

    if options.separation and options.overlayScans:
        plots.genPlot('all','plotscan')
    else:
        plots.genPlot('all','plot')

    plots.whatList.append('k')

    # Individual plot for full scan
    plots.singleCanvasSize = 'wide'        
    for var in plots.whatList:
        plots.saveAsList = ['%s-scanfull-%s-%s.gif' %(options.name,xtype,var),
                            '%s-scanfull-%s-%s.eps' %(options.name,xtype,var) ]
        if options.separation and options.overlayScans:
            graphs = plots.genPlot(var,'plotscan')
        else:
            graphs = plots.genPlot(var,'plot')

    
    # Plots for each scan range
    if options.scans:
        scans = [(int(s.split('-')[0]), int(s.split('-')[1])) for s in options.scans.split(',')]
    else:
        scans = []

    options.overlayScans = False
    for (lbMin, lbMax) in scans:
    
        nt = getNt()
        nt.lbMin = lbMin
        nt.lbMax = lbMax

        # Summary plot for each scan range
        plots = Plots(nt)
        plots.allCanvasSize = 'landscape'
        plots.allCanvasDivs = (3,3)
        plots.whatList = ['posX','posY','posZ','sigmaX','sigmaY','sigmaZ','tiltX','tiltY','rhoXY']
        plots.saveAsList = ['%s-scan%s-%s-%s-all.gif' %(options.name,lbMin,lbMax,xtype),
                            '%s-scan%s-%s-%s-all.eps' %(options.name,lbMin,lbMax,xtype) ]
        plots.genPlot('all','plot')

        plots.whatList.append('k')

        # Individual plot for each scan range
        plots.singleCanvasSize = 'default'        
        for var in plots.whatList:
            plots.saveAsList = ['%s-scan%s-%s-%s-%s.gif' %(options.name,lbMin,lbMax,xtype,var),
                                '%s-scan%s-%s-%s-%s.eps' %(options.name,lbMin,lbMax,xtype,var) ]
            graphs = plots.genPlot(var,'plot')    
            
    cmdOk = True


if not cmdOk:
    print 'ERROR: Illegal command or number of arguments'
    sys.exit(1)
