#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
taskman is a command line utility to run TaskManager functions.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: taskman.py 739379 2016-04-11 14:52:08Z amorley $'
__usage__   = '''%prog [options] taskdbconn command [args ...]

Commands are:

init                              Create a new database
checkdb                           Check (and fix) sqlite database
checkdup                          Check task database for duplicate entries
debug                             Interactive debugging (EXPERTS ONLY!)
dump [DSNAME TASKNAME]            Dump all or selected tasks in database
show DSNAME [TASKNAME]            Show list of tasks
update                            Update information of ongoing tasks
update DSNAME TASKNAME            Update information of selected tasks
rebuild                           Rebuild missing database entries from job files
rebuild DSNAME TASKNAME           Rebuild selected database entries from job files
import DBCONN                     Import all tasks from database DBCONN
setstatus DSNAME TASKNAME STATUS  Set status of selected tasks to STATUS
setfield DSNAME TASKNAME FIELDNAME VALUE  Set database field to value (experts only!)
delete DSNAME TASKNAME            Delete entry for selected task(s) (task files are NOT removed)
deleteResults DSNAME TASKNAME     Delete postprocessing results for selected set of tasks
deleteTask DSNAME TASKNAME        Delete task files and corresponding TaskManager entry
                                  for a single task (will prompt for confirmation unless
                                  option --batch is used)

Examples:

taskman.py init
taskman.py rebuild .
taskman.py dump
'''

import sys, os, stat, commands
import pprint
from InDetBeamSpotExample.TaskManager import *
from InDetBeamSpotExample.Utils import getRunFromName


from optparse import OptionParser
parser = OptionParser(usage=__usage__, version=__version__)
#parser.add_option('-x', '--xxx', dest='xx', default='myvalue', help='sample option')
parser.add_option('-b', '--batch', dest='batch', action='store_true', default=False, help='batch mode - never ask for confirmation')
parser.add_option('-v', '--verbose', dest='verbose', action='store_true', default=False, help='enable more verbose output for some commands')
parser.add_option('-n', '--nowildcards', dest='nowildcards', action='store_true', default=False, help='do not add wildcards when looking up dataset and task names')
parser.add_option('-d', '--dbconn', dest='dbconn', default='', help='task manager database connection string (default: check TASKDB, otherwise use sqlite_file:taskdata.db)')
parser.add_option('', '--proddir', dest='proddir', default='.', help='production directory (default: "."')
parser.add_option('-p', '--pretty', dest='pretty', action='store_true', default=False, help='try to nicely format output')
parser.add_option('', '--runtaskname', dest='runtaskname', default='CB_BEAMSPOT', help='task name')
(options,args) = parser.parse_args()
if len(args) < 1:
    parser.error('wrong number of command line arguments')
cmd = args[0]
proddir = options.proddir
if not os.path.exists(proddir):
    sys.exit('ERROR: Job directory %s does not exist or is unreadable' % proddir)


#
# Initialize a new task database
#
if cmd=='init' and len(args)==1:
    if not options.batch:
        a = raw_input('\nRECREATING TASK DATABASE SCHEMA - ANY EXISTING DATA WILL BE ERASED!\n\nARE YOU REALLY ABSOLUTEY SURE [n] ? ')
        if a!='y':
            sys.exit('ERROR: Rebuilding aborted by user')
        print
    taskman = TaskManager(options.dbconn,True)
    sys.exit(0)


