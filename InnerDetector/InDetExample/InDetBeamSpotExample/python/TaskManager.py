#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
TaskManager is a tool for keeping track of JobRunner jobs using a
database. TaskManager uses the notion of a task as the primary unit for
bookkeeping. Each task includes one or more jobs run using the same
JobRunner template and typically resulting from a single invocation of
JobRunner through one of the makeDPD.py or runDPD.py
scripts. TaskManager keeps track of the status of tasks and associated
jobs, runs postprocessing scripts, and accumulates results for later
analysis accross tasks.

Written by Juerg Beringer (LBNL) in 2009.
"""
__author__  = 'Juerg Beringer'
__version__ = 'TaskManager.py atlas/athena'

import time, os, glob, dircache

from InDetBeamSpotExample.Utils import getRunFromName
from InDetBeamSpotExample.Utils import getUserName
import six


# Exception classes
class TaskManagerCheckError(Exception):
    """Task manager exception class."""

class TaskManagerDatabaseError(Exception):
    """Task manager exception because of a database error"""


def dictFactory(cursor, row):
    """Convert a tuple from a database query into a dictonary."""
    d = {}
    for idx, col in enumerate(cursor.description):
        d[col[0]] = row[idx]
    return d


def getKey(d, v):
    """Get the key for which dictonary d has an entry with value v.
    Returns 'Undefined' if there's no such key, if several values are found."""
    l = [k for k in d.keys() if d[k]==v]
    if len(l)==1:
        return l[0]
    else:
        return 'Undefined'


def getStatusClass(status):
    """Returns 'ok', 'warn' or 'bad' depending on the value of status."""
    statusMap = {
        7: 'bad',
        10: 'ok',
        11: 'bad'
        }
    if status in statusMap:
        return statusMap[status]
    else:
        return 'warn'


def appendUnique(s,v):
    if not s:
        s = ''   # make sure it is not None
    if v not in s.split():
        s = ' '.join([s,v])
    return s


def getFullTaskNames(taskman,dsname,taskname,requireSingleTask=False,confirmWithUser=False,addWildCards=True):
    """Retrieve the full dataset and task names given a pair of (dsname,task) that may
       contain wildcards or be just a parital name such as the run number. Depending
       on the requireSingleTask and confirmWithUser settings a TaskManagerCheckError
       is raised if there are multiple tasks or if the user doesn't confirm."""
    taskList = taskman.getTaskNames(dsname,taskname,addWildCards)
    if len(taskList)==0:
        raise  TaskManagerCheckError ('ERROR: No tasks found for dataset = %s, task = %s' % (dsname,taskname))
    if requireSingleTask and len(taskList)!=1:
        m = "ERROR: Multiple data set names found for dataset = %s, task = %s\n       Please use full dataset or task name from list below, using option -n if necessary:\n\n" % (dsname,taskname)
        m += "    %-50s  %s\n" % ('DATASET NAME','TASK NAME')
        m += "    %s\n" % (75*'-')
        for t in taskList:
            m += "    %-50s  %s\n" % (t[0],t[1])
        m += '\n'
        raise TaskManagerCheckError (m)
    if confirmWithUser:
        print ('Please confirm that you want to execute this command for the following tasks:\n')
        print ("    %-50s  %s" % ('DATASET NAME','TASK NAME'))
        print ("    %s" % (75*'-'))
        for t in taskList:
            print ("    %-50s  %s" % (t[0],t[1]))
        a = input('\nARE YOU SURE [n] ? ')
        if a!='y':
            raise TaskManagerCheckError ('ERROR: Aborted by user')
        print()
    return taskList


def getJobConfig(jobDir,dsName,taskName,jobName='*'):
    """Read config dict from job files."""
    config = {}
    configFile = glob.glob('%s/%s/%s/%s/%s' % (jobDir,dsName,taskName,jobName,'*.config.py.final.py'))
    if not configFile:
        configFile = glob.glob('%s/%s/%s/%s/%s' % (jobDir,dsName,taskName,jobName,'*.config.py'))
    if configFile:
        exec(open(configFile[0],'r'),config)    # Eval config file and put defs into config dict
    return config['jobConfig']


class DbParam(object):
    """A utility class to hold a single parameter to be used in SQL queries."""
    def __init__(self,value):
        self.value = value
    def __repr__(self):
        return '%r' % self.value


