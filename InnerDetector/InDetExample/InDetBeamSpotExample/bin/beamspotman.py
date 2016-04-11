#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
beamspotman is a command line utility to do typical beam spot related tasks.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: beamspotman.py 717336 2016-01-12 13:11:29Z amorley $'
__usage__   = '''%prog [options] command [args ...]

Commands are:

show RUNNR                              For given run, show data on CAF and beam spot jobs
run  RUNNR TAG                          Run standard beam spot reconstruction job
runCmd DSNAME TASKNAME CMD              Repeat command for matching tasks (use %(DSNAME)s and
                                        %(TASKNAME)s in the command string CMD to specify the
                                        dataset and task names, if needed)
runMon  RUNNR TAG                       Run standard beam spot monitoring job
runMonJobs [MAXTIME]                    Run monitoring jobs for jobs completed at most MAXTIME ago
postproc                                Default postprocessing for any task that needs it
postproc DSNAME TASKNAME [WHATLIST]     Postprocessing for a selected set of tasks
upload DBFILE                           Upload SQLite file into COOL (independent of task)
upload DSNAME TASKNAME                  Upload result of beam spot determination into COOL
dq2get DSNAME TASKNAME                  Retrieve task data from grid job (must have set up grid env.)
queryT0 DSNAME TASKNAME                 Query Tier-0 database about a task
backup DIR                              Backup directory DIR (may contain wildcards) to castor
archive DSNAME TASKNAME                 Archive task data (deletes files after copying to castor,
                                        sets on-disk status to ARCHIVED)
lsbackup                                List contents of backup directory on castor
reproc TEMPLATE DSNAME TASKNAME INPUTDIR Run reprocessing task consisting of several jobs split into 5 LBs
                                        over the files in INPUTDIR
runaod TEMPLATE DSNAME TASKNAME INPUTDIR Run over AOD, splitting jobs into sets of N LBs (similar
                                        to reproc command, except for variable params)
resubmit DSNAME TASKNAME QUEUE          Rerun jobs of a specific task on specifc batch queue
dqflag DBFILE                           Upload DQ SQLite file into COOL (independent of task)
dqflag DSNAME TASKNAME                  Upload result of beam spot DQ flag determination into COOL
runBCID  RUNNR TAG                      Run standard beam spot BCID job
runBCIDJobs [MAXTIME]                   Run BCID jobs for jobs completed at most MAXTIME ago
mctag STATUS POSX POSY POSZ             Create an sqlite file containing a MC tag with the 
      SIGMAX SIGMAY SIGMAZ              parameters given (and zero errors).  A la beamSpot_set.py.
      [TILTX TILTY SIGMAXY]  

                                        
'''

# TODO: additional commands
# - authorize USERID
# - deauthorize USERID

proddir = '/afs/cern.ch/user/a/atlidbs/jobs'
produserfile = '/private/produsers.dat'
prodcoolpasswdfile = '/private/coolinfo.dat'
proddqcoolpasswdfile = '/private/cooldqinfo.dat'
tier0dbinfofile = '/private/t0dbinfo.dat'
beamspottag = ''
#castorbackuppath = '/castor/cern.ch/atlas/atlascerngroupdisk/phys-beamspot/jobs/backup'
#castorarchivepath = '/castor/cern.ch/atlas/atlascerngroupdisk/phys-beamspot/jobs/archive'
castorbackuppath = '/eos/atlas/atlascerngroupdisk/phys-beamspot/jobs/backup'
castorarchivepath = '/eos/atlas/atlascerngroupdisk/phys-beamspot/jobs/archive'

import sys, os, stat,commands
import re
import time
from InDetBeamSpotExample.TaskManager import *
from InDetBeamSpotExample.PostProcessing import doPostProcessing
from InDetBeamSpotExample import BeamSpotPostProcessing
from InDetBeamSpotExample import COOLUtils
from InDetBeamSpotExample import DiskUtils

from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
parser.add_option('-t', '--beamspottag', dest='beamspottag', default=beamspottag, help='beam spot tag')
parser.add_option('-c', '--castorpath', dest='castorpath', default='/castor/cern.ch/grid/atlas/tzero/prod1/perm', help='castor path (excluding project and stream name)')
parser.add_option('-e', '--eospath', dest='eospath', default='/eos/atlas/atlastier0/rucio', help='eos path (excluding project and stream name)')
parser.add_option('-p', '--project', dest='project', default='data10_7TeV', help='project name')
parser.add_option('-s', '--stream', dest='stream', default='expres_express', help='stream name')
parser.add_option('-f', '--filter', dest='filter', default='.*\.ESD\..*', help='regular expression to filter input files')
parser.add_option('-d', '--dbconn', dest='dbconn', default='', help='task manager database connection string (default: check TASKDB, otherwise use sqlite_file:taskdata.db)')
parser.add_option('', '--proddir', dest='proddir', default=proddir, help='production directory (default: %s' % proddir)
parser.add_option('', '--destdbname', dest='destdbname', default='CONDBR2', help='destination database instance name (default: CONDBR2)')
parser.add_option('', '--srcdbname', dest='srcdbname', default='BEAMSPOT', help='source database instance name (default: BEAMSPOT)')
parser.add_option('', '--srctag', dest='srctag', default='nominal', help='source tag (default: nominal)')
parser.add_option('-r', '--runjoboptions', dest='runjoboptions', default='InDetBeamSpotExample/VertexTemplate.py', help='template to run beam spot jobs')
parser.add_option('', '--runtaskname', dest='runtaskname', default='VTX', help='task name')
parser.add_option('-m', '--monjoboptions', dest='monjoboptions', default='InDetBeamSpotExample/MonitoringTemplate.py', help='template to run monitoring jobs')
parser.add_option('', '--bcidjoboptions', dest='bcidjoboptions', default='InDetBeamSpotExample/VertexTemplate.py', help='template to run BCID jobs')
parser.add_option('', '--montaskname', dest='montaskname', default='MON', help='task name')
parser.add_option('', '--bcidtaskname', dest='bcidtaskname', default='BCID', help='BCID task name')
parser.add_option('-g', '--griduser', dest='griduser', default='user10.JuergBeringer', help='grid user name prefix (e.g. user10.JuergBeringer)')
parser.add_option('-b', '--batch', dest='batch', action='store_true', default=False, help='batch mode - never ask for confirmation')
parser.add_option('-n', '--nowildcards', dest='nowildcards', action='store_true', default=False, help='do not add wildcards when looking up dataset and task names')
parser.add_option('-x', '--excludeiftask', dest='excludeiftask', default='', help='Exclude running cmd with runCmd if such task exists already')
parser.add_option('', '--excludeds', dest='excludeds', default='', help='Exclude running cmd with runCmd for dataset containing a certain string')
parser.add_option('', '--expertmode', dest='expertmode', action='store_true', default=False, help='expert mode (BE VERY CAREFUL)')
parser.add_option('', '--ignoremode', dest='ignoremode', default='', help='ignore update protection mode (needs expert pwd)')
parser.add_option('', '--archive', dest='archive', action='store_true', default=False, help='archive, ie delete data after backup')
parser.add_option('', '--incremental', dest='incremental', action='store_true', default=False, help='incremental backup')
parser.add_option('', '--lbfilemap', dest='lbfilemap', default='', help='text file with mapping between filename and lumi blocks')
parser.add_option('', '--lbperjob', dest='lbperjob', type='int', default=5, help='number of luminosity blocks per job (default: 5)')
parser.add_option('', '--params', dest='params', default='', help='job option parameters to pass to job option template')
parser.add_option('-z', '--postprocsteps', dest='postprocsteps', default='JobPostProcessing', help='Task-level postprocessing steps (Default: JobPostProcessing)')
parser.add_option('', '--srcdqtag', dest='srcdqtag', default='nominal', help='source DQ tag (default: nominal)')
parser.add_option('', '--dqtag', dest='dqtag', default='HEAD', help='beam spot DQ tag')
parser.add_option('', '--destdqdbname', dest='destdqdbname', default='CONDBR2', help='DQ destination database instance name (default: CONDBR2)')
parser.add_option('', '--srcdqdbname', dest='srcdqdbname', default='IDBSDQ', help='DQ source database instance name (default: IDBSDQT)')
parser.add_option('', '--dslist', dest='dslist', default='', help='Exclude running cmd with runCmd if the dataset is not in this file')
parser.add_option('', '--nominbias', dest='nominbias', action='store_true', default=False, help='overwrite MinBias_physics in DSNAME with express_express')
parser.add_option('', '--test', dest='testonly', action='store_true', default=False, help='for runaod, show only options and input files')
parser.add_option('', '--resultsondisk', dest='resultsondisk', action='store_true', default=False, help='Leave the results on disk when archiving')
parser.add_option('', '--removedups', dest='removedups', action='store_true', default=False, help='Remove duplicate rerty files of form root.N, keeping the last, for reporc command')
parser.add_option('', '--dpdinput', dest='dpdinput', action='store_true', default=False, help='Run over DPD for runaod')
parser.add_option('', '--pLbFile', dest='pseudoLbFile', default=None, help='File for pseudo LB info from scan')
parser.add_option('-l', '--filelist', dest='filelist', default=None, help='Explicit list of files for reproc command')
parser.add_option('', '--prefix', dest='prefix', default='', help='Prefix for reading files from mass storage (Default: determine from filename (`\'\') ')
parser.add_option('', '--rl', dest='runMin', type='int', default=None, help='Minimum run number for mctag (inclusive)')
parser.add_option('', '--ru', dest='runMax', type='int', default=None, help='Maximum run number for mctag (inclusive)')
parser.add_option('', '--rucio', dest='rucio', action='store_true', default=False, help='rucio directory structure')
parser.add_option('', '--noCheckAcqFlag', dest='noCheckAcqFlag', action='store_true', default=False, help='Don\'t check acqFlag when submitting VdM jobs')
parser.add_option('', '--mon', dest='mon', action='store_true', default=False, help='mon directory structure')


