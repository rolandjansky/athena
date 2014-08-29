#!/usr/bin/env python
# AtlCoolMakeReplica.py - setup an SQLite replica for a particular run
# Richard Hawkings, 29/1/08

import os,sys
from CoolConvUtilities.AtlCoolSchemaLib import coolSchema,instanceList

def MakeReplica(dbname,tag,destdb,run1,run2,excludes=[],extratags=[],
                geo=[],parfiles=[],user='',passwd=''):
    print ">>> Construct replica of %s tag %s for runs %i to %i" % (dbname,tag,run1,run2)
    print ">>> Destination database: %s" % destdb
    # check if AtlCoolCopy is available
    print "Checking for AtlCoolCopy.exe availability ..."
    noenv=(os.system('which AtlCoolCopy.exe')!=0)
    if (noenv):
        print "Cannot find AtlCoolCopy.exe - will attempt to setup release as needed"
        
    # get list of schema to process
    dblist=instanceList(instanceSel=dbname)
    nbad=0
    print ">>> Beginning replication of %i database schema" % len(dblist)
    excludelist=''
    for i in excludes:
        print ">>> Exclude folder pattern %s" % i
        excludelist+=' -exclude %s' % i
    for i in extratags:
        print ">>> Extra tag to be included %s" % i
        excludelist+=' -tag %s' % i
    for db in dblist:
        schema=db.schema()
        sourcedb="oracle://ATLAS_COOLPROD;schema=%s;dbname=%s" % (schema,dbname)
        expfolder=''
        # ugly hack to avoid copying /GLOBAL/DETSTATUS/* from online schema
        if (schema=='ATLAS_COOLONL_GLOBAL'):
            expfolder+=' -exclude /GLOBAL/DETSTATUS'
        # extra bits of command depending on options
        appendcom=''
        for i in geo:
            print 'Geometry pattern to be included %s' % i
            appendcom+=' -magic %s' % i
        for i in parfiles:
            print 'Extra parameters file %s' % i
            appendcom+=' -parfile %s' % i
        # if COMP200, need to set the run start/end times as well
        if (dbname=='COMP200' or dbname=='CONDBR2'): appendcom+=' -gettime'
        if (user!=''):
            sourcedb+=';user=%s;password=%s' % (user,passwd)
        command='AtlCoolCopy.exe \"%s\" \"%s\" -create -readoracle -hitag -prunetags -checkdest -tag %s -rs %i -ru %i %s %s %s' % (sourcedb,destdb,tag,run1,run2,excludelist,expfolder,appendcom)
        print ">>> Execute copy for schema %s" % schema
        print command
        rc=os.system(command)
        print ">>> Schema %s copied with return code %i" % (schema,rc)
        if (rc!=0): nbad+=1
    print ">>> All done with %i bad replications" % nbad
    if (nbad>0):
        return 1
    else:
        return 0

if __name__=='__main__':
    import getopt
    def usage():
        print "Usage",sys.argv[0],"<dbinstance> <tag> <destination db>"
        print "Options are"
        print "--r=<run> : Specify single run"
        print "--rs=<runsince> : Specify first run"
        print "--ru=<rununtil> : Specify last run (inclusive)"
        print "--exclude=<folder> : Specify folder pattern(s) to exclude"
        print "--extra=<tag> : Specify extra tags to force-include"
        print "--geo=<geometry> : Geometry to consider (default ATLAS- i.e. all)"
        print "--parfile=<file> : File of options to pass to AtlCoolCopy"
        print "--user=<username> : Specify non-default username"
        print "--pwd=<password> : Specify associated password"

    # main code - process command line options
    try:
        longopts=['r=','rs=','ru=','user=','pwd=','exclude=','extra=',
        'parfile=','geo=']
        opts,args=getopt.getopt(sys.argv[1:],'',longopts)
    except getopt.GetoptError,e:
        print e
        usage()
        sys.exit(-1)
    if len(args)<3:
        usage()
        sys.exit(-1)
    dbname=str(args[0])
    toptag=str(args[1])
    destdb=str(args[2])
    run1=0
    run2=(1L << 31)-1
    excludes=[]
    extratags=[]
    geo=[]
    parfiles=[]
    user=''
    passwd=''
    for o,a in opts:
        if (o=='--help'):
            self.usage()
            sys.exit(0)
        if (o=='--r'):
            run1=int(a)
            run2=run1
        if (o=='--rs'):
            run1=int(a)
        if (o=='--ru'):
            run2=int(a)
        if (o=='--exclude'):
            excludes+=[str(a)]
        if (o=='--extra'):
            extratags+=[str(a)]
        if (o=='--geo'):
            geo+=[str(a)]
        if (o=='--parfile'):
            parfiles+=[str(a)]
        if (o=='--user'):
            user=str(a)
        if (o=='--pwd'):
            passwd=str(a)
    # consider all geometry magic tags if none given
    if len(geo)==0: geo=['ATLAS-']
    rc=MakeReplica(dbname,toptag,destdb,run1,run2,excludes,extratags,geo,parfiles,user,passwd)
    sys.exit(rc)