class TaskManager:
    """TaskManager is tool for keeping track of JobRunner jobs."""

    StatusCodes = { 'UNKNOWN': -1,
                    'CONFIGURED': 0,
                    'SUBMITTED': 1,
                    'RUNNING':   2,
                    'PARTIALFAILED': 3,
                    # Codes above are considered tasks whose status needs to
                    # be monitored and updated
                    'POSTPROCESSING': 5,
                    'POSTPROCRUNNING': 6,
                    'POSTPROCFAILED': 7,
                    'COMPLETED': 10,
                    'FAILED': 11}

    OnDiskCodes = { 'UNKNOWN': -1,
                    'ALLONDISK': 0,
                    'RESULTSONDISK': 1,
                    # Codes above are for jobs whose task directory is on disk
                    'ARCHIVED': 10,
                    'DELETED': 11 }

    @classmethod
    def parseConnectionInfo(self, connstring=''):
        if not connstring:
            connstring = os.environ.get('TASKDB', 'sqlite_file:taskdata.db')

        try:
            dbtype, dbname = connstring.split(':', 1)
        except Exception:
            raise ValueError ('Illegal database connection string {}'.format(connstring))

        if dbtype == 'auth_file':
            # dbname is a file with the actual connection information
            authfile = dbname
            try:
                with open(authfile) as af:
                    connstring = af.read().strip()
                dbtype, dbname = connstring.split(':', 1)
            except Exception:
                raise ValueError ('Invalid authorization file {} (not readable or invalid format)'.format(authfile))

        return dbtype, dbname


    def __init__(self, connstring='', createDatabase=False):
        """Constructor. connstring is a connection string specifying either a SQLite file
           ("sqlite_file:..."), an Oracle database ("oracle://..."), or an authorization file
           ("auth_file:...") with connection information. If connstring is empty, the connection
           information will be taken from TASKDB (if set), or otherwise defaults to
           'sqlite_file:taskdata.db'."""

        self.debug = False
        self.paramstyle = None
        self.is_managing_context = False

        self.dbtype, self.dbname = self.__class__.parseConnectionInfo(connstring)

        if self.dbtype == 'sqlite_file':
            import sqlite3
            self.paramstyle = 'qmark'
            dbexists = os.access(self.dbname, os.F_OK)
            if dbexists and createDatabase:
                raise ValueError ('SQLite file {} exists already - remove before recreating'.format(self.dbname))
            if not (dbexists or createDatabase):
                raise ValueError ('TaskManager database not found (SQLite file {})'.format(self.dbname))
            self.dbcon = sqlite3.connect(self.dbname)
            if createDatabase:
                self._createSQLiteSchema()
        elif self.dbtype == 'oracle':
            import cx_Oracle
            self.paramstyle = 'named'
            try:
                self.dbcon = cx_Oracle.connect(self.dbname)
            except Exception:
                print ('ERROR: First connection attempt to Beam Spot Oracle database failed; will retry in 10s ...')
                time.sleep(10)
                self.dbcon = cx_Oracle.connect(self.dbname)
            if createDatabase:
                self._createOracleSchema()
        else:
            raise ValueError ('Unknown database type: {}'.format(self.dbtype))

    def __enter__(self):
        ''' Remember that we're inside a 'with' statement so we can warn otherwise: '''
        self.is_managing_context = True
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        ''' Close the database connection at the end of the 'with' statement. '''
        try:
            self.dbcon.close()
        except Exception:
            print ('ERROR: Unable to close database connection')

    def _createSQLiteSchema(self):
        """Create the database schema for a SQLite3 database."""
        self.dbcon.cursor().executescript("""
            create table tasks (
                TASKID integer primary key autoincrement,
                DSNAME text,
                RUNNR integer,
                TASKNAME text,
                TEMPLATE text,
                TASKPOSTPROCSTEPS text,
                ATLREL text,
                CREATED real,
                CREATED_USER text,
                CREATED_HOST text,
                UPDATED real,
                STATUS integer,
                ONDISK integer,
                LOGFILES text,
                COOLTAGS text,
                NRESULTFILES integer default 0,
                NJOBS integer default 0,
                NJOBS_SUBMITTED integer default -1,
                NJOBS_RUNNING integer default -1,
                NJOBS_POSTPROC integer default -1,
                NJOBS_FAILED integer default -1,
                NJOBS_COMPLETED integer default -1,
                RESULTFILES text,
                RESULTLINKS text,
                TASKCOMMENT text,
                STATUS_POSTPROC integer,
                BASEPATH text,
                AUXDATA text
             );
         """)
        self.dbcon.commit()


    def _createOracleSchema(self):
        """Create the database schema for an Oracle database."""
        try:
            self.dbcon.cursor().execute('drop sequence TASKS_SEQ')
            self.dbcon.cursor().execute('drop trigger TASKS_ID_TRIGGER')
            self.dbcon.cursor().execute('drop table TASKS')
        except Exception as e:
            print (e)
        self.dbcon.cursor().execute("""
            create table tasks (
                TASKID number(10,0),
                DSNAME varchar2(256),
                RUNNR number(10,0),
                TASKNAME varchar2(256),
                TEMPLATE varchar2(256),
                TASKPOSTPROCSTEPS varchar2(256),
                ATLREL varchar2(256),
                CREATED number,
                CREATED_USER varchar2(256),
                CREATED_HOST varchar2(256),
                UPDATED number,
                STATUS number(5,0),
                ONDISK number(5,0),
                LOGFILES varchar2(256),
                COOLTAGS varchar2(256),
                NRESULTFILES number(10,0) default 0,
                NJOBS number(10,0) default 0,
                NJOBS_SUBMITTED number(10,0) default -1,
                NJOBS_RUNNING number(10,0) default -1,
                NJOBS_POSTPROC number(10,0) default -1,
                NJOBS_FAILED number(10,0) default -1,
                NJOBS_COMPLETED number(10,0) default -1,
                RESULTFILES varchar2(4000),
                RESULTLINKS varchar2(4000),
                TASKCOMMENT varchar2(4000),
                STATUS_POSTPROC number(5,0) default -1,
                BASEPATH varchar2(256),
                AUXDATA varchar2(4000),
                constraint BEAMSPOT_TASKS_PK primary key (TASKID)
              )
            """)
        self.dbcon.cursor().execute('create index TASKS_DSNAME_INDX on TASKS(DSNAME)')
        self.dbcon.cursor().execute('create index TASKS_RUNNR_INDX on TASKS(RUNNR)')
        self.dbcon.cursor().execute('alter table TASKS add constraint TASKS_TNAME_DSNAME_UQ  unique (TASKNAME, DSNAME)')
        self.dbcon.cursor().execute('create sequence TASKS_SEQ start with 1 increment by 1 nomaxvalue')
        # The following statement must not have whitespace at the beginning of the line, otherwise
        # the trigger function won't be parsed correctly:
        self.dbcon.cursor().execute("""
create or replace trigger TASKS_ID_TRIGGER
before insert on TASKS
for each row
begin
   select TASKS_SEQ.nextval into :new.TASKID from dual;
end;
""")
        self.dbcon.commit()


    def execute(self,statementParts, doCommit=False, limit=None):
        """Execute a SQL statement passed as a list or tuple of statement parts, where each
           part is either a partial SQL string, or an object of type DbParam specifying a parameter
           for the SQL statement. The actual SQL statement is assembled (using the parameter style
           of the current database) and executed, and the resulting cursor is returned.
           Loosely follows the method discussed in the Python Cookbook.
           WARNING: At present, limit doesn't work when ordering rows for Oracle!"""
        if not self.is_managing_context:
            print ('**WARN**  TaskManager will keep the database connection open until it is deleted.')
            print ('**INFO**  TaskManager should generally only be used inside a with statement:')
            print ('**INFO**      with TaskManager(...) as taskman:')
            print ('**INFO**        # do something ...')

        if not statementParts:
            return None
        if isinstance(statementParts,str):
            raise TypeError ('Must pass list or tuple to TaskManager.execute')
        for p in statementParts:
            if not (isinstance(p,DbParam) or isinstance(p,six.string_types)):
                raise ValueError ('Can only pass SQL string fragments and DbParam objects in list'
                                   'to TaskManager.execute, found %s with "%s"' % (type(p),str(p)))
        sqlParts = None
        params = None

        if self.paramstyle=='qmark':
            sqlParts = []
            params = []
            for p in statementParts:
                if isinstance(p,DbParam):
                    sqlParts.append('?')
                    params.append(p.value)
                else:
                    sqlParts.append(p)

        if self.paramstyle=='named':
            sqlParts = []
            params = {}
            for p in statementParts:
                if isinstance(p,DbParam):
                    name = 'p%d' % len(params)
                    sqlParts.append(':%s' % name)
                    params[name] = p.value
                else:
                    sqlParts.append(p)

        if sqlParts is None:
            raise ValueError ('Unknown SQL parameter style %s' % self.paramstyle)
            return None
        sql = ' '.join(sqlParts)

        # Handle database-dependent limit clause (PRELIMINARY)
        if limit:
            if self.dbtype=='oracle':
                # WARNING: the following doesn't work always when ordering rows!
                if 'order by' in sql:   # WARNING: works only if order by is in lowercase!
                    sqlParts2 = sql.split('order by')
                    sql = '%s and ROWNUM <= %i order by %s' % (sqlParts2[0],limit,sqlParts2[1])
                else:
                    sql += ' and ROWNUM <= %i' % limit
            else:
                sql += ' limit %i' % limit

        if self.debug:
            print ('\nExecuting SQL statement:  ',sql)
            print ('        with parameters:  ',params)

        cursor = self.dbcon.cursor()
        try:
            cursor.execute(sql,params)
            if doCommit:
                self.dbcon.commit()
        except Exception as e:
            msg = '\nDatabase error executing SQL statement\n  %s\nusing parameters\n  %s\n%s' % (sql,params,e)
            raise TaskManagerDatabaseError (msg)
        return cursor


    def deleteTask(self,dsName,taskName):
        """Delete a task entry from the taskmanager database."""
        q = [ 'delete from TASKS where DSNAME =',DbParam(dsName),'and TASKNAME =',DbParam(taskName) ]
        cursor = self.execute(q,True)
        return cursor.rowcount


    def addTask(self,dsName,taskName,template,release,njobs,
                taskpostprocsteps='',
                status=StatusCodes['SUBMITTED'],
                onDisk=OnDiskCodes['ALLONDISK'],
                createdTime=None,
                createdUser=None,
                createdHost=None,
                comment=''):
        """Add an entry for a new task if the task doesn't exist already. If the task exists,
           its UPDATED, NJOBS, STATUS and ONDISK fields will be updated."""
        tstamp = time.time()
        nDefined = self.getNTasks(['where DSNAME =',DbParam(dsName),'and TASKNAME=',DbParam(taskName)])
        updateStatus = self.getNTasks(['where DSNAME = ',DbParam(dsName),'and TASKNAME=',DbParam(taskName),'and STATUS < %i' % TaskManager.StatusCodes['POSTPROCRUNNING']])

        if nDefined:
            # Task entry exists already; only update UPDATED, NJOBS, STATUS, ONDISK and possibly ATLREL
            task = self.getTaskDict(dsName,taskName)
            if task['TEMPLATE']!=template:
                print ('ERROR: Must not update task with different template: DSNAME = %s, TASKNAME = %s, templates = %s vs %s' % (dsName,taskName,task['TEMPLATE'],template))
            else:
                updateStr = ['update TASKS set UPDATED =',DbParam(tstamp),
                             ', NJOBS = ',DbParam(task['NJOBS']+njobs),
                             ', ONDISK = ',DbParam(onDisk)]

                if release not in task['ATLREL']:
                    print ('WARNING: Updating task using different release: DSNAME = %s, TASKNAME = %s, release = = %s vs %s' % (dsName,taskName,task['ATLREL'],release))
                    release = '; '.join([task['ATLREL'],release])
                    updateStr += [', ATLREL = ',DbParam(release)]

                if updateStatus:
                    updateStr += [', STATUS = ',DbParam(status)]

                updateStr += ['where DSNAME = ',DbParam(dsName),'and TASKNAME = ',DbParam(taskName)]

                self.execute( updateStr)

        else:
            # New task entry
            if not createdTime:
                createdTime = tstamp
            if not createdUser:
                createdUser = getUserName()
            if not createdHost:
                createdHost = os.uname()[1]
            runnr = getRunFromName(dsName,None,True)
            self.execute( ['insert into TASKS (DSNAME,RUNNR,TASKNAME,TEMPLATE,TASKPOSTPROCSTEPS,ATLREL,CREATED,CREATED_USER,CREATED_HOST,UPDATED,STATUS,ONDISK,NJOBS,TASKCOMMENT) values (',
                           DbParam(dsName),',',
                           DbParam(runnr),',',
                           DbParam(taskName),',',
                           DbParam(template),',',
                           DbParam(taskpostprocsteps),',',
                           DbParam(release),',',
                           DbParam(createdTime),',',
                           DbParam(createdUser),',',
                           DbParam(createdHost),',',
                           DbParam(tstamp),',',
                           DbParam(status),',',
                           DbParam(onDisk),',',
                           DbParam(njobs),',',
                           DbParam(comment),')'],
                           True)


    def getStatus(self,dsName,taskName):
        """Get status of a task."""
        cursor = self.execute( ['select STATUS from TASKS where DSNAME =', DbParam(dsName), 'and TASKNAME =', DbParam(taskName) ] )
        return cursor.fetchone()[0]


    def setStatus(self,dsName,taskName,status,oldStatus=None):
        """Set the status of a task. If oldStatus is set, the task must be in the designated status."""
        tstamp = time.time()
        if oldStatus:
            cursor = self.execute( ['update TASKS set UPDATED =',DbParam(tstamp),', STATUS =',DbParam(status),
                                    'where DSNAME =', DbParam(dsName), 'and TASKNAME =', DbParam(taskName),'and STATUS =',DbParam(oldStatus)], True)
        else:
            cursor = self.execute( ['update TASKS set UPDATED =',DbParam(tstamp),', STATUS =',DbParam(status),
                                    'where DSNAME =', DbParam(dsName), 'and TASKNAME =', DbParam(taskName)], True)
        return cursor.rowcount


    def setDiskStatus(self,dsName,taskName,onDisk):
        """Update the onDisk-status of a task."""
        tstamp = time.time()
        cursor = self.execute( ['update TASKS set UPDATED =',DbParam(tstamp),', ONDISK =',DbParam(onDisk),
                                'where DSNAME =', DbParam(dsName), 'and TASKNAME =', DbParam(taskName)], True)
        return cursor.rowcount


    def updateStatus(self,dsName,taskName,
                     status,nResultFiles,nJobs,nJobsSubmitted,nJobsRunning,nJobsPostProc,nJobsFailed,nJobsCompleted):
        """Update task status information including number of jobs in different states."""
        tstamp = time.time()
        cursor = self.execute( ['update TASKS set UPDATED =',DbParam(tstamp),
                                ', STATUS =',DbParam(status),
                                ', NRESULTFILES =',DbParam(nResultFiles),
                                ', NJOBS =',DbParam(nJobs),
                                ', NJOBS_SUBMITTED =',DbParam(nJobsSubmitted),
                                ', NJOBS_RUNNING =',DbParam(nJobsRunning),
                                ', NJOBS_POSTPROC =',DbParam(nJobsPostProc),
                                ', NJOBS_FAILED =',DbParam(nJobsFailed),
                                ', NJOBS_COMPLETED =',DbParam(nJobsCompleted),
                                'where DSNAME =', DbParam(dsName), 'and TASKNAME =', DbParam(taskName)], True)
        return cursor.rowcount


    def setValue(self,dsName,taskName,fieldName,value):
        """Update a field of a given task."""
        tstamp = time.time()
        cursor = self.execute( ['update TASKS set UPDATED =',DbParam(tstamp),', %s =' % fieldName,DbParam(value),
                                'where DSNAME =', DbParam(dsName), 'and TASKNAME =', DbParam(taskName)], True)
        return cursor.rowcount


    def getValue(self, what, qual=()):
        """Get a single value from the task database. If the query results in more than one value, only
           the first value is returned."""
        q = [ 'select %s from TASKS' % what ]
        q.extend(qual)
        return self.execute(q).fetchone()[0]


    def getCount(self, what, qual=()):
        # Except when using count(distinct(...)) which is not supported by earlier sqlite versions,
        # better use getNTasks
        q = [ 'select %s from TASKS' % what ]
        q.extend(qual)
        return len(self.execute(q).fetchall())


    def getNTasks(self, qual=()):
        q = [ 'select count(*) from TASKS' ]
        q.extend(qual)
        return self.execute(q).fetchone()[0]

    def getTaskValue(self, dsName,taskName, what):
        """Get a single value from the task database. If the query results in more than one value, only
           the first value is returned."""
        q = [ 'select %s from TASKS where DSNAME =' % what , DbParam(dsName), 'and TASKNAME =', DbParam(taskName)]
        return self.execute(q).fetchone()[0]

    def taskIter(self, what='*', qual=('order by UPDATED',)):
        q = [ 'select %s from TASKS' % what ]
        q.extend(qual)
        cursor = self.execute(q)
        return iter(cursor.fetchone,None)


    def taskIterDict(self, what='*', qual=('order by UPDATED',), limit=999999999):
        # NOTE: We could implement this more elegantly using row_factory (sqlite3)
        #       or rowfactory (Oracle), but this would come at the expense of a
        #       direct database dependence. Also, the primitive limit on the
        #       number of items returned by the generator avoids database-
        #       dependent SQL limit statements (until limit in TaskManager.execute
        #       is fully implemented also for Oracle.
        q = [ 'select %s from TASKS' % what ]
        q.extend(qual)
        cursor = self.execute(q)
        n = 0
        while n<limit:
            row = cursor.fetchone()
            if row is None: break
            n += 1
            yield dictFactory(cursor,row)


    def getTaskDict(self, dsname, taskname, what='*', qual=()):
        # NOTE: We could implement this more elegantly using row_factory (sqlite3)
        #       or rowfactory (Oracle), but this would come at the expense of a
        #       direct database dependence.
        if qual:
            q = [ 'select %s from TASKS where DSNAME =' % what, DbParam(dsname), 'and TASKNAME =', DbParam(taskname) ]
            q.extend(qual)
        else:
            if self.dbtype=='oracle':
                # WARNING: DOES THIS ALWAYS WORK? There anyway should only be a single task here?
                q = [ 'select %s from TASKS where DSNAME =' % what, DbParam(dsname), 'and TASKNAME = ', DbParam(taskname), 'and ROWNUM <= 1 order by UPDATED' ]
            else:
                q = [ 'select %s from TASKS where DSNAME =' % what, DbParam(dsname), 'and TASKNAME = ', DbParam(taskname), 'order by UPDATED limit 1' ]
        cursor = self.execute(q)
        row = cursor.fetchone()
        if row is None:
            return {}
        else:
            return dictFactory(cursor,row)


    def getDSNames(self,dsname):
        """Get all DSNAMEs for a given (partial) data set name dsname."""
        #q = [ "select distinct(DSNAME) from TASKS where DSNAME like '%%%s%%' order by UPDATED" % dsname ]
        q = [ "select distinct(DSNAME) from TASKS where DSNAME like '%%%s%%' order by DSNAME" % dsname ]
        return self.execute(q).fetchall()


    def getTaskNames(self,dsname,taskname=None,addWildCards=True):
        """Find all matching tasks and return list of full (DSNAME,TASKNAME) pairs. dsname and taskname
           may be any partial dataset and task name, respectively."""
        if addWildCards:
            if taskname:
                q = [ "select DSNAME,TASKNAME from TASKS where DSNAME like '%%%s%%' and TASKNAME like '%%%s%%' order by DSNAME,TASKNAME" % (dsname,taskname) ]
            else:
                q = [ "select DSNAME,TASKNAME from TASKS where DSNAME like '%%%s%%' order by DSNAME,TASKNAME" % (dsname) ]
        else:
            if taskname:
                q = [ "select DSNAME,TASKNAME from TASKS where DSNAME like '%s' and TASKNAME like '%s' order by DSNAME,TASKNAME" % (dsname,taskname) ]
            else:
                q = [ "select DSNAME,TASKNAME from TASKS where DSNAME like '%s' order by DSNAME,TASKNAME" % (dsname) ]
        return self.execute(q).fetchall()