(options,args) = parser.parse_args()
if len(args) < 1:
    parser.error('wrong number of command line arguments')
cmd = args[0]

# General error checking (skipped in expert mode to allow testing)
if not options.expertmode:
    if commands.getoutput('pwd') != options.proddir:
        sys.exit('ERROR: You must run this command in the production directory %s' % options.proddir)
    if not os.path.exists(os.environ.get('HOME')+produserfile):
        sys.exit('ERROR: Authorization file unreadable or does not exists')
    if not commands.getoutput('grep `whoami` %s' % os.environ.get('HOME')+produserfile):
        sys.exit('ERROR: You are not authorized to run this command (user name must be listed in produser file)')
else:
    if commands.getoutput('pwd') != options.proddir:
        print 'WARNING: You are not running in the production directory %s' % options.proddir


#
# Utilities
#
def getFullEosPath(run):
    eosPath = '/'.join([options.eospath,options.project,options.stream])
    return os.path.normpath('/'.join([eosPath,'%08i' % int(run)])) + '/'

def getFullCastorPath(run):
    """Return full castor path given a run number."""
    if run>240000:
        return getFullEosPath(run)
    castorPath = '/'.join([options.castorpath,options.project,options.stream])
    return os.path.normpath('/'.join([castorPath,'%08i' % int(run)])) + '/'   # Run numbers are now 8 digits in castor paths
    #return '/'.join([castorPath,'%07i' % int(run)])   # For now, run numbers are 7 digits in castor paths


def getT0DbConnection():
    try:
        connstring = open(os.environ.get('HOME')+tier0dbinfofile,'r').read().strip()
    except:
        sys.exit('ERROR: Unable to read connection information for Tier-0 database')
    dbtype, dbname = connstring.split(':',1)
    if dbtype!='oracle':
        sys.exit('ERROR: Invalid T0 connection string')
    import cx_Oracle
    try:
        oracle = cx_Oracle.connect(dbname)
    except:
        print 'ERROR: First connection attempt to Tier-0 Oracle database failed; will retry in 10s ...'
        try:
            time.sleep(10)
            oracle = cx_Oracle.connect(dbname)
        except Exception,e:
            print e
            sys.exit('ERROR: Unable to connect to Tier-0 Oracle database')
    if not oracle:
        sys.exit('ERROR: Unable to connect to Tier-0 Oracle database (invalid cx_Oracle connection)')
    return oracle


#
# Upload any SQLite file to COOL (independent of task, w/o book keeping)
#
if cmd=='upload' and len(args)==2:
    dbfile = args[1]
    if not options.beamspottag:
        sys.exit('ERROR: No beam spot tag specified')
    try:
        passwd = open(os.environ.get('HOME')+prodcoolpasswdfile,'r').read().strip()
    except:
        sys.exit('ERROR: Unable to determine COOL upload password')
    print '\nBeam spot file:   ',dbfile
    print 'Uploading to tag: ',options.beamspottag
    os.system('dumpBeamSpot.py -d %s -t %s %s' % (options.srcdbname,options.srctag,dbfile))

    if options.ignoremode:
        ignoremode = '--ignoremode %s' % options.ignoremode
    else:
        ignoremode = ''
    if options.batch:
        batchmode = '--batch'
    else:
        batchmode = ''
    print
    stat = os.system('/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --nomail %s %s --folder /Indet/Beampos --tag %s --retag %s --destdb %s %s %s ATLAS_COOLWRITE ATLAS_COOLOFL_INDET_W %s' % (batchmode,ignoremode,options.srctag,options.beamspottag,options.destdbname,dbfile,options.srcdbname,passwd))
    if stat:
        print "\n\nERROR: UPLOADING TO COOL FAILED - PLEASE CHECK CAREFULLY!\n\n"
        sys.exit(1)
    sys.exit(0)


#
# Run beam spot reconstruction jobs
#
if cmd=='run' and len(args)==3:
    run = args[1]
    tag = args[2]
    c = getFullCastorPath(run)
    datasets = []
    pattern = re.compile(options.filter+tag+'.*')
    for p in DiskUtils.filelist(c):
        f = p.split('/')[-1]
        if pattern.search(f):
            datasets.append(f)
    if len(datasets)!=1:
        print 'ERROR: %i datasets found - use TAG to uniquely identify dataset' % len(datasets)
        sys.exit(1)

    dsname = '.'.join(datasets[0].split('.')[:3])
    #os.system('runJobs.py -n0 -c %s %s %s.%s %s' % (options.runjoboptions,dsname,options.runtaskname,tag,'/'.join([c,datasets[0]])))
    #os.system('runJobs.py -n0 -c -p "LumiRange=2" %s %s %s.%s %s' % (options.runjoboptions,dsname,options.runtaskname+'-LR2',tag,'/'.join([c,datasets[0]])))
    os.system('runJobs.py -n0 -c -f \'%s\' -p "LumiRange=5" %s %s %s.%s %s' % (options.filter,options.runjoboptions,dsname,options.runtaskname+'-LR5',tag,'/'.join([c,datasets[0]])))
    sys.exit(0)


#
# Run beam spot monitoring job
#
if cmd=='runMon' and len(args)==3:
    run = args[1]
    tag = args[2]

    datasets = []
    c=''
    if options.filelist == None:
        c = getFullCastorPath(run)

        pattern = re.compile(options.filter+tag+'.*')
        for p in DiskUtils.filelist(c):
            f = p.split('/')[-1]
            if pattern.search(f) and ('.TMP.log' not in f):
                datasets.append(f)
        if len(datasets)!=1:
            print 'ERROR: %i datasets found (if ambiguous, use full TAG to uniquely identify dataset)' % len(datasets)
            sys.exit(1)
        dsname = '.'.join(datasets[0].split('.')[:3])
    else:
        if options.eospath=='': 
          datasets.append(options.filelist.rstrip())
        else :
          datasets.append('root://eosatlas/' + options.filelist.rstrip() ) 
        dsname=options.project+'.'+run+'.'+options.stream

    if not options.beamspottag:
        sys.exit('ERROR: No beam spot tag specified')
    # NOTE: The command below may be executed via a cron job, so we need set STAGE_SVCCLASS
    #       explicitly in all cases, since it may not be inherited from the environment.
    if 'ESD' in options.filter:
        # NOTE: We pass along the filter setting, but currently we can do --lbperjob only for ESD since for
        #       other data sets we have only the merged files.
        os.system('runJobs.py --lbperjob 10 -c -f \'%s\' -p "cmdjobpreprocessing=\'export STAGE_SVCCLASS=atlcal\', useBeamSpot=True, beamspottag=\'%s\'" %s %s %s.%s %s/' % (options.filter,options.beamspottag,options.monjoboptions,dsname,options.montaskname,tag,'/'.join([c,datasets[0]])))
    elif options.filelist != None:
        lbinfoinfiles=True
        for line in open(options.filelist,'r'):
            if "lb" not in line: 
                lbinfoinfiles=False
                break
        lboptions='--lbperjob 10' if lbinfoinfiles else '-n 10'
        cmd = 'runJobs.py %s -f \'%s\' -q atlasb1_long -p "useBeamSpot=True, beamspottag=\'%s\', tracksAlreadyOnBeamLine=True" %s %s %s.%s %s' % (lboptions,options.filter,options.beamspottag,options.monjoboptions,dsname,options.montaskname,tag,datasets[0])
        print cmd
        os.system(cmd)
    else:
        cmd = 'runJobs.py --lbperjob 10 -c -f \'%s\' -p "cmdjobpreprocessing=\'export STAGE_SVCCLASS=atlcal\', useBeamSpot=True, beamspottag=\'%s\'" %s %s %s.%s %s/' % (options.filter,options.beamspottag,options.monjoboptions,dsname,options.montaskname,tag,'/'.join([c,datasets[0]]))
        os.system(cmd)
        
    sys.exit(0)


