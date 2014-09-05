#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Tae Min Hong, Univ. of Pennsylvania (2011, 2012)
#
# Makes xmon ntuples from COOL db or TRP ntuples.
#
# See https://cern.ch/x for the ntuple structure & storage.
#
# History:
#   2012/04/18 Update for 2012
#   2011/01/11 Based on exampleCost.py
#
# Code strcture:
#   Parse command line args
#   def InsertRun()
#   def FindFillMap()
#   def RunAtlRunQuery()
#   def GetGRL()
#   __main__
#
# Examples:
"""
python makeRoot.py --time-beg="2010-11-01:00:00:00" --time-end="2010-11-07:00:00:00"
python makeRoot.py --run 156682 --lb-beg=450 --lb-end=514    # L1 during Proton-Proton
python makeRoot.py --run 170482 --lb-beg=1   --lb-end=5      # L1+HLT during Heavy Ion
python makeRoot.py                                           # Checks last hour
"""

print 'Hello world makeRoot.py'
import sys
cmd = ' '.join(sys.argv)
print cmd

# Parse command-line arguments
import optparse
preamble='usage: %prog [options]'
p = optparse.OptionParser(usage=preamble, add_help_option=False, version='%prog')

queryXML='queryHandler.xml'

# Arg't-required options
p.add_option('-d','--dir',          dest='dir',         type='string', default='.',         help='output directory                   [default=%default]')
p.add_option('-o','--root',         dest='root',        type='string', default='test.root', help='output ROOT file                   [default=%default]')
p.add_option('-r','--run',          dest='run',         type='string', default=[],          help='run no: e.g., 170482, last2        [default=%default]')
p.add_option('-f','--fill',         dest='fill',        type='string', default=[] ,         help='fill no: e.g., 1546, last5         [default=%default]')
p.add_option('',  '--lb-beg',       dest='lbbeg',       type='int',    default=1,           help='LB start                           [default=%default]')
p.add_option('',  '--lb-end',       dest='lbend',       type='int',    default=9999,        help='LB end                             [default=%default]')
p.add_option('',  '--trp-path',     dest='trppath',     type='string', default='',          help='TRP ntuple path                    [default=%default]')
p.add_option('-w','--window',       dest='window',      type='int',    default=24,          help='time window wrt now (in hours)     [default=%default]')
p.add_option('',  '--time-end',     dest='timeend',     type='string', default='',          help='time end, 2011-02-23:09:09:02      [default=%default]')
p.add_option('',  '--time-beg',     dest='timebeg',     type='string', default='',          help='time beg, 2011-01-23:09:09:02      [default=%default]')
p.add_option('',  '--queryXML',     dest='queryXML',    type='string', default=queryXML,    help='do not rerun AtlRunQuery, use      [default=%default]')
p.add_option('',  '--queryDir',     dest='queryDir',    type='string', default='data',      help='base directory for queryHandler.xml[default=%default]')

# True/False options
p.add_option('-?','--usage',        dest='help',        action='store_true', default=False, help='show this message and exit         [default=%default]')
p.add_option('-h','--help',         dest='help',        action='store_true', default=False, help='show this message and exit         [default=%default]')
p.add_option('-n','--do-not-run',   dest='do_not_run',  action='store_true', default=False, help='exit after printing run list       [default=%default]')
p.add_option('-u','--update',       dest='update',      action='store_true', default=False, help='for xmon, uses lastrunRoot.sh      [default=%default]')
p.add_option('-s','--silent',       dest='options',     action='store_const', const=-2,     help='silent (only error)                [default=%default]')
p.add_option('-q','--quiet',        dest='options',     action='store_const', const=-1,     help='quiet (only warning & error)       [default=%default]')
p.add_option('-v','--verbose',      dest='options',     action='store_const', const=1,      help='verbose (debug)                    [default=%default]')
p.add_option('-c','--cool',         dest='cool',        action='store_true', default=False, help='COOL --> ROOT file                 [default=%default]')
p.add_option('-t','--trp',          dest='trp',         action='store_true', default=False, help='TRP --> ROOT file                  [default=%default]')
p.add_option('-b','--dbtype',       dest='dbtype',      type='string',       default='',    help='Same as --cool --trp               [default=%default]')
p.add_option('-l','--lastnruns',    dest='lastnruns',   type='string',       default='20',  help='Check last N runs in the inventory [default=%default]')
p.add_option('',  '--fast',         dest='fast',        action="store_true", default=False, help="for TRP: one sample per lumi block for (~12x faster)")
p.add_option('',  '--stable',       dest='stable',      action='store_true', default=False, help='require "stable beams"             [default=%default]')
p.add_option('',  '--ready',        dest='ready',       action='store_true', default=False, help='require "readyforphysics=1"        [default=%default]')
p.add_option('',  '--querySkip',    dest='querySkip',   action='store_true', default=False, help='skip AtlRunQuery.py                [default=%default]')