#
# Check integrity of sqlite database and vacuum, if necessary
#
if cmd=='checkdb' and len(args)==1:
    try:
        (dbtype,dbfile) = options.dbconn.split(':')
    except:
        sys.exit("ERROR: Illegal or empty/default database connection string - must provide explicity SQLite file reference")
    if dbtype != 'sqlite_file':
        sys.exit('ERROR: checkdb is only supported for SQLite databases')
    if not os.path.exists(dbfile):
        sys.exit('ERROR: SQLite file %s does not exist' % dbfile)
    (status,output) = commands.getstatusoutput("sqlite3 %s 'pragma integrity_check;'" % dbfile)
    if status!=0:
        sys.exit('ERROR: Error executing sqlite3 command')
    if output!='ok':
        print 'ERROR: SQLite database file has errors:\n'
        print output
        print
        if not options.batch:
            a = raw_input('Do you want to try VACUUM [n] ? ')
            if a!='y':
                sys.exit('\nERROR: VACUUM not executed - please fix database file manually')
        (status,output) = commands.getstatusoutput("sqlite3 %s 'vacuum;'" % dbfile)
        print output
        if status!=0:
            sys.exit('ERROR: VACUUM failed')
        (status,output) = commands.getstatusoutput("sqlite3 %s 'pragma integrity_check;'" % dbfile)
        if status!=0 or output!='ok':
            print output
            sys.exit('ERROR: Integrity check still failed - please check')
        print 'INFO: SQLite file now passes integrity test (content may still have errors)'
        sys.exit(2)
    sys.exit(0)


#
# Open task manager (used by all following commands)
#
try:
    taskman = TaskManager(options.dbconn)
except:
    print 'ERROR: Unable to access task manager database %s' % options.dbconn
    sys.exit(1)


#
# Check database for duplicate entries and remove duplicates, if necessary
#
if cmd=='checkdup' and len(args)==1:
    #q = ['select dsname,taskname,count(*) from tasks group by dsname,taskname']
    nDuplicates = 0
    for t in taskman.taskIter('dsname,taskname,count(*)', ['group by dsname,taskname']):
        if t[2]>1:
            nDuplicates += 1
            print 'Duplicate task:  %s / %s' % (t[0],t[1])
            for d in taskman.taskIterDict(qual=['where DSNAME =',DbParam(t[0]), 'and TASKNAME =', DbParam(t[1]), 'order by taskid']):
                print '   TASKID =',d['TASKID']
    print nDuplicates,'duplicates found'
    sys.exit(0)


#
# Dump contents of task database
#
if cmd=='dump' and len(args)==1:
    print taskman.getNTasks(),'task(s) found:\n'
    if options.pretty:
        for t in taskman.taskIterDict():
            print '\n\nTask %s  /  %s:\n' % (t['DSNAME'],t['TASKNAME'])
            print pprint.pformat(t)
    else:
        for t in taskman.taskIter():
            print t
    sys.exit(0)

if cmd=='dump' and len(args)==3:
    try:
        taskList = getFullTaskNames(taskman,args[1],args[2],addWildCards=not options.nowildcards)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
    for t in taskList:
        taskEntry = taskman.getTaskDict(t[0],t[1])
        if options.pretty:
            print '\n\nTask %s  /  %s:\n' % (taskEntry['DSNAME'],taskEntry['TASKNAME'])
            print pprint.pformat(taskEntry)
        else:
            print taskEntry
    sys.exit(0)


#
# Show list of tasks
#
if cmd=='show' and (len(args)==2 or len(args)==3):
    dsname = args[1]
    taskname = args[2] if len(args)>2 else ''
    try:
        taskList = getFullTaskNames(taskman,dsname,taskname,addWildCards=not options.nowildcards)
        print
        print "    %-50s  %s" % ('DATASET NAME','TASK NAME')
        print "    %s" % (75*'-')
        for t in taskList:
            print "    %-50s  %s" % (t[0],t[1])  
        print '\n%i task(s) found\n' % len(taskList)
        sys.exit(0)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)


#
# Update task database entries based on task files on disk
#
if cmd=='update' and len(args)==1:
    for t in taskman.taskIterDict('DSNAME,TASKNAME',['where STATUS < %i and ONDISK < %i' %
                                  (TaskManager.StatusCodes['POSTPROCESSING'], TaskManager.OnDiskCodes['ARCHIVED'])]):
        a = TaskAnalyzer(proddir,t['DSNAME'],t['TASKNAME'])
        if a.analyzeFiles():
            print 'Updating task %s/%s' % (t['DSNAME'],t['TASKNAME'])
            a.updateStatus(taskman)
        else:
            taskman.setDiskStatus(t['DSNAME'],t['TASKNAME'],TaskManager.OnDiskCodes['DELETED'])
    sys.exit(0)