#
# Backup to castor
#
if cmd=='backup' and len(args)==2:
    if options.archive and options.incremental:
        sys.exit('ERROR: Cannot do incremental archiving')
    tmplist = glob.glob(args[1]+'/')
    dirlist = []
    for dslash in tmplist:
        d = dslash[:-1]   # Remove trailing /
        if options.incremental:
            baklog = d+'/backup.log'
            if os.path.exists(baklog):
                cmd = 'find %s  -maxdepth 1 -newer %s' % (d,baklog)
                (status,output) = commands.getstatusoutput(cmd)
                status = status >> 8
                if status:
                    sys.exit("ERROR: Error executing %s" % cmd)
                if output:
                    dirlist.append(d)
            else:
                dirlist.append(d)
            pass
        else:
            dirlist.append(d)
    print '\nFound %i directories for backup:\n' % len(dirlist)
    for d in dirlist:
        print '  %s' % d
    if options.archive:
        print '\n****************************************************************'
        print 'WARNING: ARCHIVING - DIRECTORIES WILL BE DELETED AFTER BACKUP!!!'
        print '****************************************************************'
    if not options.batch:
        a = raw_input('\nARE YOU SURE [n] ? ')
        if a!='y':
            sys.exit('ERROR: Aborted by user')
    for d in dirlist:
        tmpdir = '/tmp'
        if options.archive:
            outname = d.replace('/','-')+time.strftime('-%G_%m_%d.tar.gz')
            backuppath = castorarchivepath
        else:
            outname = d.replace('/','-')+'.tar.gz'
            backuppath = castorbackuppath

        print '\nBacking up  %s  -->  %s/%s ...\n' % (d,backuppath,outname)

        status = os.system('tar czf %s/%s %s' % (tmpdir,outname,d)) >> 8
        if status:
            sys.exit('\nERROR: Unable to create local tar file %s/%s' % (tmpdir,outname))

        status = os.system('xrdcp -f %s/%s root://eosatlas/%s/%s' % (tmpdir,outname,backuppath,outname)) >> 8

        if status:
            # Continue to try other files if one failed to upload to castor
            print '\nERROR: Unable to copy file to CASTOR to %s/%s' % (backuppath,outname)            
            continue

        os.system('rm %s/%s' % (tmpdir,outname))
        status = os.system('echo "`date`   %s/%s" >> %s/backup.log' % (backuppath,outname,d)) >> 8
        if status:
            sys.exit('\nERROR: Could not update backup log file')

        if options.archive:
            print '\nWARNING: DELETION OF SOURCE FILES NOT YET IMPLEMENTED\n'
    sys.exit(0)


#
# List backup directory on castor
#
if cmd=='lsbackup' and len(args)==1:
    backuppath = castorarchivepath if options.archive else castorbackuppath
    backuppath = os.path.normpath(backuppath)+'/'
    print '\nCASTOR directory %s:\n' % backuppath
    #os.system('nsls -l %s' % backuppath)
    #os.system('xrd castoratlas ls %s' % backuppath)
    print DiskUtils.ls(backuppath, longls=True)
    print
    sys.exit(0)


#
# Open task manager (used by all following commands, at the very least through subcommands)
#
try:
    taskman = TaskManager(options.dbconn)
except:
    print 'ERROR: Unable to access task manager database %s' % options.dbconn
    sys.exit(1)


#
# Show available data sets
#
if cmd=='show' and len(args)==2:
    run = args[1]
    c = getFullCastorPath(run)
    print '\nCASTOR base path:   ', options.castorpath
    print   'Project tag:        ', options.project
    print   'Stream:             ', options.stream
    print   'Full path:          ', c
    print '\nFiles in CASTOR:\n'
    pattern = re.compile(options.filter)
    for f in DiskUtils.ls(c, longls=True).split('\n'):
        # for EOS use 'eos ls -l' 
        if pattern.search(f):
            (access,nfiles,user,group,size,modmonth,modyear,modtime,name) = f.split()
            print '%-60s   %s files, last modified %s %s %s' % (name,nfiles,modmonth,modyear,modtime)

    print '\nBeam spot tasks:\n'
    for t in taskman.taskIterDict(qual=["where DSNAME like '%%%s%%' order by UPDATED" % run]):
        print '%-40s %-25s  %-2s job(s), last update %s' % (t['DSNAME'],t['TASKNAME'],t['NJOBS'],time.ctime(t['UPDATED']))
    print '\n'
    sys.exit(0)


#
# Postprocessing: for all tasks requiring it, or for a selected set of tasks
#
if cmd=='postproc' and len(args)==1:
    for t in taskman.taskIterDict(qual=['where STATUS > %i and STATUS <= %i order by UPDATED' % (TaskManager.StatusCodes['SUBMITTED'],TaskManager.StatusCodes['POSTPROCESSING'])]):
        doPostProcessing(taskman,t,t['TASKPOSTPROCSTEPS'].split(),BeamSpotPostProcessing)
    sys.exit(0)

if cmd=='postproc' and len(args)>=3 and len(args)<=4:
    whatList = args[3].split(',') if len(args)>3 else None
    if whatList:
        print 'Executing postprocessing tasks:',whatList
    else:
        print 'Executing postprocessing tasks as specified in task database'
    print
    try:
        taskList = getFullTaskNames(taskman,args[1],args[2],confirmWithUser=not options.batch,addWildCards=not options.nowildcards)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
    for taskName in taskList:
        t = taskman.getTaskDict(taskName[0],taskName[1])
        if whatList:
            doPostProcessing(taskman,t,whatList,BeamSpotPostProcessing,True)
        else:
            doPostProcessing(taskman,t,t['TASKPOSTPROCSTEPS'].split(),BeamSpotPostProcessing,True)
    sys.exit(0)


#
# Upload beam spot data for given task to COOL
#
if cmd=='upload' and len(args)==3:
    try:
        [(dsname,task)] = getFullTaskNames(taskman,args[1],args[2],requireSingleTask=True,confirmWithUser=not options.batch,addWildCards=not options.nowildcards)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
    if not options.beamspottag:
        sys.exit('ERROR: No beam spot tag specified')

    dbfile = glob.glob('%s/%s/*-beamspot.db' % (dsname,task))
    if len(dbfile)!=1:
        print 'ERROR: Missing or ambiguous input COOL file:',dbfile
        sys.exit()

    try:
        passwd = open(os.environ.get('HOME')+prodcoolpasswdfile,'r').read().strip()
    except:
        sys.exit('ERROR: Unable to determine COOL upload password')

    print '\nData set:         ',dsname
    print 'Beam spot file:   ',dbfile[0]
    print 'Uploading to tag: ',options.beamspottag
    os.system('dumpBeamSpot.py -d %s -t %s %s' % (options.srcdbname,options.srctag,dbfile[0]))

    if options.ignoremode:
        ignoremode = '--ignoremode %s' % options.ignoremode
    else:
        ignoremode = ''
    if options.batch:
        batchmode = '--batch'
    else:
        batchmode = ''

    stat = os.system('/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --nomail  %s %s --folder /Indet/Beampos --tag %s --retag %s --destdb %s %s %s ATLAS_COOLWRITE ATLAS_COOLOFL_INDET_W %s' % (batchmode,ignoremode,options.srctag,options.beamspottag,options.destdbname,dbfile[0],options.srcdbname,passwd))

    if stat:
        print "\n\nERROR: UPLOADING TO COOL FAILED - PLEASE CHECK CAREFULLY!\n\n"
        sys.exit(1)

    # Record upload information, both in task DB (field COOLTAGS) and in upload flag file
    uploadflag = dbfile[0]+'.uploaded'
    stat = os.system('echo "`date`   %s" >> %s' % (options.beamspottag,uploadflag) )
    if stat:
        print "ERROR: Uploading was successful, but unable to set upload flag", uploadflag 

    cooltags = options.beamspottag

    # If uploading to the beamspot tag linked to Current BLK alias and Next BLK alias aslo exists (not '')
    # and points to different tag then AtlCoolMerge will upload to the Next beamspot tag too.  Hence record
    # that in COOLTAGS too.  Mimik logic in AtlCoolMergeLib.py
    
    nextbeamspot = ''
    try:
        nextbeamspot = COOLUtils.resolveNextBeamSpotFolder()
    except:
        nextbeamspot = ''
    if nextbeamspot == options.beamspottag:
        nextbeamspot = ''
    if not ('UPD' in options.beamspottag and 'UPD' in nextbeamspot):
        nextbeamspot = ''
    if nextbeamspot != '':
        print 'Additionally uploading to Next tag: ',nextbeamspot
        cooltags = appendUnique(cooltags, nextbeamspot)

    # Upload tag(s) to TaskManager
    t = taskman.getTaskDict(dsname,task)
    taskman.setValue(dsname,task,'COOLTAGS',appendUnique(t['COOLTAGS'],cooltags))
        
    sys.exit(0)


#
# Retrieve task data from grid job
#
if cmd=='dq2get' and len(args)==3:
    try:
        [(dsname,task)] = getFullTaskNames(taskman,args[1],args[2],requireSingleTask=True,addWildCards=not options.nowildcards)
        del taskman
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
    dir = '/'.join([dsname,task])
    griddsname = '%s.%s-%s' % (options.griduser,dsname,task)
    path = '/'.join([dir,griddsname])
    if os.path.exists(path):
        print 'ERROR: Path exists already:',path
        sys.exit(1)
    print 'Extracing into %s/%s ...' % (dir,griddsname)
    stat = os.system('cd %s; dq2-get -T 6,6 %s' % (dir,griddsname))
    if stat:
        print "ERROR: Problem occurred while extracting data set - task status not changed"

    statfile = glob.glob('%s/000/*.status.SUBMITTED' % (dir))
    if len(statfile)!=1:
        print "ERROR: Unable to uniquely identfying status - giving up. Candidate status files are:"
        print statfile
        sys.exit(1)
    os.system('rm %s' % statfile[0])
    basename = statfile[0][:-17]
    os.system('touch %s.exit.0' % basename)
    os.system('touch %s.COMPLETED' % basename)
    print "\nSuccessfully downloaded data set",griddsname
    os.system('du -hs %s' % path)
    print
    sys.exit(0)