# opts is from above, args are the arguments without a flag
(opts, args) = p.parse_args()

# Help
if opts.help:
    p.print_help()
    sys.exit(1)

# COOL or TRP?
if opts.dbtype=='cool': opts.cool = True
if opts.dbtype=='trp':  opts.trp  = True
if opts.cool: opts.dbtype='cool'
if opts.trp:  opts.dbtype='trp'
if (opts.cool and opts.trp) or not (opts.cool or opts.trp):
    print 'You must specify either --cool or --trp'
    sys.exit(1)

print 'Using %s database to process run=%s' % ('COOL' if opts.cool else 'TRP', opts.run)

#
# Import packages after parsing options
#

import os
import shutil
import logging
logging.basicConfig(level=logging.INFO)
log = logging.getLogger('makeRoot')
print 'Loaded standard packages'

from TrigCostPython import TrigCostAnalysis ; print 'Loaded special TrigCostPython.TrigCostAnalysis package'
from TrigCostPython import TrigCostTRP      ; print 'Loaded special TrigCostPython.TrigCostTRP package'
from TrigCostPython import TrigCostCool     ; print 'Loaded special TrigCostPython.TrigCostCool package'
from TrigCostPython import TrigCostXML      ; print 'Loaded special TrigCostPython.TrigCostXML package'
from TrigCostPython import TrigCostRoot     ; print 'Loaded special TrigCostPython.TrigCostRoot package'
from CoolConvUtilities import AtlCoolTool   ; print 'Loaded special CoolConvUtilities.AtlCoolTool package'


#	#------------------------------------------------------------------------------
#	def ReadFillTxt( name = 'fill.txt' ):
#	    file = open(name, 'r')
#	    fillList = []
#	    for line in file:
#	        if not 'INFO:TrigCostCool:' in line:
#	            continue
#	        line    = line.split('INFO:TrigCostCool:')[1].rstrip()
#	        fields  = line.split(',')
#	        fill    = fields[0].split('fill=')[1].replace(' ','')
#	        beg     = fields[1].split('--')[0].replace(' ','')
#	        end     = fields[1].split('--')[1].replace(' ','')
#	        begTime = fields[2].split('--')[0]
#	        endTime = fields[2].split('--')[1]
#	        lhc     = fields[3].split('lhc='  )[1].replace(' ','').replace('"','')
#	        beam    = fields[4].split('beam=' )[1].replace(' ','').replace('"','')
#	        ebeam   = fields[5].split('ebeam=')[1].replace(' ','')
#	        data = {}
#	        data['fill']     =fill
#	        data['beg']      =beg
#	        data['end']      =end
#	        data['begTime']  =begTime
#	        data['endTime']  =endTime
#	        data['lhc']      =lhc
#	        data['beam']     =beam
#	        data['ebeam']    =ebeam
#	        fillList.append(data)
#
#	    file.close()
#	    return fillList


#	#------------------------------------------------------------------------------
#	def ReadRunTxt( name = 'run.txt' ):
#	    file = open(name, 'r')
#	    runList = []
#	    for line in file:
#	        if not 'INFO:TrigCostCool:' in line:
#	            continue
#	        line    = line.split('INFO:TrigCostCool:')[1].rstrip()
#	        fields  = line.split(',')
#	        run     = fields[0].split('run=')[1].replace(' ','')
#	        beg     = fields[1].split('--')[0].replace(' ','')
#	        end     = fields[1].split('--')[1].replace(' ','')
#	        begTime = fields[2].split('--')[0]
#	        endTime = fields[2].split('--')[1]
#	        data = {}
#	        data['run']      =run
#	        data['beg']      =beg
#	        data['end']      =end
#	        data['begTime']  =begTime
#	        data['endTime']  =endTime
#	        runList.append(data)
#
#	    file.close()
#	    return runList