if cmd=='update' and len(args)==3:
    try:
        taskList = getFullTaskNames(taskman,args[1],args[2],confirmWithUser=not options.batch,addWildCards=not options.nowildcards)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
    for t in taskList:
        a = TaskAnalyzer(proddir,t[0],t[1])
        if a.analyzeFiles():
            print 'Updating task %s/%s' % (t[0],t[0])
            a.updateStatus(taskman)
        else:
            taskman.setDiskStatus(t[0],t[0],TaskManager.OnDiskCodes['DELETED'])
    sys.exit(0)


#
# Rebuild task database entries based on task files on disk
#
if cmd=='rebuild' and (len(args)==1 or len(args)==3):
    if len(args)==3:
        taskpath = proddir+'/%s/%s/' % (args[1],args[2])
    else:
        taskpath = proddir+'/*/*/'
    if not options.batch:
        print 'Rebuilding the following tasks:\n'
        print  "    DB  %-50s  %s" % ('DATASET NAME','TASK NAME')
        print "    %s" % (75*'-')
        for p in glob.glob(taskpath):
            (dsName,taskName) = p.split('/')[-3:-1]
            nDefined = taskman.getNTasks(['where DSNAME=',DbParam(dsName),'and TASKNAME=',DbParam(taskName)])
            if options.verbose:
                print "    %s   %-50s  %s" % ('*' if nDefined>0 else ' ',dsName,taskName)
            else:
                if nDefined==0:
                    print "        %-50s  %s" % (dsName,taskName)
        a = raw_input('\nARE YOU SURE [n] ? ')
        if a!='y':
            sys.exit('ERROR: Rebuilding aborted by user')
        print
    for p in glob.glob(taskpath):
        (dsName,taskName) = p.split('/')[-3:-1]
        nDefined = taskman.getNTasks(['where DSNAME=',DbParam(dsName),'and TASKNAME=',DbParam(taskName)])
        if nDefined==0:
            print 'Adding missing entry for task %s/%s' % (dsName,taskName)
            configFile = glob.glob(proddir+'/'+dsName+'/'+taskName+'/*/*.config.py')
            if configFile:
                mtime = os.stat(configFile[0])[stat.ST_MTIME]
                config = {}
                exec(open(configFile[0],'r'),config)    # Eval config file and put defs into config dict
                try:
                    template = config['jobConfig']['joboptionpath']
                except:
                    template = 'UNKNOWN'
                try:
                    release = config['jobConfig']['release']
                except:
                    release = 'UNKNOWN'
                try:
                    taskpostprocsteps = config['jobConfig']['taskpostprocsteps']
                except:
                    taskpostprocsteps = ''
                try:
                    comment = config['jobConfig']['comment'] + '   (rebuilt from config file)'
                except:
                    comment = 'rebuilt from config file'
                taskman.addTask(dsName,taskName,template,release,0,taskpostprocsteps,createdTime=mtime,createdUser='UNKNOWN',createdHost='UNKNOWN',comment=comment)
            else:
                print 'WARNING: no config file found, unable to determine task creation time and configuration details'
                # Should we still add such a task - most likely this is a directory of files, e.g. some AOD or ESD files
                taskman.addTask(dsName,taskName,'UNKNOWN','UNKNOWN',0,comment='rebuilt, no config file found')
            a = TaskAnalyzer(proddir,dsName,taskName)
            a.analyzeFiles()
            a.updateStatus(taskman)
        if nDefined==1:
            print 'Entry exists already for task %s/%s - SKIPPED' % (dsName,taskName)
            #a = TaskAnalyzer(proddir,dsName,taskName)
            #a.analyzeFiles()
            #a.updateStatus(taskman)
        if nDefined>1:
            print 'ERROR: %i conflicting task entries for task %s/%s - SKIPPED' % (nDefined,dsName,taskName)
    sys.exit(0)