#
# Query T0 task status
#
if cmd=='queryT0' and len(args)==3:
    try:
        [(dsname,task)] = getFullTaskNames(taskman,args[1],args[2],requireSingleTask=True,addWildCards=not options.nowildcards)
        del taskman
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
    tags = task.split('.')[-1]
    # FIXME: change the following to automatically determine from the input files of the DB_BEAMSPOT job?
    if 'ESD' in options.filter:
        t0TaskName = '%s.recon.ESD.%s.beamspotproc.task' % (dsname,tags)
    else:
        t0TaskName = '%s.recon.AOD.%s.beamspotproc.task' % (dsname,tags)
    print 'Querying Tier-0 database for task',t0TaskName,'...'
    oracle = getT0DbConnection()
    cur = oracle.cursor()
    try:
        #sql = str("SELECT status FROM etask WHERE taskname='%s' AND creationtime>sysdate-10" % t0TaskName)  # Oracle doesn't want unicode
        sql = str("SELECT status FROM tasks WHERE taskname='%s' AND tasktype='beamspotproc'" % t0TaskName)  # Oracle doesn't want unicode
        cur.execute(sql)
        r = cur.fetchall()
    except Exception,e:
        print e
        sys.exit('ERROR: Unable to retrieve status of task %s' % t0TaskName)
    if len(r)==0:
        sys.exit('ERROR: No such task found: %s' % t0TaskName)
    if len(r)>1:
        sys.exit('ERROR: %i tasks found - unable to determine task status' % len(r))
    status = r[0][0]
    print '\nTask status = %s\n' % status
    if status=='FINISHED' or  status=='TRUNCATED':
        sys.exit(0)
    else:
        sys.exit(2)


#
# Run command over set of matching tasks
#
if cmd=='runCmd' and len(args)==4:
    dssel = args[1]
    tasksel = args[2]
    cmd = args[3]
    if options.nowildcards:
        qual = ["where DSNAME like '%s' and TASKNAME like '%s'" % (dssel,tasksel)]
    else:
        qual = ["where DSNAME like '%%%s%%' and TASKNAME like '%%%s%%'" % (dssel,tasksel)]
    if options.beamspottag:
        qual.append("and COOLTAGS like '%%%s%%'" % options.beamspottag)
    #qual.append("order by RUNNR desc")
    qual.append("order by RUNNR")
    print 'Running command\n'
    print '   ',cmd % ({'DSNAME': 'DSNAME', 'TASKNAME': 'TASKNAME', 'RUNNR': 'RUNNR', 'FULLDSNAME': 'FULLDSNAME'})
    print '\nover the following datasets / tasks:\n'
    print '    %-10s   %-40s   %s' % ('Run','Dataset','Task')
    print '    %s' % (74*'-')

    taskList = []
    for t in taskman.taskIterDict('*',qual):
        dsname = t['DSNAME']

        # Deal with express vs minBias in 900 GeV runs
        if options.nominbias and dsname.find('physics_MinBias') :
            print 'Warning: changing physics_MinBias in dsname to express_express'
            dsname = dsname.replace('physics_MinBias','express_express')
            t['DSNAME'] = dsname

        runnr = t['RUNNR']
        taskname = t['TASKNAME']
        print '    %-10s   %-40s   %s'% (runnr,dsname,taskname)

        if options.excludeiftask:
            if options.nowildcards:
                # n = taskman.getNTasks(['where DSNAME =',DbParam(dsname),'and TASKNAME like',DbParam(options.excludeiftask)])
                n = taskman.getNTasks(["where DSNAME ='%s' and TASKNAME like '%s'" % (dsname,options.excludeiftask)])
            else:
                n = taskman.getNTasks(["where DSNAME ='%s' and TASKNAME like '%%%s%%'" % (dsname,options.excludeiftask)])
            if n!=0:
                print '    ==> SKIPPING - %i matching tasks found' % n
                continue

        if options.excludeds:
            excludeList = options.excludeds.split(',')
            for s in excludeList:
                if s in dsname:
                    print '    ==> SKIPPING dataset'
                    continue
            

        if options.dslist:
            stat, out = commands.getstatusoutput('grep -c %s %s' % (dsname, options.dslist))
            if stat==2: # Missing file etc 
                print out
                sys.exit(1)

            try:
                if int(out)==0:
                    print '    ==> SKIPPING - dataset %s not found in %s' % (dsname, options.dslist)
                    continue
            except ValueError: # Some other error
                print out
                sys.exit(1)

            del stat, out
            # # Find full dtasetname from file
            stat, out = commands.getstatusoutput('grep %s %s' % (dsname, options.dslist))
            if stat==2: # Missing file etc 
                print out
                sys.exit(1)

            t['FULLDSNAME'] = out.strip()

        taskList.append(t)

    if not taskList:
        print '\nNo jobs need to be run.\n'
        sys.exit(0)

    print '\n%i datasets / tasks selected.\n' % len(taskList)
    if not options.batch:
        a = raw_input('\nARE YOU SURE [n] ? ')
        if a!='y':
            sys.exit('ERROR: Running of monitoring tasks aborted by user')
        print
    for t in taskList:
        fullcmd = cmd % t
        print '\nExecuting:  ',fullcmd,' ...'
        sys.stdout.flush()
        os.system(fullcmd)

    print
    sys.exit(0)


#
# Run monitoring jobs for jobs completed at most MAXTIME ago
#
if cmd=='runMonJobs' and len(args)<3:
    if len(args)==2:
        earliestUpdateTime = time.time()-float(args[1])
    else:
        earliestUpdateTime = 0
    if not options.beamspottag:
        sys.exit('ERROR: No beam spot tag specified')

    print 'Running the following monitoring tasks for tasks of type %s:\n' % options.runtaskname
    print '       %-10s  %s' % ('RUN','MONITORING TASK')
    print '    %s' % (60*'-')

    onDiskCode = TaskManager.OnDiskCodes['ALLONDISK']
    taskList = []
    for t in taskman.taskIterDict('*',["where TASKNAME like '%s%%'" % options.runtaskname,
                                       "and UPDATED >= ", DbParam(earliestUpdateTime),
                                       "and ONDISK = ", DbParam(onDiskCode),
                                       "and COOLTAGS like '%%%s%%'" % options.beamspottag,
                                       "order by RUNNR desc"]):
        dsname = t['DSNAME']
        runnr = t['RUNNR']
        taskName = t['TASKNAME']
        datatag = taskName.split('.')[-1].split('_')[0]
        monTaskName = 'MON.%s.%s' % (taskName,datatag)

        try:
            m = taskman.taskIterDict('*',['where RUNNR =',DbParam(runnr),'and DSNAME =',DbParam(dsname),'and TASKNAME =',DbParam(monTaskName),'order by UPDATED desc']).next()
            print '       %-10s  %s'% (runnr,monTaskName)
        except:
            print '    *  %-10s  %s'% (runnr,'--- no monitoring task found ---')
            taskList.append(t)
            pass
    
    if not taskList:
        print '\nNo jobs need to be run.\n'
        sys.exit(0)

    if not options.batch:
        a = raw_input('\nARE YOU SURE [n] ? ')
        if a!='y':
            sys.exit('ERROR: Running of monitoring tasks aborted by user')
        print
 
    oracle = getT0DbConnection()
    for t in taskList:
        dsname = t['DSNAME']
        runnr = t['RUNNR']
        ptag = dsname.split('.')[0]
        stream = dsname.split('.')[2]
        taskName = t['TASKNAME']
        fulldatatag = taskName.split('.')[-1]
        datatag = fulldatatag.split('_')[0]
        monTaskName = 'MON.%s' % (taskName)

        # Now that beamspot folder tag can change, the tag to be monitored must be the one the parent task uploaded to.
        # If more than one tag (might be case if upload to current/next) then take first (should be same)
        cooltags = t['COOLTAGS']
        if not cooltags: cooltags = ''
        bstag = cooltags.split()[0]

        filter = 'AOD'
        t0dsname = '%s.merge.AOD.%s%%' % (dsname,datatag)  # For running over AOD
        c = getJobConfig('.',dsname,taskName)
        if 'ESD' in c['inputfiles'][0]:
            filter = 'ESD'
            t0dsname = '%s.recon.ESD.%s' % (dsname,datatag)   # For running over ESD
        print '\nRunning monitoring job for run %s:' % runnr

        submitjob=True
        eospath=options.eospath
        # for old runs we would need to check if the dataset had been replicated at Tier-0.  
        # with EOS this is no longer necessary.
        r=[]
        if int(runnr)<240000:
            print '... Querying T0 database for replication of %s' % t0dsname
            cur = oracle.cursor()
            cur.execute("select DATASETNAME,PSTATES from DATASET where DATASETNAME like '%s' and PSTATES like '%%replicate:done%%'" % t0dsname)
            r = cur.fetchall()
            if not r:
                print '    WARNING: input data not yet replicated - please retry later'
                submitjob=False
        else:
            print '... Querying T0 database for completion of merging jobs of %s' % t0dsname
            cur = oracle.cursor()
            origt0TaskName='%s.recon.AOD.%s%%.aodmerge.task' % (dsname,datatag)
            cur.execute("select status from tasks where taskname like '%s' and tasktype='aodmerge'" % origt0TaskName)
            r = cur.fetchall()
            if not r:
                print '    WARNING: can\'t get status of merge job for %s, running on un-merged samples instead' % origt0TaskName
                eospath='/eos/atlas/atlastier0/tzero/prod'
            elif not (r[0][0]=='FINISHED' or r[0][0]=='TRUNCATED'):
                print '    Merge job for taskname %s is not finished yet, has status %s, running on un-merged samples instead.' % (origt0TaskName, r[0][0])
                eospath='/eos/atlas/atlastier0/tzero/prod'
            else:
                print '    Merge job is finished, launching jobs.'
            submitjob=True

        if submitjob:
            if int(runnr)<240000:
                print '   ',r
            print '... Submitting monitoring task'
            cmd = 'beamspotman.py --eospath=%s -p %s -s %s -f \'.*\\.%s\\..*\' -t %s --montaskname %s runMon %i %s' % (eospath,ptag,stream,filter,bstag,'MON.'+taskName,int(runnr),datatag)
            print '    %s' % cmd
            sys.stdout.flush()
            status = os.system(cmd) >> 8   # Convert to standard Unix exit code
            if status:
                print '\nERROR: Job submission returned error - exit code %i\n'

    print
    sys.exit(0)


