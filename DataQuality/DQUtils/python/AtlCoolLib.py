#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# AtlCoolLib.py
# module defining utilities for ATLAS command line tool use of COOL
# Richard Hawkings, started 5/2/07
"""
Module defining utilities for ATLAS command line/python use of COOL
Copied to DQUtils for private use
"""

import sys,os,traceback,getopt,time,calendar,commands
from PyCool import cool,coral

def transConn(conn):
    """
    Translate simple connection string (no slash) to mycool.db with given
    instance, all others left alone
    """
    if (not '/' in conn):
        return 'sqlite://X;schema=mycool.db;dbname='+conn
    else:
        return conn

def forceOpen(conn):
    """
    Open COOL database with given connection, or force open if not possible
    Return COOL database object, or None if cannot be opened
    Database is opened in update mode
    """
    dbSvc=cool.DatabaseSvcFactory.databaseService()
    conn2=transConn(conn)
    try:
        db=dbSvc.openDatabase(conn2,False)
    except Exception, e:
        print e
        print 'Could not connect to',conn,'try to create it'
        try:
            db=dbSvc.createDatabase(conn2)
        except Exception, e:
            print e
            print 'Could not create the database - give up'
            return None
    return db

def readOpen(conn):
    """
    Open COOL database with given connection for reading
    """
    dbSvc=cool.DatabaseSvcFactory.databaseService()
    conn2=transConn(conn)
    try:
        db=dbSvc.openDatabase(conn2,True)
    except Exception, e:
        print e
        print 'Could not connect to',conn
        return None
    return db
    
def ensureFolder(db,folder,spec,desc,version=cool.FolderVersioning.SINGLE_VERSION):
    """
    Ensure folder exists, creating it if needed with given spec
    Return COOL folder object
    """
    if (not db.existsFolder(folder)):
        print 'Attempting to create',folder,'with description',desc
        try:
            # Deprecated/dropped:  db.createFolder(folder,spec,desc,version,True)
            folderSpec = cool.FolderSpecification(version,spec)
            db.createFolder(folder,folderSpec,desc,True)
            print 'Folder created'
        except Exception,e:
            print e
            print 'Could not create folder',folder
            return None
    return db.getFolder(folder)

def athenaDesc(runLumi,datatype):
    """
    Return the correct folder description string for Athena IOVDbSVc access
    runLumi=True or False for timestamp
    datatype= { AthenaAttributeList | CondAttrListCollection }
    """
    if (runLumi):
        desc='<timeStamp>run-lumi</timeStamp>'
    else:
        desc='<timeStamp>time</timeStamp>'
    stype=71
    clid=0
    tname=''
    if (datatype=='CondAttrListCollection'):
        clid=1238547719
        tname='CondAttrListCollection'
    elif (datatype=='AthenaAttributeList'):
        clid=40774348
        tname='AthenaAttributeList'
    desc+='<addrHeader><address_header service_type=\"'+str(stype)+'\" clid=\"'+str(clid)+'\" /></addrHeader><typeName>'+tname+'</typeName>'
    return desc

def timeVal(val):
    "Convert argument to time in seconds, treating as a literal or date"
    try:
        a=int(val)
        return a
    except ValueError:
        try:
            ts=time.strptime(val+'/UTC','%Y-%m-%d:%H:%M:%S/%Z')
            return int(calendar.timegm(ts))
        except ValueError,e:
            print "ERROR in time specification, use e.g. 2007-05-25:14:01:00"
            sys.exit(-1)
        
def timeString(iovkey):
    "Convert the IOVkey (63 bit) to a string representing timestamp"
    if (iovkey==cool.ValidityKeyMin):
        return "ValidityKeyMin"
    elif (iovkey==cool.ValidityKeyMax):
        return "ValidityKeyMax"
    else:
        stime=int(iovkey/1000000000L)
        return time.asctime(time.gmtime(stime))+" UTC"