#
# Import tasks from another task database
#
if cmd=='import' and len(args)==2:
    try:
        fromman = TaskManager(args[1])
    except:
        print 'ERROR: Unable to access source task manager database %s' % args[1]
        sys.exit(1)
    nImported = 0
    for t in fromman.taskIterDict():
        print 'Importing ',t['DSNAME'],'/',t['TASKNAME'],'...'
        if not 'RUNNR' in t.keys():
            # Fill run number from DSNAME
            t['RUNNR'] = getRunFromName(t['DSNAME'],None,True)
        if not t.get('RESULTLINKS'):
            # Rebuild old result files and links
            links = ''
            files = t.get('RESULTFILES','')
            if files==None:
                files = ''
            dsname = t['DSNAME']
            taskname = t['TASKNAME']
            summaryFiles = glob.glob('%s/%s/*beamspot.gif' % (dsname,taskname))
            if len(summaryFiles)>0:
                for r in summaryFiles:
                    f = r.split('/')[-1]
                    pdf = f[:-3]+'pdf'
                    if not f in files.split():
                        files = ' '.join([files,f])
                    if not pdf in files.split():
                        files = ' '.join([files,pdf])
                    links += ' <a href="../files?u=%s/%s/%s">summary</a>' %(dsname,taskname,f)
                    links += ' (<a href="/jobfiles/%s/%s/%s">pdf</a>)' %(dsname,taskname,pdf)
            monitoringFiles = glob.glob('%s/%s/*beamspotmon.gif' % (dsname,taskname))
            if len(monitoringFiles)>0:
                for r in monitoringFiles:
                    f = r.split('/')[-1]
                    pdf = f[:-3]+'pdf'
                    if not f in files.split():
                        files = ' '.join([files,f])
                    if not pdf in files.split():
                        files = ' '.join([files,pdf])
                    links += ' <a href="../files?u=%s/%s/%s">monitoring</a>' %(dsname,taskname,f)
                    links += ' (<a href="/jobfiles/%s/%s/%s">pdf</a>)' %(dsname,taskname,pdf)
            t['RESULTFILES'] = files
            t['RESULTLINKS'] = links

        fieldNameString = ''
        params = []
        for fieldName in t.keys():
            newFieldName = fieldName
            if fieldName=='TASKID':
                continue # Skip old primary key
            if fieldName=='COMMENT':
                newFieldName = 'TASKCOMMENT'
            if len(params):
                fieldNameString += ','
                params.append(',')
            fieldNameString += newFieldName
            params.append(DbParam(t[fieldName]))
        q = [ 'insert into TASKS (%s) values (' % fieldNameString ]
        q.extend(params)
        q.append(')')
        taskman.execute(q, True)
        nImported += 1
    print '\n%i tasks imported' % nImported
    sys.exit(0)


#
# Set task status
#
if cmd=='setstatus' and len(args)==4:
    statusName = args[3].upper()
    status = TaskManager.StatusCodes.get(statusName,None)
    if status:
        print 'Setting task status to %s   (code %i)' % (statusName,status)
    else:
        sys.exit('ERROR: Illegal status code name %s' % args[3])
    print
    try:
        taskList = getFullTaskNames(taskman,args[1],args[2],confirmWithUser=not options.batch,addWildCards=not options.nowildcards)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
    n = 0
    for t in taskList:
        n += taskman.setValue(t[0],t[1],'STATUS',status)
    print '%i task status entries updated\n' % (n)
    sys.exit(0)


#
# Set database field
#
if cmd=='setfield' and len(args)==5:
    fieldName = args[3].upper()
    fieldValue = args[4]
    print 'Setting field %s to:   %s' % (fieldName,fieldValue)
    print
    try:
        [(dsname,task)] = getFullTaskNames(taskman,args[1],args[2],requireSingleTask=True,confirmWithUser=not options.batch,addWildCards=not options.nowildcards)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
    n = taskman.setValue(dsname,task,fieldName,fieldValue)
    print '%i task status entries updated\n' % (n)
    sys.exit(0)


#
# Delete task entry
#
if cmd=='delete' and len(args)==3:
    try:
        taskList = getFullTaskNames(taskman,args[1],args[2],confirmWithUser=not options.batch,addWildCards=not options.nowildcards)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
    n = 0
    for t in taskList:
        n += taskman.deleteTask(t[0],t[1])
    print '\n%i task entries deleted\n' % (n)
    sys.exit(0)