#
# Archive task to castor
#
if cmd=='archive' and len(args)==3:
    if not options.batch:
        print '\nWARNING: If you confirm below, each of the following datasets will:'
        print '         - be archived to CASTOR'
        if options.resultsondisk:
            print '         - will be marked as RESULTSONDISK in the task database'
            print '         - all except the results files *** WILL BE DELETED ***'
        else:
            print '         - will be marked as ARCHIVED in the task database'
            print '         - all its files *** WILL BE DELETED ***'            
        print
    try:
        taskList = getFullTaskNames(taskman,args[1],args[2],confirmWithUser=not options.batch,addWildCards=not options.nowildcards)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)

    tmpdir = '/tmp'
    backuppath = castorarchivepath
    onDiskCode = TaskManager.OnDiskCodes.get('ALLONDISK',None)
    archivedCode = TaskManager.OnDiskCodes.get('RESULTSONDISK',None) if options.resultsondisk else TaskManager.OnDiskCodes.get('ARCHIVED',None)
    exceptList = ['*dqflags.txt', '*.gif', '*.pdf', '*.config.py*', '*.argdict.gpickle', '*.AveBeamSpot.log', '*.PlotBeamSpotCompareReproc.log', '*.sh', '*.BeamSpotNt.*', '*.BeamSpotGlobalNt.log', '*.status.*', '*.exit.*']
    
    for (dsname,taskname) in taskList:
        t = taskman.getTaskDict(dsname,taskname)


        # Check that task files are still on disk, so we're not overwriting an earlier archive
        if t['ONDISK'] != onDiskCode:
            print 'Skipping task %s / %s status %s (task files must be on disk)' % (dsname,taskname,getKey(TaskManager.OnDiskCodes,t['ONDISK']))
            print
            continue            

        dir = '%s/%s' % (dsname,taskname)
        outname = dir.replace('/','-')+time.strftime('-%G_%m_%d.tar.gz')
        print 'Archiving task %s / %s ...' % (dsname,taskname)
        print '    --> %s/%s ...' % (backuppath,outname)

        # Paranoia check against later catastrophic delete
        if dir=='.' or dir=='*':
            print '\n**** FATAL ERROR: Very dangerous value of task directory found: %s - ABORTING' % dir
            sys.exit(1)

        # If expected directory exists, tar up files, write to CASTOR, mark as archived, and delete
        if os.path.exists(dir):
            status = os.system('tar czf %s/%s %s' % (tmpdir,outname,dir)) >> 8
            if status:
                sys.exit('\n**** ERROR: Unable to create local tar file %s/%s' % (tmpdir,outname))
            status = os.system('xrdcp %s/%s root://eosatlas/%s/%s' % (tmpdir,outname,backuppath,outname)) >> 8
            if status:
                sys.exit('\n**** ERROR: Unable to copy file to CASTOR to %s/%s' % (backuppath,outname))

            os.system('rm %s/%s' % (tmpdir,outname))
            n = taskman.setValue(dsname,taskname,'ONDISK',archivedCode)
            if n!=1:
                sys.exit('\n**** ERROR: Unexpected number of tasks modified: %i instead of 1 (DSNAME=%s,TASKNAME=%s)' % (n,dsname,taskname))

            if options.resultsondisk:
                oscmd = "find %s ! \( -name '%s' \) -type f -exec rm  {} \;" % (dir, "' -or -name '".join(exceptList))
                os.system(oscmd)
            else:
                os.system('rm -rf %s' % dir)
        
        else:
            print '\n**** ERROR: No task directory',dir,'\n'

        print
    sys.exit(0)

#
# Run beam spot resumit failed jobs
# Double check directory structure is appropriate for you
#

if cmd=='resubmit' and len(args)==4: 

    dsname    = args[1]
    taskname  = args[2]
    queue     = args[3]

    # Form bunched jobs
    # Submit bunched jobs
    basepath = os.getcwd()+'/'+dsname+'/'+taskname+'/'
    dircontents = os.listdir( basepath )

    for dir in dircontents:
        if not os.path.isdir(os.path.join(basepath,dir)):
            continue
        print dir
        jobname=dir
        if options.mon: 
           jobname= dsname+'-'+taskname+'-'+dir
        fullpath = os.getcwd()+'/'+dsname+'/'+taskname+'/'+dir

        isRunning = False
        for f in os.listdir(fullpath):
          if re.search('RUNNING', f) or re.search('COMPLETED',f):
            isRunning = True

        if isRunning:
          continue

        for f in os.listdir(fullpath):
          if re.search('.exitstatus.', f):
            os.remove(os.path.join(fullpath, f))
          elif re.search('.exit.', f):
            os.remove(os.path.join(fullpath, f))
          elif re.search('.status.', f):
            os.remove(os.path.join(fullpath, f))
          elif re.search('.log', f):
            os.remove(os.path.join(fullpath, f))
          elif re.search('.py.final.py', f):
            os.remove(os.path.join(fullpath, f))

        #params['lbList'] = '[' + ','.join([str(l) for l in lbs]) + ']'

        jobConfig ={'test':'this' }
        jobConfig['batchqueue'] = queue
        jobConfig['jobname'] = jobname
        jobConfig['jobdir'] = os.getcwd()+'/'+dsname+'/'+taskname+'/'+dir
        jobConfig['logfile']='%(jobdir)s/%(jobname)s.log' % jobConfig
        jobConfig['scriptfile']='%(jobdir)s/%(jobname)s.sh' % jobConfig

        
        batchCmd = 'bsub -q %(batchqueue)s -J %(jobname)s -o %(logfile)s %(scriptfile)s' % jobConfig
        print batchCmd
        os.system(batchCmd)

    print taskman.getStatus(dsname, taskname)
    taskman.setStatus(dsname, taskname, TaskManager.StatusCodes['RUNNING'] )

    sys.exit(0)


#
# Run beam spot reprocessing jobs
# Defines one task with several jobs where the splitting into groups of 5 LBs is done by hand
#
if cmd=='reproc' and len(args)==5: 
    from InDetBeamSpotExample import LSFJobRunner

    jobopts   = args[1]
    dsname    = args[2]
    taskname  = args[3]
    inputdata = args[4]

    lbperjob  = options.lbperjob
    params    = {'LumiRange': lbperjob}
    cmd       = ' '.join(sys.argv)
    pattern   = re.compile(options.filter)
    files     = []

    # Additional job parameters
    for s in options.params.split(', '):
        if s:
            try:
                p = s.split('=',1)
                params[p[0].strip()] = eval(p[1].strip())
            except:
                print '\nERROR parsing user parameter',p,'- parameter will be ignored'

    # Make list of all files (either from explicit list or by 'globbing' dir)
    if options.filelist:
        inputdata = os.path.normpath(inputdata)

        protocol=''
        if options.prefix:
            protocol = options.prefix
        elif inputdata.split('/')[1]=='castor':
            protocol = 'root://castoratlas/'
        elif inputdata.split('/')[1]=='eos':
            protocol = 'root://eosatlas/'
        
        flist = open(options.filelist)
        files = sorted(['%s%s/%s' % (protocol,inputdata,f.strip('\n')) for f in flist if pattern.search(f)])

        if options.rucio:
            # Has extra subdirectories: find unique set of these from filepaths, ignoring final part (i.e. filename).
            # Will also work for single directory but more time consuming.
            #inputdirs = set(['/'+'/'.join(f.split('/')[4:-1])+'/' for f in files])
            inputdirs = set(['/'.join(f.replace(protocol, '').split('/')[:-1])+'/' for f in files])
        else:
            inputdirs = [inputdata]
    else:
        if os.path.isfile(inputdata):
          # read in the file names from the text file
          for line in open(inputdata,'r'):
             files.append('root://eosatlas/'+line.rstrip())
      
        elif inputdata.split('/')[1] in ('castor', 'eos'):
            inputdata = os.path.normpath(inputdata)+'/'
            castorfiles = DiskUtils.filelist(inputdata, prefix=options.prefix if options.prefix else True)
            for f in castorfiles:
                if pattern.search(f):
                    files.append(f)
        else:
            files = sorted(glob.glob(inputdata+'/'+options.filter))

    # Check if files, matching pattern, exist
    if not files: sys.exit('No files existing in directory %s matching "%s"' % (inputdata, options.filter))

    # Remove duplicate with different retry number, keeping latest    
    if options.removedups:
        nWithDup = len(files)
        fileAndExt = {}
        for f in files:
            tokens = f.split('.')
            fname  = '.'.join(tokens[:-1])
            ext    = tokens[-1]
            try:
                fileAndExt[fname].append(ext)
            except KeyError:
                fileAndExt[fname] = [ext]
                
        files = []
        for fname,ext in fileAndExt.items():
            files.append(fname+'.'+ext[-1])

        print 'Removed ', (nWithDup - len(files)), 'duplicates'
    
    # Get file-LB mapping
    lbMap = {}
    if options.lbfilemap:
        lbfilemap = open(options.lbfilemap, 'r')

        for line in lbfilemap:
            fname = line.split(' ')[0]
            lbs   = [int(l) for l in line.split(' ')[1].split(',')]
            
            lbMap[fname] = lbs
    else:
        from InDetBeamSpotExample.ExtractLBFileMap import extract,extractFromFiles
        #lbMap = extract(inputdirs, options.filter, prefix=options.prefix if options.prefix else True)
        lbMap = extractFromFiles( files )

    # Form bunched jobs
    jobFileDict = {}
    jobLBDict   = {}

    # Special case to submit a single job over all files
    if lbperjob == -1:
        jobId = 1
        jobFileDict[jobId] = files
        jobLBDict[jobId] = []

        for f in files:
            try:
                lbs = sorted(lbMap[f.split('/')[-1]])
            except KeyError:
                sys.exit('No mapping for file %s' % f.split('/')[-1])

            jobLBDict[jobId].extend(lbs)
    else:                
        for f in files:
            try:
                lbs = sorted(lbMap[f.split('/')[-1]])
            except KeyError:
                sys.exit('No mapping for file %s' % f.split('/')[-1])
                
            for lbnr in lbs:
                jobId = int((lbnr-1)/lbperjob)

                if not jobId in jobFileDict:
                    jobFileDict[jobId] = [f]
                    jobLBDict[jobId]   = [lbnr]
                else:
                    if not f in jobFileDict[jobId]:
                        jobFileDict[jobId].append(f)
                    jobLBDict[jobId].append(lbnr)
    
    # Submit bunched jobs
    for i in  sorted(jobFileDict.keys()):
        jobnr = i*lbperjob+1  # use first LB number as job number
        files=jobFileDict[i]
        lbs = sorted(set(jobLBDict[i]))

        #params['lbList'] = '[' + ','.join([str(l) for l in lbs]) + ']'
        intlbs = []
        for lbnr in lbs:
            intlbs.append(int(lbnr))
        params['lbList'] = intlbs
        jobname=dsname+'-'+taskname+'-lb%03i' % jobnr

        runner = LSFJobRunner.LSFJobRunner(jobnr=jobnr,
                                           jobdir=os.getcwd()+'/'+dsname+'/'+taskname+'/'+jobname,
                                           jobname=jobname,
                                           inputds='',
                                           inputfiles=files,
                                           joboptionpath=jobopts,
                                           filesperjob=len(files),
                                           batchqueue='atlasb1_long',
                                           addinputtopoolcatalog=True,
                                           taskpostprocsteps='ReprocVertexDefaultProcessing',
                                           #outputfilelist=['dpd.root', 'nt.root', 'monitoring,root', 'beamspot.db'],
                                           autoconfparams='DetDescrVersion',
                                           returnstatuscode=True,
                                           comment=cmd,
                                           **params)

        #runner.showParams()
        try:
            runner.configure()
        except Exception,e:
            print "ERROR: Unable to configure JobRunner job - perhaps same job was already configured / run before?"
            print "DEBUG: Exception =",e
        else:
            taskman.addTask(dsname, taskname, jobopts, runner.getParam('release'), runner.getNJobs(), runner.getParam('taskpostprocsteps'), comment=cmd)
            runner.run()

    sys.exit(0)