def indirectOpen(coolstr,readOnly=True,oracle=False,debug=False):
    """Obtain a connection to the database coolstr (e.g.
       COOLONL_INDET/OFLP200) using Oracle servers only, bypassing SQLite files
       and simulating the action of DBReplicaSvc to choose the correct one.
       Returns None in case a connection cannot be established.
       debug=True produces debug printout to show servers being tried"""

    dbSvc=cool.DatabaseSvcFactory.databaseService()
    connstr=transConn(coolstr)
    
    # split the name into schema and dbinstance
    splitname=connstr.split('/')
    forceSQLite='ATLAS_COOL_FORCESQLITE' in os.environ
    if (debug and forceSQLite):
        print "ATLAS_COOL_FORCESQLITE: Force consideration of SQLite replicas"
    if (len(splitname)!=2 or readOnly==False or oracle==False or forceSQLite):
        try:
            db=dbSvc.openDatabase(connstr,readOnly)
        except Exception,e:
            # if writeable connection on readonly filesystem, get
            # 'cannot be established error' - retry readonly
            if (not readOnly and ('attempt to write a readonly database' in e.__repr__())):
                print "Writeable open failed - retry in readonly mode"
                db=dbSvc.openDatabase(connstr,True)
            else:
                raise
        return db
    if (debug): print "Trying to establish connection for %s from Oracle" % connstr
    schema=splitname[0]
    dbinst=splitname[1]
    # list of servers to try, first one at beginning
    serverlist=replicaList()
    if (debug): print serverlist
    # will pop candidates from end of list - so reverse it first
    serverlist.reverse()
    while len(serverlist)>0:
        server=serverlist.pop()
        # deal with frontier server - use only if FRONTIER_SERVER is set
        if server=='ATLF':
            if ('FRONTIER_SERVER' in os.environ):
                connstr='frontier://ATLF/();schema=ATLAS_%s;dbname=%s' % (schema,dbinst)
            else:
                # skip ATLF replica if FRONTIER_SERVER not set
                continue
        elif server=='atlas_dd': continue
        else:
            connstr='oracle://%s;schema=ATLAS_%s;dbname=%s' % (server,schema,dbinst)
        if (debug): print "Attempting to open %s" % connstr
        try:
            dbconn=dbSvc.openDatabase(connstr)
            if (dbconn is not None and dbconn.isOpen()):
                if (debug): print "Established connection to %s" % server
                return dbconn
            if (debug): print "Cannot connect to %s" % server
        except Exception,e:
            if (debug): print "Exception connecting to %s" % server
    # all replicas tried - give up
    print "All available replicas tried - giving up"
    return None

def replicaList():
    """Return list of Oracle database server names to try, mimicing action of
    Athena DBReplicaSvc"""
    configfile=pathResolver('dbreplica.config')
    if configfile is None:
        print "Cannot find dbreplica.config"
        return None
    hostname=getHostname()
    best=0
    serverlist=[]
    for line in configfile.readlines():
        epos=line.find('=')
        if (epos>0 and line[0]!="#"):
            domains=line[0:epos].split()
            for dom in domains:
                i=hostname.find(dom)
                if ((hostname[-len(dom):]==dom and len(dom)>best) or (best==0 and dom=='default')):
                    best=len(dom)
                    serverlist=line[epos+1:].split()
    configfile.close()
    if (len(serverlist)==0):
        print "No suitable servers found"
        return None
    return serverlist
            
def pathResolver(leaf,retFile=True):
    """Find the file leaf using the DATAPATH variable to look for it
    Return a read-only file object, or None if not found.
    If retFile is false, just return the pathname.
    Current directory takes precendence if file is there"""
    try:
        paths=os.environ['DATAPATH'].split(':')
    except Exception:
        # DATAPATH not set - just use current directory
        paths=[]
    paths=['.']+paths
    for path in paths:
        try:
            s=os.stat(path+'/'+leaf)
            if (retFile):
                return open(path+'/'+leaf,'r')
            else:
                return path+'/'+leaf
        except OSError:
            pass
    return None