#------------------------------------------------------------------------------
def RunAtlRunQuery(runlist,stable=False,ready=False,queryXML='queryHandler.xml',queryDir='data'):

    import subprocess

    # Command-line arguments
    stablebeams  = 'and lhc stablebeams true' if stable else ''
    readyphysics = '--readyforphysics \"1\"'  if ready  else ''
    extraargs    = 'verbose nodef nohtml noroot xmlfile'
    runquery     = ''
    for i, rundata in enumerate(runlist):
        if i>0: runquery += ','
        runquery += str(rundata['run'])

    # Execute
    atlCom = 'AtlRunQuery.py \'find run '+runquery+' '+stablebeams+' // '+extraargs+' dirlist:'+queryXML+'\' '+readyphysics
    log.info( atlCom )
    p = subprocess.Popen(atlCom, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout

    # Check for failures
    fail = False
    for line in p.readlines():
        if len(line) == 0: continue
        if opts.options > -1: print line,
        if line[0:9] == 'Traceback': fail=True

    # Try again (once more)
    if fail:
        log.error('AtlRunQuery Failed!!! Try again...')
        p = subprocess.Popen(atlCom, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout

    # Check that xml exists
    queryFile = '/'.join([queryDir, queryXML])
    if not os.access(queryFile, os.R_OK):
        log.error("can't open %s" % queryXML)
        sys.exit()

    return


#------------------------------------------------------------------------------
# runlist --> 'Good runs list'
#
def GetGRL(runlist,stable=False,ready=False,queryXML='queryHandler.xml',queryDir='data',querySkip=False):

    from xml.etree.ElementTree import ElementTree, Element, SubElement

    grl = []

    # Use AtlRunQuery
    if not querySkip:
        RunAtlRunQuery(runlist,stable,ready,queryXML,queryDir)

    # Parse xml -- stolen from AtlDataSumQuery.py
    queryFile = '/'.join([queryDir, queryXML])
    tree = ElementTree()
    tree.parse(queryFile)
    if tree.getroot().tag != 'LumiRangeCollection':
        log.error("can't find 'LumiRangeCollection' tag in file=%s" % queryXML)
        sys.exit()

    for lbc in tree.findall('NamedLumiRange/LumiBlockCollection'):
        runnum = int(lbc.findtext('Run', '-1'))
        if runnum < 0 :
            log.error("can't find 'Run' tag in file %s" % queryXML)
            sys.exit()

        for lbr in lbc.findall('LBRange'):
            lbstart = int(lbr.get('Start', '-1'))
            lbend = int(lbr.get('End', '-1'))

            if lbstart < 0 or lbend < 0:
                log.error("can't find LBRange attributes for run=%d in file=%s" % (runnum, queryXML))
                sys.exit()

            # Find this run in our original list
            thisrun = None
            for i, rundata in enumerate(runlist):
                if rundata['run'] == runnum:
                    thisrun = rundata
                    thisrun['lbbeg'] = lbstart
                    thisrun['lbend'] = lbend
                    runlist.pop(i)

            # Append
            if thisrun:
                grl.append(thisrun)

                if opts.options > -1:
                    StartTime = thisrun['StartTime']
                    EndTime = thisrun['EndTime']
                    beg_ = ACT.time.ctime(StartTime/1.0E9)
                    end_ = ACT.time.ctime(EndTime/1.0E9)
                    log.info('Grl append: run=%d lbbeg=%d lbend=%d time=%s--%s' % (runnum,lbstart,lbend,beg_,end_))

    return grl

#------------------------------------------------------------------------------
def InsertRun( run, lbbeg=0, lbend=0 ):
    data = {}
    data['run'] = int( run )
    data['lbbeg'] = lbbeg
    data['lbend'] = lbend
    data['StartTime'] = 0
    data['EndTime'] = 0
    return data


#------------------------------------------------------------------------------
# Find this 'run' in a 'fill'
#
def FindFillMap( runstart, runend, fillList ):

    fillMap = []
    for fill in fillList:

        fillnum   = fill['fill']
        fillstart = fill['StartTime']
        fillend   = fill['EndTime']

        # Start of run must have started inside the fill
        if fillstart <= runstart and runstart <= fillend:

            # Run is completely inside fill
            if runend <= fillend:
                fillMap.append( fillnum )
                break

            fillMap.append( fillnum )

    return fillMap


#------------------------------------------------------------------------------
#
#
# Main
#
#

print 'Starting main'

# Command-line inputs
ratecoll= None
lbset   = None
config  = None
options = []
if opts.options==-2: options.append("silent")
if opts.options==-1: options.append("quiet")
if opts.options== 1: options.append("debug")
if opts.fast:        options.append("fast")


# Read-in fill & run lists
fillList    = []
runList     = []
goodrunList = []

# Retrieve run list from queryXML for --update
if len(opts.run)==0 or opts.update:
#       fillList = ReadFillTxt()
#       runList  = ReadRunTxt()
#       fillList = TrigCostCool.GetFillList(opts.window, opts.timebeg, opts.timeend, opts.options>0)
    runList  = TrigCostCool.GetRunList(opts.window, opts.timebeg, opts.timeend, opts.options>0)
    if len(runList)>0:
        goodrunList = GetGRL(runList, opts.stable, opts.ready, opts.queryXML, opts.queryDir, opts.querySkip)
else:
    myList = opts.run.replace(' ','').split(',')
    for runnum in myList:
        if len(runList) == 0:
            goodrunList.append( InsertRun(runnum,opts.lbbeg,opts.lbend) )
            continue

        for run in runList:
            if runnum == run['run']:
                goodrunList.append( run )
                myList.pop()

# For xmon, special saving location
user = os.environ['USER']
if opts.update: #user in ['xmon','apache']

    # TODO Remove hard-coded path
#    opts.dir = '/afs/cern.ch/work/x/xmon/public/root/live'
    lastnruns = -20
    try:
        lastnruns = -1*int(opts.lastnruns)
    except:
        pass

    # Check list against existing xmon ROOT inventory
    dir = os.path.join(opts.dir, '../')
    inventory= []
    for fname in os.listdir(dir):
        try:
            this_run    = fname.split('_')[0]
            this_dbtype = fname.split('_')[1]
            if this_dbtype == opts.dbtype+'.root':
                inventory.append(this_run)
        except:
            pass

    # Look for first run not in the inventory
    inventory.sort()
    i_not_in_inventory = 0
    for i, run in enumerate(goodrunList):
        runnum = str(run['run'])
        if not runnum in inventory:
            print runnum, 'not in list'
            if i_not_in_inventory == 0:
                i_not_in_inventory = i

            continue

    log.info('--------------------------------------------------------------------------------')
    log.info('Will remake for inventory %s' % dir)
    log.info('\n'+'\n'.join([('%s'%i) for i in goodrunList[:i+1]]))
    log.info('--------------------------------------------------------------------------------')

    if opts.do_not_run:
        sys.exit()

# Check if output dir exists
log.info('Output directory is :\n%s' % opts.dir)
if not os.path.exists( opts.dir ):
    log.error('Output directory "%s" does not exist, please specify --dir' % opts.dir )
    sys.exit(1)

# Exit?
if opts.do_not_run:
    log.info('Exiting due to -n / --do-not-run option')
    sys.exit(0)

# Loop over runs to make ntuples
rootlist = []
for i, runMap in enumerate(goodrunList):

    # Setup
    run   = runMap['run']
    lbbeg = runMap['lbbeg']
    lbend = runMap['lbend']
    start = runMap['StartTime']
    end   = runMap['EndTime']
    elapse= (end-start)/1e9/60.

    # Nice info
    fillMap = FindFillMap( start, end, fillList )
    log.info('')
    log.info('################################################################################')
    log.info('#')
    log.info('# %d%s run out of %d' % (i+1,'st' if i==0 else 'nd' if i==1 else 'th',len(goodrunList)))
    log.info('# fill=%s run=%d lb=%d,%-4d elapse=%.1f minutes' %
             (fillMap,run,lbbeg,lbend,elapse))
    log.info('# time=%s--%s' % (AtlCoolTool.time.ctime(start/1e9), AtlCoolTool.time.ctime(end/1e9)))
    log.info('#')
    log.info('################################################################################')

    # Make file
    rootname = ''
    if   opts.trp : rootname = '%s/%d_trp.root'  % (opts.dir, run)
    elif opts.cool: rootname = '%s/%d_cool.root' % (opts.dir, run)
    else:           rootname = '%s/%d_cool.root' % (opts.dir, run)
    rootlist.append(rootname)

    # Get luminosity information
    lbset = TrigCostCool.GetLumiblocks(run, lbbeg, lbend, options)

    # Get trigger information
    ratecoll = None
    if   opts.trp : ratecoll = TrigCostTRP .ReadTRP (run, lbbeg, lbend, options, opts.trppath)
    elif opts.cool: ratecoll = TrigCostCool.GetRates(run, lbbeg, lbend, options)

    # Make xmon ROOT file
    TrigCostRoot.MakeRoot(rootname, ratecoll, lbset, options, opts.update)

#	    # Stdout summary
#	    ratecoll.Print(options)
#	    lbset.Print(options)

# Clean up
TrigCostCool.CloseDB()

# Now done in updateRoot.sh
#	# --force
#	if user=='xmon' and opts.update:
#	    tgtdir = '/afs/cern.ch/work/x/xmon/public/root'
#	    for rootfile in rootlist:
#	        if not os.path.isfile(rootfile):
#	            log.info('%s does not exist, skipping' % rootfile)
#	            continue
#
#	        log.info('Moving %s -> %s' % (rootfile, tgtdir))
#	        shutil.move(rootfile, tgtdir)

# Make html readable
print
print 'That is all!'

#
# Archived code
#
#eof
