#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
import sys,os,traceback,time,commands,getopt
from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen

def testRun(globaltag,run,tofs,nevt,deltat,configfile,debug=0):
    "Execute the test run with given parameters"
    print "Simulate tag %s run %i with %i events time increment %i" % (globaltag,run,nevt,deltat)

    starttime=time.time()
    # find run start time from SORParams folder
    tdaqDB=indirectOpen('COOLONL_TDAQ/COMP200',oracle=True)
    if (tdaqDB is None):
        print "ERROR: Cannot connect to COOLONL_TDAQ/COMP200"
        return 1
    sortime=0
    try:
        tdaqfolder=tdaqDB.getFolder('/TDAQ/RunCtrl/SOR_Params')
        runiov=run << 32
        obj=tdaqfolder.findObject(runiov,0)
        payload=obj.payload()
        sortime=payload['SORTime']/1.E9+tofs
    except Exception,e:
        print "ERROR accessing /TDAQ/RunCtrl/SOR_Params"
        print e
    tdaqDB.closeDatabase()
    print "Starttime is %i (%s)" % (sortime,time.asctime(time.gmtime(sortime)))

    # now run Athena job
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
from AthenaCommon.GlobalFlags import globalflags 
globalflags.DetGeo.set_Value_and_Lock('atlas') 
globalflags.DataSource.set_Value_and_Lock('data')
from IOVDbSvc.CondDB import conddb
conddb.iovdbsvc.OutputLevel=%i
conddb.iovdbsvc.GlobalTag='%s'
        
# setup DBReplicaSvc
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

# include file to set FolderList
include('%s')
# remove some folders known to cause trouble (as DetDescr not loaded)
badfolders=['/LAR/Identifier/OnOffIdAtlas','/LAR/Identifier/FebRodAtlas','/LAR/Identifier/LArTTCellMapAtlas','/CALO/Identifier/CaloTTOnOffIdMapAtlas','/CALO/Identifier/CaloTTOnAttrIdMapAtlas']

for i in range(0,len(FolderList)):
  folder=FolderList[i]
  ignore=False
  for j in badfolders:
    if folder.find(j)>=0: ignore=True
  if (ignore):
    print "Suppressing folder",folder
  else:
    svcMgr.IOVDbSvc.Folders+=[folder]
print "Processing",len(svcMgr.IOVDbSvc.Folders),"of",len(FolderList),"folders"

""" % (iovsvcdbg,globaltag,run,sortime,deltat,nevt,configfile) )

    jofile.close()

    comm='athena.py condtestjobopts.py '
    if (debug==0):
        comm+=' >& log'
    else:
        comm+=' |tee log'
    print 'About to execute',comm
    # execute Athena job 
    athenarc=os.system(comm)
    # athenarc=0
    print 'Return code from athena=',athenarc

    # analysis of job based on status codes from Tier-0 test
    retcode=0
    # tail logfile if we did not print whole thing
    if (debug==0):
        print 'End of logfile follows ...'
        (s,o)=commands.getstatusoutput('tail -n 200 log')
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
        print "Script for running COOL test workloads based on real conditions data and reconstruction jobs"
        print "Usage",sys.argv[0],'{<options>} <configfile.py> <globaltag> <run> {<timeoffset> <nevt> <deltat>}'
        print "<configfile.py> gets included and should define FolderList"
        print "<globaltag> specifies the global conditions tag"
        print "<run> specifies the run number to be used when retrieving"
        print "<timeoffset> specifies an offset (in seconds) into the run for DCS data"
        print "<nevt> can be used to process >1 event (defaut 1)"
        print "<deltat> specifies the time advance between events (in seconds)"
        print "Options are:"
        print "--help : Print this help"
        print "--debug=<level> : Enable full log file (1) + IOVDbSvc debug (2)"
        print
        print "Example:",sys.argv[0],'--debug 1 ../config/Tier0Cosmic2.py COMCOND-ES1C-000-00 91389'
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
    if len(args)<3:
        usage()
        sys.exit(-1)
    configfile=str(args[0])
    globaltag=str(args[1])
    run=int(args[2])
    tofs=0
    nevt=1
    deltat=1
    if len(args)>3:
        tofs=int(args[3])
    if len(args)>4:
        nevt=int(args[4])
    if len(args)>4:
        deltat=int(args[5])
    retcode=testRun(globaltag,run,tofs,nevt,deltat,configfile,debug)
    sys.exit(retcode)