class TaskAnalyzer:
    """TaskAnalyzer is a class for analyzing the files of a given task."""

    def __init__(self,jobDir,dsName,taskName):
        self.jobDir = jobDir
        self.dsName = dsName
        self.taskName = taskName
        self.path = jobDir+'/'+dsName+'/'+taskName

        self.status = TaskManager.StatusCodes['UNKNOWN']
        self.onDisk = TaskManager.OnDiskCodes['UNKNOWN']
        self.nResultFiles = 0
        self.nJobs = 0
        self.nJobsSubmitted = 0
        self.nJobsRunning = 0
        self.nJobsPostProc = 0
        self.nJobsFailed = 0
        self.nJobsCompleted = 0


    def isOnDisk(self):
        """Check if task directories are still on disk."""
        onDisk = os.path.exists(self.path)
        if not onDisk:
            self.onDisk = TaskManager.OnDiskCodes['DELETED']
        return onDisk


    def analyzeFiles(self):
        onDisk = self.isOnDisk()
        if onDisk:
            self.nJobs = len(glob.glob(self.path+'/*/*.config.py'))
            self.nResultFiles = len(glob.glob(self.path+'/*')) - self.nJobs
            self.nJobsSubmitted = len(glob.glob(self.path+'/*/*.SUBMITTED'))
            self.nJobsRunning = len(glob.glob(self.path+'/*/*.RUNNING'))
            self.nJobsPostProc = len(glob.glob(self.path+'/*/*.POSTPROCESSING'))
            self.nJobsCompleted = len(glob.glob(self.path+'/*/*.COMPLETED'))
            self.nJobsFailed = self.nJobsCompleted - len(glob.glob(self.path+'/*/*.exit.0'))

            self.status = TaskManager.StatusCodes['UNKNOWN']
            if self.nJobs>0:
                self.status = TaskManager.StatusCodes['CONFIGURED']
            if self.nJobs==self.nJobsCompleted:
                self.status = TaskManager.StatusCodes['COMPLETED']

            if self.nJobsSubmitted>0:
                self.status = TaskManager.StatusCodes['SUBMITTED']
            if self.nJobsRunning>0:
                self.status = TaskManager.StatusCodes['RUNNING']
            if self.nJobsPostProc>0:
                self.status = TaskManager.StatusCodes['POSTPROCESSING']
            if self.nJobsFailed>0:
                if self.nJobsSubmitted>0 or self.nJobsRunning>0 or self.nJobsPostProc>0:
                    self.status = TaskManager.StatusCodes['PARTIALFAILED']
                else:
                    self.status = TaskManager.StatusCodes['FAILED']

        return onDisk


    def updateStatus(self,taskman):
        taskman.updateStatus(self.dsName,self.taskName,
                             self.status,self.nResultFiles,
                             self.nJobs,self.nJobsSubmitted,self.nJobsRunning,self.nJobsPostProc,self.nJobsFailed,self.nJobsCompleted)