# List results for selected sets of tasks
#
if cmd=='listResults' and len(args)==3:
    try:
        taskList = getFullTaskNames(taskman,args[1],args[2],confirmWithUser=not options.batch,addWildCards=not options.nowildcards)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
    for t in taskList:
        print 'Listing results for task %s/%s ...' % (t[0],t[1]) 
        print taskman.getTaskValue(t[0],t[1],'RESULTFILES')
        print taskman.getTaskValue(t[0],t[1],'RESULTLINKS')
    sys.exit(0)



#
# Clear results for selected sets of tasks
# TODO: remove associated files as well (?)
#
if cmd=='deleteResults' and len(args)==3:
    try:
        taskList = getFullTaskNames(taskman,args[1],args[2],confirmWithUser=not options.batch,addWildCards=not options.nowildcards)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
    for t in taskList:
        print 'Deleting results for task %s/%s ...' % (t[0],t[1]) 
        taskman.setValue(t[0],t[1],'RESULTFILES','')
        taskman.setValue(t[0],t[1],'RESULTLINKS','')
    sys.exit(0)


#
# Delete task (files and associated TaskManager entry)
#
if cmd=='deleteTask' and len(args)==3:
    try:
        [(dsname,task)] = getFullTaskNames(taskman,args[1],args[2],confirmWithUser=not options.batch,requireSingleTask=True,addWildCards=not options.nowildcards)
    except TaskManagerCheckError, e:
        print e
        sys.exit(1)
    dir = '/'.join([proddir,dsname,task])
    print
    if os.path.exists(dir):
        os.system('du -hs %s' % dir)
    else:
        print 'No task files found (no directory %s)' % dir
    if not options.batch:
        a = raw_input('\nDeleteing task entry and all files - ARE YOU SURE [n] ? ')
        if a!='y':
            print "ERROR: Deletion aborted by user."
            sys.exit(1)
    n = taskman.deleteTask(dsname,task)
    print '\n%i task entry deleted.' % (n)
    print 'Deleting ',dir,' ...'
    os.system('rm -rf %s' % dir)
    sys.exit(0)

if cmd=='notifyFailed' and len(args)<3:
    if len(args)==2:
        earliestUpdateTime = time.time()-float(args[1])
    else:
        earliestUpdateTime = 0

    taskList = []
    for t in taskman.taskIterDict('*',["where TASKNAME like '%%%s%%'" % options.runtaskname,
                                       "and UPDATED >= ", DbParam(earliestUpdateTime),
                                       "and (STATUS = %i or STATUS = %i)" %(TaskManager.StatusCodes['FAILED'], TaskManager.StatusCodes['POSTPROCFAILED']),
                                       "order by RUNNR desc"]):

        # Change status to string version
        t['STATUS'] = [k for k, v in TaskManager.StatusCodes.iteritems() if v == t['STATUS']][0]
        taskList.append(t)

    if taskList:

        dsWidth     = len(max([t['DSNAME'] for t in taskList],key=len))
        taskWidth   = len(max([t['TASKNAME'] for t in taskList],key=len))
        statusWidth = len(max([t['STATUS'] for t in taskList],key=len))

        bodyFormat  = '%%(DSNAME)%ss %%(TASKNAME)%ss %%(STATUS)%ss' %(dsWidth, taskWidth, statusWidth)
        
        mailBody = 'The following %s tasks have reported failures' % len(taskList)
        if len(args)==2:
            hours = round(float(args[1])/3600.)
            mailBody += ' in the last %s hours' % hours
        mailBody += ':\n\n'
        mailBody += '\n'.join([bodyFormat % t for t in taskList])
        stat = os.system('source /afs/cern.ch/user/a/atlidbs/cron/mailwrapper; mailwrapper "[atlidbs] %s tasks report failures" "%s"' %(len(taskList), mailBody)) >> 8

        if stat:
            print '\nERROR: Unable to send mail\n'
    
    sys.exit(0)

#
# Debugging (for experts only!)
#
# This must be the last command. If we end up here, it's either debugging
# or an illeage command.
#
if cmd=='debug' and len(args)==1:
    taskman.debug = True
    os.environ['PYTHONINSPECT'] = '1'
    print
    print 'Entering python shell. taskman is the TaskManager object. Be careful!'
    print
else:
    print 'ERROR: Illegal command or number of arguments'
    sys.exit(1)
