#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
import sys,os,traceback,time,commands,getopt

def testRun(dbid,run,nevt,deltat,configfiles,debug=0):
    "Execute the test run with given parameters"
    if dbid=="":
        dbid='logical:X:ATLAS_COOLONL_GLOBAL:STRM200'
    starttime=time.time()
    time0=run*3600
        
    print ">== Data will be read from database connection",dbid
    # generate configuration for folders
    cfiles=[]
    i=0
    for configfile in configfiles:
        print '>==  Generate config for file',configfile
        comm='TestCoolRecWriter.exe \"'+dbid+'\" 2 '+configfile
        print 'Execute command:',comm
        rc=os.system(comm)
        if (rc!=0):print 'Non-zero return code',rc,'from TestCoolRecWriter'
        # add suffix CoolReadConfig.py file
        filen='CoolReadConfig_'+str(i)+'.py'
        os.system('mv CoolReadConfig.py %s' % filen)
        cfiles+=[ filen ]
        i+=1
    
    # now run Athena job
    print cfiles
    iovsvcdbg=3
    if (debug>=2): iovsvcdbg=2
    
    jofile=open('condtestjobopts.py','w')
    jofile.write("""import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AlgSequence import AlgSequence
topSequence=AlgSequence()
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.AthenaPool

#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc
#--------------------------------------------------------------
import IOVDbSvc.IOVDb
svcMgr.IOVDbSvc.OutputLevel        = %i
svcMgr.IOVDbSvc.ManageConnections=False

# setup DBReplicaSvc
PoolSvc=Service("PoolSvc")
PoolSvc.SortReplicas=True
from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
svcMgr+=DBReplicaSvc(UseCOOLSQLite=False)

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaDBTestRec.AthenaDBTestRecConf import TestCoolRecRead
theTestCoolRecRead=TestCoolRecRead(OutputLevel=INFO)
topSequence+=theTestCoolRecRead
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
# and timestampsjob
svcMgr.EventSelector.RunNumber         = %i
svcMgr.EventSelector.EventsPerRun      = 999999
svcMgr.EventSelector.FirstEvent        = 0
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
svcMgr.EventSelector.InitialTimeStamp  = %i
# increment of 3 minutes
svcMgr.EventSelector.TimeStampInterval = %i
theApp.EvtMax                   = %i

svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.debugLimit        = 100000

#
""" % (iovsvcdbg,run,time0,deltat,nevt) )

    jofile.close()

    comm='athena.py condtestjobopts.py '
    for i in cfiles: comm+=i+' '
    if (debug==0):
        comm+=' >& log'
    else:
        comm+=' |tee log'
    print 'About to execute',comm
    # execute Athena job 
    athenarc=os.system(comm)
    print 'Return code from athena=',athenarc

    # analysis of job based on status codes from Tier-0 test
    retcode=0
    # tail logfile if we did not print whole thing
    if (debug==0):
        print 'End of logfile follows ...'
        (s,o)=commands.getstatusoutput('tail -n 100 log')
        print o
    if (athenarc!=0):
        retcode=62600
    else:
        # extract summary line from logfile
        (s,msg)=commands.getstatusoutput('grep TestCoolRecReadSummary log')
        opmsg=msg[msg.find('TestCoolRecReadSummary:')+24:]
        print 'Extracted output message:',opmsg
        if (opmsg!='All folders OK'):
            retcode=65000

    print '>== All done, return code',retcode,'time taken is',time.time()-starttime
    return retcode

if __name__=='__main__':
    def usage():
        print "Script for running COOL test workloads"
        print "Usage",sys.argv[0],'{<options>} \"DbIbString\" <run> <nevt> <deltat> <config1> { <config2> ...}'
        print "Options are:"
        print "--help : Print this help"
        print "--debug=<level> : Enable full log file (1) + IOVDbSvc debug (2)"
        sys.exit(-1)
    try:
        longopts=['debug=','help']
        opts,args=getopt.getopt(sys.argv[1:],'',longopts)
    except getopt.GetoptError,e:
        print e
        usage()
        sys.exit(-1)
    debug=0
    # process optional arguments
    for o,a in opts:
        if (o=='--help'):
            usage()
            sys.exit(0)
        if (o=='--debug'):
            debug=int(a)
    # check there are enough arguments
    if len(args)<5:
        usage()
        sys.exit(-1)
    dbid=args[0]
    if (dbid==""):
        dbid='logical:X:ATLAS_COOLONL_GLOBAL:STRM200'
    run=int(args[1])
    nevt=int(args[2])
    deltat=int(args[3])
    retcode=testRun(dbid,run,nevt,deltat,args[4:],debug)
    sys.exit(retcode)