def getHostname():
    "Get the hostname including domain name if possible"
    if ('ATLAS_CONDDB' in os.environ):
        name=os.environ['ATLAS_CONDDB']
    else:
        try:
            name=os.environ['HOSTNAME']
        except Exception:
            # try socket lib (for Mac) as HOSTNAME does not contain os.environ
            import socket
            try:
                name=socket.getfqdn()
            except Exception:
                name='unknown'
    # check name has a . (and doesn't end with .local, as macs default to), 
    # otherwise try command hostname --fqdn
    if (name.find('.')>=0 and name[-6:]!=".local"): 
        return name
    else:
        if (name[-6:]==".local"):
            print "WARNING. Hostname is ",name, " which does not define a domain. Consider setting $ATLAS_CONDDB to avoid this"
            return "unknown"
        
    try:
        name=commands.getoutput('hostname --fqdn')
    except Exception:
        name='unknown'
    
    #Calling hostname wrong will fill 'name' with error message
    if (name.find('illegal')>-1 or name.find('unrecognized')>-1):
      print "WARNING. hostname --fqdn returned the following:",name
      print "Consider setting $ATLAS_CONDDB to avoid this."
      return "unknown"
    return name
    

# test code
def tests():
    print 'AtlCoolLib tests'
    db=forceOpen("TESTCOOL")
    if (db is None):
        print 'Could not create test database'
        sys.exit(1)
    spec=cool.RecordSpecification()
    spec.extend('testString',cool.StorageType.String255)
    folder=ensureFolder(db,'/TEST/TEST1',spec,athenaDesc(True,'CondAttrListCollection'),cool.FolderVersioning.MULTI_VERSION)
    if (folder is None):
        print 'Could not create test folder'
        sys.exit(1)
    print 'All done'
    return

class coolTool:
    """
    Class coolTool implements a base for python command-line tools to access
    and set data in COOL folders
    Incomplete baseclass implementation - clients must implement the following:
     - setup(self,args) - set additional arguments
     - usage(self) - print usage
     - execute(self) - execute command
     - procopts(self,opts) - (optional) - process additional command switches
    """
    def __init__(self,name,readonly,minarg,maxarg,longopts):
        """
        Initialise class and process command line options and switches
        """
        self.name=name
        self.readonly=readonly
        self.runLumi=True
        self.runmin=0
        self.runmax=(1 << 31)-1
        self.lumimin=0
        self.lumimax=(1 << 32)-2
        self.tsmin=cool.ValidityKeyMin
        self.tsmax=cool.ValidityKeyMax
        self.since=cool.ValidityKeyMin
        self.until=cool.ValidityKeyMax
        self.oracle=False
        self.debug=False
        # get and process options - note only use long options format
        try:
            fullopts=longopts+['r=','rs=','ru=','l=','ls=','lu=','ts=','tu=','readoracle','debug']
            opts,args=getopt.getopt(sys.argv[1:],'',fullopts)
        except getopt.GetoptError, e:
            print e
            self.usage()
            sys.exit(1)
        if len(args)<minarg or len(args)>maxarg:
            print name,'takes between',minarg,'and',maxarg,'non-optional parameters'
            self.usage()
            sys.exit(1)
        # set standard parameters
        self.conn=transConn(str(args[0]))
        # allow derived class to set parameters
        self.setup(args[1:])
        self._procopts(opts)
        self.procopts(opts)
        # open database connection and execute command
        if self.readonly:
            self.db=indirectOpen(self.conn,oracle=self.oracle,debug=self.debug)
        else:
            self.db=forceOpen(self.conn)
        self.execute()

    def _usage1(self):
        # base implementation of usage - first part
        print 'Usage:',self.name,' {<options>} dbname ',
        
    def _usage2(self):
        # base implementation of usage - second part
        print 'Options to specify IOV (default valid for all intervals)'
        print '--rs=<first run>'
        print '--ru=<last run> (inclusive)'
        print '--r=<run> (only single run)'
        print '--ls=<lumi block since>'
        print '--l=<lumi block> (only single LB)'
        print '--lu=<lumi block until> (inclusive)'
        print '--ts=<initial timestamp> (in seconds)'
        print '--tu=<final timestamp> (in seconds)'
        print '--readoracle: Force read-only queries to use Oracle replica'
        print '--debug: Enable debugging information'

    def _procopts(self,opts):
        # process the standard parameters
        for o,a in opts:
            if (o=='--rs'): self.runmin=int(a)
            if (o=='--ru'): self.runmax=int(a)
            if (o=='--r'):
                self.runmin=int(a)
                self.runmax=int(a)
            if (o=='--ls'): self.lumimin=int(a)
            if (o=='--lu'): self.lumimax=int(a)
            if (o=='--l'):
                self.lumimin=int(a)
                self.lumimax=int(a)
            if (o=='--ts'):
                self.tsmin=timeVal(a)*1000000000L
                self.runLumi=False
            if (o=='--tu'):
                self.tsmax=timeVal(a)*1000000000L
                self.runLumi=False
            if (o=='--readoracle'): self.oracle=True
            if (o=='--debug'): self.debug=True
        # now set the real interval of validity
        if (self.runLumi):
            print '>== Data valid for run,LB [',self.runmin,',',self.lumimin,'] to [',self.runmax,',',self.lumimax,']'
            self.since=(self.runmin << 32)+self.lumimin
            self.until=(self.runmax << 32)+self.lumimax+1
        else:
            self.since=self.tsmin
            self.until=self.tsmax
            print '>== Data valid for timestamps (sec) from ',self.since/1000000000L,'(',timeString(self.since),') to ',self.until/1000000000L,'(',timeString(self.until),')'
        # check IOV is OK
        if (self.until<self.since):
            print 'ERROR in IOV definition: until<since'
            sys.exit(1)

    def procopts(self,opts):
        # default implementation of procopts - do nothing
        pass