class JobAnalyzer:
    """JobAnalyzer is a class for analyzing the jobs of a given task."""

    def __init__(self,jobDir,dsName,taskName):
        self.jobDir = jobDir
        self.dsName = dsName
        self.taskName = taskName
        self.path = jobDir+'/'+dsName+'/'+taskName
        #self.maxWallTime = 5*86400   # max time job should be running in s


    def jobList(self):
        try:
            l = dircache.listdir(self.path)
        except Exception:
            l = []
        return l


    def jobStatus(self,jobName):
        status = TaskManager.StatusCodes['UNKNOWN']
        exitcode = ''
        p = self.path+'/'+jobName
        if os.path.exists(p):
            if glob.glob(p+'/*.config.py'):
                status = TaskManager.StatusCodes['CONFIGURED']
            if glob.glob(p+'/*.SUBMITTED'):
                status = TaskManager.StatusCodes['SUBMITTED']
            # Protect against jobs dying w/o updating time stamp files - must be
            # accompanied by similar protection in TaskAnalyzer
            #r = glob.glob(p+'/*.RUNNING'):
            #if r:
            #    status = TaskManager.StatusCodes['RUNNING']
            #    if len(r)==1 &&  (time.time()-os.path.getctime(r[0]))>self.maxWallTime:
            #        status = TaskManager.StatusCodes['FAILED']
            if glob.glob(p+'/*.RUNNING'):
                status = TaskManager.StatusCodes['RUNNING']
            if glob.glob(p+'/*.POSTPROCESSING'):
                status = TaskManager.StatusCodes['POSTPROCESSING']
            if glob.glob(p+'/*.COMPLETED'):
                status = TaskManager.StatusCodes['COMPLETED']
                # Check if job completed w/o producing an exit status
                if len(glob.glob(p+'/*.exit.0'))==0:
                    status = TaskManager.StatusCodes['FAILED']
            try:
                exitcode = open(glob.glob(p+'/*.exitstatus.dat')[0]).read()
            except Exception:
                pass
            if len(glob.glob(p+'/*.exit.*'))>len(glob.glob(p+'/*.exit.0')):
                status = TaskManager.StatusCodes['FAILED']
        return (status,exitcode)


#
# Test code for modules
#
if __name__ == '__main__':
    #t = TaskManager('sqlite_file:test.db', True)
    #t = TaskManager('sqlite_file:test.db')
    #t = TaskManager('oracle:USER/PASSWORD@HOST')
    #t.addTask('foo','bar','VertexTemplate.py','15.4.0',55)
    #t.addTask('foo','bbb','VertexTemplate.py','15.4.0',55)
    pass