#
# Run beam spot reprocessing jobs
# Defines one task with several jobs where the splitting into groups of 5 LBs is done by hand
#
if cmd=='reproc' and len(args)==5: 
    from InDetBeamSpotExample import LSFJobRunner

    jobopts   = args[1]
    dsname    = args[2]
    taskname  = args[3]
    inputdata = args[4]

    lbperjob  = options.lbperjob
    params    = {'LumiRange': lbperjob}
    cmd       = ' '.join(sys.argv)
    pattern   = re.compile(options.filter)
    files     = []

    # Additional job parameters
    for s in options.params.split(', '):
        if s:
            try:
                p = s.split('=',1)
                params[p[0].strip()] = eval(p[1].strip())
            except:
                print '\nERROR parsing user parameter',p,'- parameter will be ignored'

    # Make list of all files (either from explicit list or by 'globbing' dir)
    if options.filelist:
        inputdata = os.path.normpath(inputdata)

        protocol=''
        if options.prefix:
            protocol = options.prefix
        elif inputdata.split('/')[1]=='castor':
            protocol = 'root://castoratlas/'
        elif inputdata.split('/')[1]=='eos':
            protocol = 'root://eosatlas/'
        
        flist = open(options.filelist)
        files = sorted(['%s%s/%s' % (protocol,inputdata,f.strip('\n')) for f in flist if pattern.search(f)])

        if options.rucio:
            # Has extra subdirectories: find unique set of these from filepaths, ignoring final part (i.e. filename).
            # Will also work for single directory but more time consuming.
            #inputdirs = set(['/'+'/'.join(f.split('/')[4:-1])+'/' for f in files])
            inputdirs = set(['/'.join(f.replace(protocol, '').split('/')[:-1])+'/' for f in files])
        else:
            inputdirs = [inputdata]
    else:
        if os.path.isfile(inputdata):
          # read in the file names from the text file
          for line in open(inputdata,'r'):
             files.append('root://eosatlas/'+line.rstrip())
      
        elif inputdata.split('/')[1] in ('castor', 'eos'):
            inputdata = os.path.normpath(inputdata)+'/'
            castorfiles = DiskUtils.filelist(inputdata, prefix=options.prefix if options.prefix else True)
            for f in castorfiles:
                if pattern.search(f):
                    files.append(f)
        else:
            files = sorted(glob.glob(inputdata+'/'+options.filter))

    # Check if files, matching pattern, exist
    if not files: sys.exit('No files existing in directory %s matching "%s"' % (inputdata, options.filter))

    # Remove duplicate with different retry number, keeping latest    
    if options.removedups:
        nWithDup = len(files)
        fileAndExt = {}
        for f in files:
            tokens = f.split('.')
            fname  = '.'.join(tokens[:-1])
            ext    = tokens[-1]
            try:
                fileAndExt[fname].append(ext)
            except KeyError:
                fileAndExt[fname] = [ext]
                
        files = []
        for fname,ext in fileAndExt.items():
            files.append(fname+'.'+ext[-1])

        print 'Removed ', (nWithDup - len(files)), 'duplicates'
    
    # Get file-LB mapping
    lbMap = {}
    if options.lbfilemap:
        lbfilemap = open(options.lbfilemap, 'r')

        for line in lbfilemap:
            fname = line.split(' ')[0]
            lbs   = [int(l) for l in line.split(' ')[1].split(',')]
            
            lbMap[fname] = lbs
    else:
        from InDetBeamSpotExample.ExtractLBFileMap import extract,extractFromFiles
        #lbMap = extract(inputdirs, options.filter, prefix=options.prefix if options.prefix else True)
        lbMap = extractFromFiles( files )

    # Form bunched jobs
    jobFileDict = {}
    jobLBDict   = {}

    # Special case to submit a single job over all files
    if lbperjob == -1:
        jobId = 1
        jobFileDict[jobId] = files
        jobLBDict[jobId] = []

        for f in files:
            try:
                lbs = sorted(lbMap[f.split('/')[-1]])
            except KeyError:
                sys.exit('No mapping for file %s' % f.split('/')[-1])

            jobLBDict[jobId].extend(lbs)
    else:                
        for f in files:
            try:
                lbs = sorted(lbMap[f.split('/')[-1]])
            except KeyError:
                sys.exit('No mapping for file %s' % f.split('/')[-1])
                
            for lbnr in lbs:
                jobId = int((lbnr-1)/lbperjob)

                if not jobId in jobFileDict:
                    jobFileDict[jobId] = [f]
                    jobLBDict[jobId]   = [lbnr]
                else:
                    if not f in jobFileDict[jobId]:
                        jobFileDict[jobId].append(f)
                    jobLBDict[jobId].append(lbnr)
    
    # Submit bunched jobs
    for i in  sorted(jobFileDict.keys()):
        jobnr = i*lbperjob+1  # use first LB number as job number
        files=jobFileDict[i]
        lbs = sorted(set(jobLBDict[i]))

        #params['lbList'] = '[' + ','.join([str(l) for l in lbs]) + ']'
        intlbs = []
        for lbnr in lbs:
            intlbs.append(int(lbnr))
        params['lbList'] = intlbs
        jobname=dsname+'-'+taskname+'-lb%03i' % jobnr

        runner = LSFJobRunner.LSFJobRunner(jobnr=jobnr,
                                           jobdir=os.getcwd()+'/'+dsname+'/'+taskname+'/'+jobname,
                                           jobname=jobname,
                                           inputds='',
                                           inputfiles=files,
                                           joboptionpath=jobopts,
                                           filesperjob=len(files),
                                           batchqueue='2nd',
                                           addinputtopoolcatalog=True,
                                           taskpostprocsteps='ReprocVertexDefaultProcessing',
                                           #outputfilelist=['dpd.root', 'nt.root', 'monitoring,root', 'beamspot.db'],
                                           autoconfparams='DetDescrVersion',
                                           returnstatuscode=True,
                                           comment=cmd,
                                           **params)

        #runner.showParams()
        try:
            runner.configure()
        except Exception,e:
            print "ERROR: Unable to configure JobRunner job - perhaps same job was already configured / run before?"
            print "DEBUG: Exception =",e
        else:
            taskman.addTask(dsname, taskname, jobopts, runner.getParam('release'), runner.getNJobs(), runner.getParam('taskpostprocsteps'), comment=cmd)
            runner.run()

    sys.exit(0)

#
# Run task over AOD, bunching input files to jobs according to meta-data in input
# AOD files.
#


