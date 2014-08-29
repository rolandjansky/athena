#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
  check db has all needed host information

"""

import os
import shutil
import datetime as dt
from sqlite3 import IntegrityError, OperationalError

def getHost(hostname=None):
    if hostname is None:
        hostname=os.getenv('HOSTNAME')
    #hostname='lxbsu2921.cern.ch'
    #hostname1='lxbsu2921a.cern.ch'
    hout=os.popen('lshosts '+hostname)
    res=hout.read()
    hout.close()
    res2=[l.split() for l in res.strip().split('\n')]
    res3=[ l[:8]+[' '.join(l[8:])] for l in res2 ]
    return res3

class DataBase(object):
    def __init__(self,fname):
        self.fname=fname
        self.cnx=None
        self.cur=None

    def openConnection(self):
        import sqlite3
        self.cnx = sqlite3.connect(self.fname)
        self.cur = self.cnx.cursor()

    def _execute(self,cmd,args=()):
        print 'SQL:',cmd,args
        self.cur.execute(cmd,args)

    def _dumpTable(self,t):
        #t = (symbol,)
        #cur.execute('select * from stocks where symbol=?', t)
        self._execute('select * from %s'%t)
        header= [ s[0] for s in self.cur.description ]
        format= ' '.join([ '%25s' for s in self.cur.description])
        print format%tuple(header)
        for res in self.cur.fetchall():
            print format%res


    def insert(self,tname='t_run',hargs=[],args=[]):
        "create prepared statement and execute"
        keys=','.join(hargs)
        vals=','.join(('?',)*len(args))
        cmd="insert into %s (%s) values (%s)"%(tname,keys,vals)
        print cmd,args
        self.cur.execute(cmd,args)
        self.rid=self.cur.lastrowid
        return self.rid

    def dummyHostInfo(self):
        hostInfo={'HOST_NAME': 'lxplus443',
                  'RESOURCES': '(intel plus quadcore wan)',
                  'cpuf': '1.9',
                  'maxmem': '48161M',
                  'maxswp': '51207M',
                  'model': 'vi_10_24',
                  'ncpus': '8',
                  'server': 'Yes',
                  'type': 'SLC5_64'}
        return hostInfo

    def addRun(self):

        #AtlasArea=/afs/cern.ch/atlas/software/builds/nightlies/devval/AtlasProduction/rel_2
        atlasArea=os.getenv('AtlasArea')
        dummy,branch,proj,rel=atlasArea.rsplit('/',3)

        #LS_SUBCWD=/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/rel_2/devval/build/x86_64-slc5-gcc43-opt/offline/SimCoreTests/AtlasG4_muons
        subcwd=os.getenv('LS_SUBCWD')
        dummy1,cmt,dummy2,package,job=subcwd.rsplit('/',4)

        rdate,rtime=dt.datetime.now().strftime('%Y-%m-%d %H:%M:%S').split()

        hostInfo=getHost()
        hostInfo=dict(zip(*hostInfo))

        machine=hostInfo['HOST_NAME']
        KSI2K_fac=float(hostInfo['cpuf'])
        hargs=['branch', 'rel', 'cmt', 'rdate', 'rtime', 'machine', 'KSI2K_fac', 'package', 'job']
        args=[locals()[x] for x in hargs]
        id=self.insert('t_run',hargs,args)
        self.cnx.commit()
        return id

    def addFile(self,fl):
        "add file entry to db and return id"
        # check if already registered
        if fl.dbid is not None:
            return fl.dbid

        # check if database
        cmd='select id from t_files where fl_name=?'
        self._execute(cmd,(fl.fl,))
        res=self.cur.fetchone()
        if res is None:
            print 'WARNING',fl.fl,'not found'
            # adding it
            id=self.insert('t_files',['fl_name'],[fl.fl])
        else:
            # receive existing entry
            id=res[0]
        fl.dbid=id
        return id

    def addFunction(self,fn):
        if self.parser.filelist.has_key(fn.fl_id):
            fl=self.parser.filelist[fn.fl_id]
            fid=self.addFile(fl)
        else:
            fid=0
        cmd='select id from t_functions where fn_name=? and fid=? and issue=?'
        self._execute(cmd,(fn.fn,fid,fn.issue))
        res=self.cur.fetchone()
        if res is None:
            # adding it
            hargs=['fn_name','fid','issue','lib']
            args=[fn.fn,fid,fn.issue,fn.lib]
            id = self.insert('t_functions',hargs,args)
        else:
            id = res[0]
        fn.dbid=id
    
    def addFunctions(self,parser):
        self.parser=parser
        for fn in parser.functions.values():
            self.addFunction(fn)
        self.cnx.commit()

    def addCountAndLinks(self,rid,parser=None):
        if parser!=None:
            self.parser=parser

        success=True
        for fn in parser.functions.values():
            if fn.dbid is None:
                print 'ERROR fn=',fn.fn,'not in db yet'
                self.addFunction(fn)

            hargs=['rid','fid','selfcounts','totalcounts']
            args=[rid,fn.dbid,fn.selfcost,fn.totalcost]
            # add counts to db
            try:
                self.insert('t_counts',hargs,args)
            except IntegrityError as detail:
                print 'IntegrityError',detail
                success=False
                print 'fn=',fn
                continue

            # add sub call links to db
            for i,counts in fn.calls.iteritems():
                if i!=0:
                    cfn=self.parser.functions[i]
                    if cfn.dbid is None:
                        print 'ERROR cfn=',cfn.fn,'not in db yet'
                        self.addFunction(cfn)
                    hargs=['rid','fid','cfunid','counts']
                    args=[rid,fn.dbid,cfn.dbid,counts]
                    try:
                        self.insert('t_links',hargs,args)
                    except IntegrityError as detail:
                        print 'IntegrityError',detail
                        success=False
                        print 'fn=',fn
                        print 'cfn=',cfn
                        
        if success:
            self.cnx.commit()
        else:
            print 'some ERROR occured rolling back'
            self.cnx.rollback()


if __name__=="__main__":
    # create input files (callgrind.out & athena.profile.txt if not present already
    #db=DataBase('rtt.sqlite')
    doTest=False
    if doTest:
        db=DataBase('./rttProfile.sqlite')
    else:
        db=DataBase('/afs/cern.ch/atlas/groups/Simulation/rtt/rttProfile.sqlite')
        
    db.openConnection()

    cmd="select distinct machine from t_run"
    db._execute(cmd)
    res = db.cur.fetchall()
    ll=list(zip(*res)[0])


    try:
        db._execute("select * from t_hosts");
        db.cur.fetchall()
    except OperationalError as detail:
        print detail
        print "table t_hosts not found: try create one"
        db.cnx.rollback()
        cmd="create table t_hosts (id integer primary key autoincrement, hostname varchar(32), os_type varchar(32), model varchar(32), cpuf float, ncpus integer , maxmem varchar(32), maxswp varchar(32), server varchar(6), resources text)"
        db._execute(cmd);
        db.cnx.commit()
    
    for hostname in ll:
        print 'check',hostname
        #info = getHost(hostname)
        #print info

        cmd='select id from t_hosts where hostname=?'
        db._execute(cmd,(hostname,))
        res=db.cur.fetchone()
        if res is None:
            # add to database
            print hostname,"not found, adding it now"

            hargs=['hostname', 'os_type', 'model', 'cpuf', 'ncpus', 'maxmem', 'maxswp', 'server', 'resources']
            bargs=['HOST_NAME', 'type', 'model', 'cpuf', 'ncpus', 'maxmem', 'maxswp', 'server', 'RESOURCES']
            hostInfo=getHost(hostname)
            hostInfo=dict(zip(*hostInfo))
            hostInfo['maxmem']=int(hostInfo['maxmem'].strip('M'))
            hostInfo['maxswp']=int(hostInfo['maxswp'].strip('M'))
            args=[hostInfo[h] for h in bargs]
            
            db.insert('t_hosts',hargs,args)
        db.cnx.commit()
        #
        #int(x.strip('M'))