class coolWrapper:
    """
    comment
    """
    def __init__(self,db,folder,spec=[],version=cool.FolderVersioning.SINGLE_VERSION):
        # setup the db reference
        # if its a string, try to open it
        if (type(db)==str):
            self.db=forceOpen(db)
        else:
            # assume its an already existing database object
            self.db=db
        self.folder=folder
        self.spec=spec
        self.version=version
        # check the database is open
        if (not self.db.isOpen()):
            raise 'Database not open'
        

    def storeDataIOV(self,data,since=cool.ValidityKeyMin,
                     until=cool.ValidityKeyMax,channel=0,tag=''):
        if (type(channel)==int):
            chanid=channel
        elif (type(channel)==string):
            # lookup channel
            print 'channel is string'

    def storeDataRun(self,data,run,channel=0,tag=''):
        since=(run << 32L)
        until=((run+1) << 32L)
        storeDataIOV(since,until,channel,data,tag)

class RangeList:
    """Hold a list of IOVs (start/end pairs) which are good, allowing parts
    of list to be vetoed. start/end interpreted in COOL convention"""
    def __init__(self,start,end):
        "Initalise RangeList with given start and end (end=1 after last valid)"
        self._starts=[start]
        self._ends=[end]

    def vetoRange(self,start,end):
        "Veto part of the original range, splitting it if needed"
        if (start>=end): return
        if (len(self._starts)==0): return
        ix=0
        while (ix<len(self._starts) and end>self._starts[ix]):
            if (end>=self._starts[ix]):
                if (start<=self._starts[ix] and end>=self._ends[ix]):
                    # remove whole interval
                    self._remove(ix)
                    ix-=1
                elif (start<=self._starts[ix] and end<self._ends[ix]):
                    # remove front of stored interval
                    self._starts[ix]=end
                elif (start<=self._ends[ix] and end>=self._ends[ix]):
                    # remove back of stored interval
                    self._ends[ix]=start
                elif (start>self._starts[ix] and end<self._ends[ix]):
                    # have to split the stored interval
                    oldstart=self._starts[ix]
                    oldend=self._ends[ix]
                    self._ends[ix]=start
                    self._insert(ix+1,end,oldend)
                    ix+=1
            ix+=1

    def getAllowedRanges(self,start,end):
        """Return a list of tuples giving the allowed (start,end) within the
        specified (start,end)"""
        result=[]
        for ix in range(0,len(self._starts)):
            if (self._ends[ix]>start and self._starts[ix]<end):
                result+=[(max(self._starts[ix],start),min(self._ends[ix],end))]
        return result

    def __str__(self):
        "Print representation of range as list of [x,y] allowed values"
        rep=''
        for i in range(0,len(self._starts)):
            rep+='[%i,%i] ' % (self._starts[i],self._ends[i])
        return rep

    def _remove(self,idx):
        "Remove the entry at idx"
        self._starts[idx:]=self._starts[1+idx:]
        self._ends[idx:]=self._ends[1+idx:]

    def _insert(self,idx,start,end):
        "Put a new entry at idx, moving others to make space"
        self._starts.insert(idx,start)
        self._ends.insert(idx,end)