if cmd=='runaod' and len(args)==5:
    from InDetBeamSpotExample import LSFJobRunner

    jobopts   = args[1]
    dsname    = args[2]
    taskname  = args[3]
    inputdata = args[4]
    
    lbperjob  = options.lbperjob
    params    = {'LumiRange': lbperjob}

    # Always fit a single pLb for scans 
    if options.pseudoLbFile:
        params    = {'LumiRange': 1}

    cmd       = ' '.join(sys.argv)
    pattern   = re.compile(options.filter)
    files     = []

    # Additional job parameters
    for s in options.params.split(', '):
        if s:
            try:
                p = s.split('=',1)
                params[p[0].strip()] = eval(p[1].strip())
            except:
                print '\nERROR parsing user parameter',p,'- parameter will be ignored'

    # Make list of all files
    # first, is this a directory?  probably.
    if os.path.isdir(inputdata) or "castor" in inputdata or "eos" in inputdata:
        if inputdata.split('/')[1] in ('castor', 'eos'):
            inputdata = os.path.normpath(inputdata)+'/'
            #files=[]
            castorfiles = DiskUtils.filelist(inputdata, options.prefix if options.prefix else True)
            for f in castorfiles:
                if pattern.search(f):
                    files.append(f)
        elif options.dpdinput:
            files = sorted(glob.glob(inputdata+'/*/*-dpd.root*'))
        else:
            files = sorted(glob.glob(inputdata+'/*'+options.filter+'*'))
    elif os.path.isfile(inputdata):
        # read in the file names from the text file
        for line in open(inputdata,'r'):
            files.append('root://eosatlas/'+line.rstrip())

    # Check if files, matching pattern, exist
    if not files: sys.exit('No files existing in directory %s matching "%s"' % (inputdata, options.filter))
        
    # Get file-LB mapping
    lbMap = {}
    if options.lbfilemap:
        lbfilemap = open(options.lbfilemap, 'r')

        for line in lbfilemap:
            fname = line.split(' ')[0]
            lbs   = [int(l) for l in line.split(' ')[1].split(',')]
            
            lbMap[fname] = lbs
    else:
        from InDetBeamSpotExample.ExtractLBFileMap import extract
        if options.dpdinput:
            lbMap = extract([inputdata], 'dpd')
        else:
            lbMap = extract([inputdata], options.filter, prefix=options.prefix if options.prefix else True)

    # Form bunched jobs
    jobFileDict = {}
    jobLBDict   = {}
    jobParams   = {}
    
    if options.pseudoLbFile:
        # Extract start and end times of real LBs
        from InDetBeamSpotExample.COOLUtils import COOLQuery
        coolQuery = COOLQuery()
        from InDetBeamSpotExample.Utils import getRunFromName        
        lbTimes = coolQuery.getLbTimes( getRunFromName(dsname, None, True) ) 

        # Loop over pseudo LBs
        with open(options.pseudoLbFile) as pLbFile:            
            for line in pLbFile:
                if line[0] == '#': continue
                
                tokens = line.split()                
                plbnr,tstart,tend = int(tokens[0]),int(tokens[1]),int(tokens[2])
                jobId = int(plbnr/lbperjob)

                # if we're passing in a file with acqFlags, and we're checking those flags,
                # then skip any points that don't have acqFlag=1.0
                if not options.noCheckAcqFlag and len(tokens)>=5 and abs(float(tokens[4])-1.)>0.001:
                    print "Point is not stationary -- skipping job %d" % jobId
                    continue

                # Find real LBs covering time period of pseudo LB.  Assumes pLBs in nsec
                rlbs = [lb for (lb,time) in lbTimes.items() if (time[0] - tend/1e9)*(time[1] - tstart/1e9) < 0]

                # Find files containing those real lbs
                filenames = []
                for f in files:
                    try:
                        lbs = sorted(lbMap[f.split('/')[-1]])
                    except KeyError:
                        sys.exit('No mapping for file %s' % f.split('/')[-1])

                    if not sum([lb for lb in lbs if lb in rlbs]): continue

                    filenames.append(f)

                try:
                    jobLBDict[jobId].extend([lb for lb in rlbs if not lb in jobLBDict[jobId]])
                    jobFileDict[jobId].extend([f for f in filenames if not f in jobFileDict[jobId]])                     
                    jobParams[jobId]['lbData'].append(line.strip('\n').strip())
                except KeyError:
                    jobLBDict[jobId] = rlbs
                    jobFileDict[jobId] = filenames
                    jobParams[jobId] = {'lbData' : [line.strip('\n').strip()]}

    else:
        for f in files:
            try:
                lbs = sorted(lbMap[f.split('/')[-1]])
            except KeyError:
                print 'WARNING: No mapping for file %s.  Skipping' % f.split('/')[-1]
                continue
                #sys.exit('No mapping for file %s' % f.split('/')[-1])

            for lbnr in lbs:
                jobId = int((lbnr-1)/lbperjob)

                if not jobId in jobFileDict:
                    jobFileDict[jobId] = [f]
                    jobLBDict[jobId]   = [lbnr]
                else:
                    if not f in jobFileDict[jobId]:
                        jobFileDict[jobId].append(f)
                    jobLBDict[jobId].append(lbnr)

    # Submit bunched jobs
    for i in  sorted(jobFileDict.keys()):
        jobnr = i*lbperjob+1  # use first LB number as job number
        files=jobFileDict[i]
        lbs = sorted(set(jobLBDict[i]))
        
        intlbs = []
        for lbnr in lbs:
            intlbs.append(int(lbnr))
        params['lbList'] = intlbs

        # Allow job-by-job parameters
        try:
            p = jobParams[i]
            for k,v in p.items(): params[k] = v
        except KeyError:
            pass

        jobname=dsname+'-'+taskname+'-lb%03i' % jobnr

        runner = LSFJobRunner.LSFJobRunner(jobnr=jobnr,
                                           jobdir=os.getcwd()+'/'+dsname+'/'+taskname+'/'+jobname,
                                           jobname=jobname,
                                           inputds='',
                                           inputfiles=files,
                                           joboptionpath=jobopts,
                                           filesperjob=len(files),
                                           batchqueue='1nw' if options.pseudoLbFile else 'atlasb1', # run on different queue for VdM scans to avoid cloggin up normal queue # '1nw' '2nd'
                                           addinputtopoolcatalog=True,
                                           taskpostprocsteps=options.postprocsteps,
                                           autoconfparams='DetDescrVersion',
                                           returnstatuscode=True,
                                           comment=cmd,
                                           **params)
        if options.testonly:
            runner.showParams()
        else:
            try:
                runner.configure()
            except Exception,e:
                print "ERROR: Unable to configure JobRunner job - perhaps same job was already configured / run before?"
                print "DEBUG: Exception =",e
            else:
                taskman.addTask(dsname, taskname, jobopts, runner.getParam('release'), runner.getNJobs(), runner.getParam('taskpostprocsteps'), comment=cmd)
                runner.run()
    sys.exit(0)



#
# Upload any DQ sqlite file to COOL (independent of task, w/o book keeping)
#
if cmd=='dqflag' and len(args)==2:
    dbfile = args[1]
    if not options.dqtag:
        sys.exit('ERROR: No beamspot DQ tag specified')
    try:
        passwd = open(os.environ.get('HOME')+proddqcoolpasswdfile,'r').read().strip()
    except:
        sys.exit('ERROR: Unable to determine DQ COOL upload password')

    print '\nBeam spot DQ file:   ',dbfile
    print 'Uploading to tag: ',options.dqtag

    if options.ignoremode:
        ignoremode = '--ignoremode %s' % options.ignoremode
    else:
        ignoremode = ''

    if options.batch:
        batchmode = '--batch'
    else:
        batchmode = ''

    print
    cmd = 'dq_defect_copy_defect_database.py --intag %s --outtag %s "sqlite://;schema=%s;dbname=%s" "oracle://ATLAS_COOLWRITE;schema=ATLAS_COOLOFL_GLOBAL;dbname=%s;"' %(options.srcdqtag, options.dqtag, dbfile[0], options.srcdqdbname, options.destdqdbname) 
    print cmd

    stat = os.system(cmd)

    # Old DQ flags
    #stat = os.system('/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --nomail %s %s --folder /GLOBAL/DETSTATUS/SHIFTOFL --tag %s --retag %s --destdb %s %s %s ATLAS_COOLWRITE ATLAS_COOLOFL_GLOBAL_W %s' % (batchmode,ignoremode,options.srcdqtag,options.dqtag,options.destdqdbname,dbfile,options.srcdqdbname,passwd))

    if stat:
        print "\n\nERROR: UPLOADING DQ FLAG TO COOL FAILED - PLEASE CHECK CAREFULLY!\n\n"
        sys.exit(1)
    sys.exit(0)