class TimeStampToRLB:
    "Translation between timestamps and RunLB IOVs using /TRIGGER/LUMI/LBLB"
    # contribution from Jahred Adelman
    def __init__(self,dbconn,iovstart,iovend):
        "Initialise and cache using the given DB connection, for RLB start/end"
        # dbconn must correspond to COOLONL_TRIGGER/COMP200
        self.StartTime = -1
        self.EndTime = -1
        self.since=iovstart
        self.until=iovend
        self.TSBeginMap = []
        self.TSEndMap = []
        self.RLMap = []		
        self.readdb = dbconn

        lblbname='/TRIGGER/LUMI/LBLB'
        try:
            readfolder=self.readdb.getFolder(lblbname)
        except Exception,e:
            print e
            print "Could not access folder %s " % lblbname
            raise RuntimeError,"TimeStampToRLB initialisation error"
        # First try to read timestamp info            
        isFirst=True
        try:
            readobjs=readfolder.browseObjects(self.since,self.until,cool.ChannelSelection.all());
            while readobjs.goToNext():
                readobj=readobjs.currentRef()
                payload=readobj.payload()
                if (isFirst == True):
                    isFirst=False
                    self.StartTime=payload['StartTime']
                else:
                    self.EndTime = payload['EndTime']
        except Exception,e:
            print e
            print "Problem reading data from folder %s" % lblbname
            raise RuntimeError,"TimeStampToRLB: initialisation error"
        if (self.StartTime==-1):
            raise RuntimeError,"TimeStampToRLB: no data for given runs"
        
        # Now try to read the LBTIME folder to translate timestamps into run/lumi blocks
        lbtimename='/TRIGGER/LUMI/LBTIME'
        try:
            readfolder=self.readdb.getFolder(lbtimename)
        except Exception,e:
            print e
            print "Problem accessing folder %s" % lbtimename
            raise RuntimeError,"TimeStampToRLB: Initialisation error"
        try:
            readobjs=readfolder.browseObjects(self.StartTime, self.EndTime, cool.ChannelSelection.all());
            while readobjs.goToNext():
                readobj=readobjs.currentRef()
                payload=readobj.payload()
                TimeStampStart = readobj.since()
                TimeStampEnd = readobj.until()
                iov=(payload['Run'] << 32)+payload['LumiBlock']
                self.TSBeginMap+=[TimeStampStart]
                self.TSEndMap+=[TimeStampEnd]
                self.RLMap+=[iov]
        except Exception, e:
            print e
            print "Problem reading from folder %s" % lbtimename
            raise RuntimeError,"TimeStampToRLB: Time data access error"
        print "TimeStampToRLB initialised with %i entries in map" % len(self.RLMap)

    def getRLB(self,timestamp,StartType=True):
        """Lookup a timestamp value. If it is outside a run, round up to next
run (StartType=True) or down to previous (StartType=False)"""

        # New version. Let's take advantage of the fact that the DB entries should be time-ordered
        if (StartType):
            for TSbegin, RL in zip(self.TSBeginMap, self.RLMap):
                if (timestamp <= TSbegin):
                    return RL
            # Timestamp above cached endtime - return endtime
            return self.until
                
        else:
            for TSend, RL in reversed(zip(self.TSEndMap, self.RLMap)):
                if (timestamp >= TSend):
                    return RL
            # Timestamp below cached starttime - return starttime
            return self.since


    
# main code - run test of library functions
if __name__=='__main__':
    tests()
    print 'All OK'
    