#
# Upload beam spot DQ flag for given task to COOL
#
if cmd=='dqflag' and len(args)==3:
    try:
        [(dsname,task)] = getFullTaskNames(taskman,args[1],args[2],requireSingleTask=True,confirmWithUser=not options.batch,addWildCards=not options.nowildcards)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
        
    if not options.dqtag:
        sys.exit('ERROR: No beam spot DQ tag specified')

    dbfile = glob.glob('%s/%s/*-dqflags.db' % (dsname,task))
    if len(dbfile)!=1:
        print 'ERROR: Missing or ambiguous input COOL DQ file:',dbfile
        sys.exit()

    try:
        passwd = open(os.environ.get('HOME')+proddqcoolpasswdfile,'r').read().strip()
    except:
        sys.exit('ERROR: Unable to determine DQ COOL upload password')

    print '\nData set:         ',dsname
    print 'Beam spot DQ file:  ',dbfile[0]
    print 'Uploading to tag:   ',options.dqtag

    if options.ignoremode:
        ignoremode = '--ignoremode %s' % options.ignoremode
    else:
        ignoremode = ''
    if options.batch:
        batchmode = '--batch'
    else:
        batchmode = ''

    cmd = 'dq_defect_copy_defect_database.py --intag %s --outtag %s "sqlite://;schema=%s;dbname=%s" "oracle://ATLAS_COOLWRITE;schema=ATLAS_COOLOFL_GLOBAL;dbname=%s;"' %(options.srcdqtag, options.dqtag, dbfile[0], options.srcdqdbname, options.destdqdbname) 
    print "Running %s" % cmd
    stat = os.system(cmd)

    # Old DQ flags
    #stat = os.system('/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --nomail  %s %s --folder /GLOBAL/DETSTATUS/SHIFTOFL --tag %s --retag %s --destdb %s %s %s ATLAS_COOLWRITE ATLAS_COOLOFL_GLOBAL_W %s' % (batchmode,ignoremode,options.srcdqtag,options.dqtag,options.destdqdbname,dbfile[0],options.srcdqdbname,passwd))
    if stat:
        print "\n\nERROR: UPLOADING DQ FLAG TO COOL FAILED - PLEASE CHECK CAREFULLY!\n\n"
        sys.exit(1)

    # Record upload information in upload flag file
    uploadflag = dbfile[0]+'.uploaded'
    stat = os.system('echo "`date`   %s" >> %s' % (options.dqtag,uploadflag) )
    if stat:
        print "ERROR: Uploading DQ flag was successful, but unable to set upload flag", uploadflag

    sys.exit(0)

#
# Run beam spot BCID job
#
if cmd=='runBCID' and len(args)==3:
    run = args[1]
    tag = args[2]
    c = getFullCastorPath(run)
    datasets = []
    pattern = re.compile(options.filter+tag+'.*')
    for p in DiskUtils.filelist(c):
        f = p.split('/')[-1]
        if pattern.search(f):
            datasets.append(f)

    if len(datasets)!=1:
        print 'ERROR: %i datasets found (if ambiguous, use full TAG to uniquely identify dataset)' % len(datasets)
        sys.exit(1)
    dsname = '.'.join(datasets[0].split('.')[:3])
    # NOTE: The command below may be executed via a cron job, so we need set STAGE_SVCCLASS
    #       explicitly in all cases, since it may not be inherited from the environment.
    if 'ESD' in options.filter:
        # For ESD, run over 10 LBs
        # NOTE: We pass along the filter setting, but currently we can do --lbperjob only for ESD since for
        #       other data sets we have only the merged files.
        os.system('runJobs.py -c -n 0 -f \'%s\' -z \'BCIDDefaultProcessing\' -p "cmdjobpreprocessing=\'export STAGE_SVCCLASS=atlcal\', SeparateByBCID=True, VertexNtuple=False" %s %s %s.%s %s/' % (options.filter,options.bcidjoboptions,dsname,options.bcidtaskname,tag,'/'.join([c,datasets[0]])))
    else:
        # Non-ESD format (most likely merged AOD) - run over 2 files
        os.system('runJobs.py -c -n 0 -f \'%s\' -z \'BCIDDefaultProcessing\' -p "cmdjobpreprocessing=\'export STAGE_SVCCLASS=atlcal\', SeparateByBCID=True, VertexNtuple=False" %s %s %s.%s %s/' % (options.filter,options.bcidjoboptions,dsname,options.bcidtaskname,tag,'/'.join([c,datasets[0]])))

    sys.exit(0)

#
# Run BCID jobs for jobs completed at most MAXTIME ago
#
if cmd=='runBCIDJobs' and len(args)<3:
    if len(args)==2:
        earliestUpdateTime = time.time()-float(args[1])
    else:
        earliestUpdateTime = 0

    if not options.beamspottag:
        sys.exit('ERROR: No beam spot tag specified')
        
    print 'Running the following BCID tasks for tasks of type %s:\n' % options.runtaskname
    print '       %-10s  %s' % ('RUN','BCID TASK')
    print '    %s' % (60*'-')

    taskList = []
    for t in taskman.taskIterDict('*',["where TASKNAME like '%s%%'" % options.runtaskname,
                                       "and UPDATED >= ", DbParam(earliestUpdateTime),
                                       "and COOLTAGS like '%%%s%%'" % options.beamspottag,
                                       "order by RUNNR desc"]):
        dsname = t['DSNAME']
        runnr = t['RUNNR']
        taskName = t['TASKNAME']
        datatag = taskName.split('.')[-1].split('_')[0]
        bcidTaskName = 'BCID.%s.%s' % (taskName,datatag)

        try:
            m = taskman.taskIterDict('*',['where RUNNR =',DbParam(runnr),'and DSNAME =',DbParam(dsname),'and TASKNAME =',DbParam(bcidTaskName),'order by UPDATED desc']).next()
            print '       %-10s  %s'% (runnr,bcidTaskName)
        except:
            print '    *  %-10s  %s'% (runnr,'--- no BCID task found ---')
            taskList.append(t)
            pass
    
    if not taskList:
        print '\nNo jobs need to be run.\n'
        sys.exit(0)

    if not options.batch:
        a = raw_input('\nARE YOU SURE [n] ? ')
        if a!='y':
            sys.exit('ERROR: Running of BCID tasks aborted by user')
        print
 
    oracle = getT0DbConnection()
    for t in taskList:
        dsname = t['DSNAME']
        runnr = t['RUNNR']
        ptag = dsname.split('.')[0]
        stream = dsname.split('.')[2]
        taskName = t['TASKNAME']
        datatag = taskName.split('.')[-1].split('_')[0]
        bcidTaskName = 'BCID.%s.%s' % (taskName,datatag)
        filter = 'AOD'
        t0dsname = '%s.merge.AOD.%s%%' % (dsname,datatag)  # For running over AOD
        c = getJobConfig('.',dsname,taskName)
        if 'ESD' in c['inputfiles'][0]:
            filter = 'ESD'
            t0dsname = '%s.recon.ESD.%s' % (dsname,datatag)   # For running over ESD
        print '\nRunning BCID job for run %s:' % runnr

        print '... Querying T0 database for replication of %s' % t0dsname
        cur = oracle.cursor()
        cur.execute("select DATASETNAME,PSTATES from DATASET where DATASETNAME like '%s' and PSTATES like '%%replicate:done%%'" % t0dsname)
        r = cur.fetchall()
        if not r:
            print '    WARNING: input data not yet replicated - please retry later'
        else:
            print '   ',r
            print '... Submitting BCID task'
            cmd = 'beamspotman.py -p %s -s %s -f \'.*\\.%s\\..*\' --bcidtaskname %s runBCID %i %s' % (ptag,stream,filter,'BCID.'+taskName,int(runnr),datatag)
            print '    %s' % cmd
            sys.stdout.flush()
            status = os.system(cmd) >> 8   # Convert to standard Unix exit code
            if status:
                print '\nERROR: Job submission returned error - exit code %i\n'

    print
    sys.exit(0)
#
#  Create an sqlite file containing a MC (OFLP200) tag a la beamSpot_set.py
#  Need -- before positional agruments to deal with -ve values
#  beamspotman.py -t IndetBeampos-7TeV-PeriodD-SmallWidth-001 mctag -- 0 0.1 1.1 -5 0.045 0.048 63 0.00043 -4e-05 9e-05
#
if cmd=='mctag' and len(args)<12:
    
    from InDetBeamSpotExample.COOLUtils import *

    if not options.beamspottag:
        sys.exit('ERROR: No beam spot tag specified')

    dbfile=options.beamspottag + '.db'
    folderHandle = openBeamSpotDbFile(dbfile, dbName = 'OFLP200', forceNew = True)

    runMin = options.runMin if options.runMin is not None else 0
    runMax = options.runMax if options.runMax is not None else (1 << 31)-1      
    
    writeBeamSpotEntry(folderHandle, tag=options.beamspottag,
                       runMin=runMin, runMax=runMax,
                       status=int(args[1]),
                       posX=float(args[2]), posY=float(args[3]), posZ=float(args[4]),
                       sigmaX=float(args[5]), sigmaY=float(args[6]), sigmaZ=float(args[7]),
                       tiltX=float(args[8]) if len(args)>8 else 0.,
                       tiltY=float(args[9]) if len(args)>9 else 0.,                       
                       sigmaXY=float(args[10]) if len(args)>10 else 0.,
                       posXErr=0., posYErr=0., posZErr=0.,
                       sigmaXErr=0., sigmaYErr=0., sigmaZErr=0.,
                       tiltXErr=0., tiltYErr=0.,
                       sigmaXYErr=0.)

    print '* MC beamspot tag written to db=OFLP200, tag=%s in %s ' %(options.beamspottag, dbfile)
    print '  - AtlCoolConsole.py "sqlite://;schema=' + dbfile + ';dbname=OFLP200"'
    print '* To upload to oracle use:'
    print '  - beamspotman.py --srctag %s -t %s --srcdbname OFLP200 --destdbname OFLP200 upload %s' %(options.beamspottag, options.beamspottag, dbfile)
    print '  - /afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --nomail %s OFLP200 ATLAS_COOLWRITE ATLAS_COOLOFL_INDET_W <passwd>' %(dbfile)
    sys.exit(0)


print 'ERROR: Illegal command or number of arguments'
sys.exit(1)
