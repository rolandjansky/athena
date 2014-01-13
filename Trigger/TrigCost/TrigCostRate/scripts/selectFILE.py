#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## Run Table SFO_TZ_RUN
##
## "SFOID"        "VARCHAR2(32 Bytes)"
## "RUNNR"        "NUMBER(10,0)"
## "STREAMTYPE"   "VARCHAR2(32 Bytes)"
## "STREAM"       "VARCHAR2(32 Bytes)"
## "STATE"        "VARCHAR2(32 Bytes)" [OPENED,CLOSED,TRANSFERRED]
## "TZSTATE"      "VARCHAR2(32 Bytes)" filled by TZ
## "DSNAME"       "VARCHAR2(256 Bytes)"
## "T_STAMP"      "TIMESTAMP(0)"  "RUN_SEQNUM"    "NUMBER(10,0)"


## File Table SFO_TZ_File
##
## "LFN"          "VARCHAR2(256 Bytes)" filename
## "FILENR"       "NUMBER(10,0)"
## "SFOID"        "VARCHAR2(32 Bytes)"
## "RUNNR"        "NUMBER(10,0)"
## "LUMIBLOCKNR"  "NUMBER(10,0)"
## "STREAMTYPE"   "VARCHAR2(32 Bytes)"
## "STREAM"       "VARCHAR2(32 Bytes)"
## "SFOHOST"      "VARCHAR2(64 Bytes)"
## "TRANSFERSTATE" "VARCHAR2(32 Bytes)" [ONDISK,TRANSFERRED]
## "FILESTATE"    "VARCHAR2(32 Bytes)" [OPENED,CLOSED,DELETED]
## "FILEHEALTH"   "VARCHAR2(32 Bytes)" [SANE,TRUNCATED]
## "TZSTATE"      "VARCHAR2(32 Bytes)" filled by TZ
## "FILESIZE"     "NUMBER(10,0)"
## "GUID"         "VARCHAR2(64 Bytes)"
## "CHECKSUM"     "VARCHAR2(64 Bytes)" Adler-32
## "PFN"          "VARCHAR2(1024 Bytes)" /path/filename on Castor
## "SFOPFN"       "VARCHAR2(1024 Bytes)" /path/filename on SFO
## "NREVENTS"     "NUMBER(10,0)"
## "T_STAMP"      "TIMESTAMP(0)" "ENDTIME"    "TIMESTAMP(0)" UTC time
## "FILE_SEQNUM"  "NUMBER(10,0)"

#tier0 information
#!/usr/bin/env python

##    DATASETNAME                                        VARCHAR2(128)
##    CREATIONTIME                                       DATE
##    MODIFICATIONTIME                                   DATE
##    TYPE                                               VARCHAR2(64)
##    DATASETTYPE2                                       VARCHAR2(64)
##    WRITER1                                            VARCHAR2(64)
##    DDM                                                VARCHAR2(64)
##    AMI                                                VARCHAR2(64)
##    PSTATES                                            VARCHAR2(256)
##    PARTID                                             VARCHAR2(128)
##    SIGNOFF                                            VARCHAR2(4000)
##    NFILES                                             NUMBER(12)
##    RUNNR                                              NUMBER(12)
##    STORAGEINFO                                        VARCHAR2(1000)
##    TOTSIZE                                            NUMBER(15)
##    TOTEVENTS                                          NUMBER(12)
##    PROJECT                                            VARCHAR2(64)
##    STREAM                                             VARCHAR2(64)
##    USERNAME                                           VARCHAR2(128)
##    USERGROUP                                          VARCHAR2(32)
##    USERROLE                                           VARCHAR2(32)
from optparse import OptionParser
from collections import namedtuple
import cx_Oracle
import os,commands
import time
from datetime import datetime
import logging
#path to files with L2CostMon complete
#webpath="/afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/rustem/online"
#weblist=os.listdir(webpath)
#p = OptionParser(usage="usage: <RUNNR path> -log <log path>", version="1.1")

#
# Parse command line options
#   
import TrigCostRate.TrigCostRateConfig as TrigCostConfig
p = TrigCostConfig.prepareRatesParser()
p.add_option( '--runnr', type="int", default=156678, dest="runnr", help="the run number to be checked")
p.add_option( '--stream', type="string", default="L2CostMonitoring", dest="stream",         help="the CostMonitoring stream that we are submitting to html")
p.add_option( '--currRun', type="int", default=155700, dest="currRun",         help="the current Runnr")
p.add_option( '--log', type="string", default="CostMon.log", dest="log",         help="the log file name")
p.add_option( '--logDir', type="string", default="/afs/cern.ch/user/s/schae/testarea/AtlasWork", dest="logDir",help="output log directory")
p.add_option( '--outputDir', type="string", default="/afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/schaefer", dest="outputDir",help="output log directory")
p.add_option( '--debugDOUG', type="string", default="warning", dest="debugDOUG",help="controls the level of output: debug, info warning, error, critical")
p.add_option( '--reSubmit', type="int", default=0, dest="reSubmit",help="allows you to force things through if the costmon files are on castor")
p.add_option( '--wait-time', type="float", default=10.0, dest="wait_time",help="set amount of time that we wait for job to stop looking for new files")
p.add_option('--XMLoutputfile', type = 'string', default ='rob-ros.xml', dest = 'xml', help = 'out xml file')
p.add_option('--stopMap',   action='store_true', default=False, dest="stopMap",   help="do NOT collect the ros-rob map")

(opts, args) = p.parse_args()

#
# Parse command line options
#   
#import TrigMonCosts.TrigCostConfig as TrigCostConfig
#par = TrigCostConfig.prepareRatesParser()
#(options, args) = par.parse_args()
#
# Configure python RunConfig wrapper around C++ RunModule
#

#import os,commands
def getCastorDirectoryList(path):
    cmd  = 'nsls %s/ ' % (path)
    if path.count('/eos/'):
        cmd  = 'eos ls %s/ ' % (path)
    file = ["%s" %i for i in commands.getoutput(cmd).split('\n')]
    return file

def list_difference_html(list1, list2):
   # """uses list1 as the reference, returns list of items not in list2"""
    diff_list = []
    for item in list1:
        if str(item) not in list2 and str(item)+"-EF" not in list2 and str(item)+"-L2" not in list2:
            diff_list.append(item)
    return diff_list

def list_difference(list1, list2):
   # """uses list1 as the reference, returns list of items not in list2"""
    diff_list = []
    for item in list1:
        if str(item) not in list2 and "0"+str(item) not in list2 and item not in diff_list:
            diff_list.append(item)
    return diff_list
def getMAXRUNNR(cu):
    q = "SELECT MAX(RUNNR) FROM SFO_TZ_RUN WHERE RUNNR<1000000"
    cu.execute(q)

    for r in cu.fetchall():
        return r[0]
def isRUNstillgoing(cu,currtime,maxRUNNR,maxtime):
    records = 'RUNNR,T_STAMP'
    q="SELECT MAX(T_STAMP) FROM SFO_TZ_FILE WHERE RUNNR<1000000 and RUNNR=%s"%maxRUNNR# and STREAM like '%%CostMonitoring'
    cu.execute(q)
    #print "currenttime:",currtime
    t = time.mktime(currtime)
    t=t-opts.wait_time*3600.0
    ctime=time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(t))
    #print "currenttime minus three hours:",ctime
    #print "Checking to see if the run has terminated. The current time-3hours is:", ctime
    for r in  cu.fetchall():
        processtime=time.strftime(str(r[0]))
        maxtime.append(str(r[0]))
        #print "maxtime:",maxtime
        if processtime>=ctime:
            return 1
        elif processtime < ctime:
            return 0

def getTRANSlist(cu,run,ctime,stream):
    records = 'RUNNR,T_STAMP'
    F=namedtuple('F',records)
    q="SELECT %s FROM SFO_TZ_FILE WHERE RUNNR<1000000 and RUNNR=%s and TRANSFERSTATE='TRANSFERRED' and STREAM='%s'"%(records,run,stream)
    cu.execute(q)
    runlist=[]
    for r in  map(F._make, cu.fetchall()):
        if r.RUNNR not in runlist:
            runlist.append(r.RUNNR)
    return runlist

def getSFOFILESforRUNNR(montype,RUNNR):
    records = 'RUNNR,PFN,LFN,FILENR,T_STAMP,TRANSFERSTATE'
    F=namedtuple('F',records)
    q="SELECT %s FROM SFO_TZ_FILE WHERE RUNNR=%s and STREAM like '%s'"%(records,RUNNR,montype)
    cu.execute(q)
    list=[]
    for r in  map(F._make, cu.fetchall()):
        list.append(r)
    return list
def getProjName(RUNNR,stream,cu):
    records = 'RUNNR,DATASETNAME'
    Q=namedtuple('Q',records)
    stream_temp=stream
    if not stream.count("CostMonitoring"):
        stream_temp="debug_all"
    q = "SELECT DISTINCT %s FROM dataset WHERE RUNNR=%s and TYPE!='LOG' and DATASETNAME like '%%%s%%'" %(records,RUNNR,stream_temp)
    cu.execute(q)
    projname=""
    for r in map(Q._make, cu.fetchall()):
        #print "datasetnames:",str(r.DATASETNAME)
        if str(r.DATASETNAME).count("RAW"):
            projname=str(r.DATASETNAME).strip()
    i=4
    gotname=0
    while not gotname and len(projname):
        dummy=projname[:i]
        #print "the projectname:",dummy
        #print "gname:",str(gotname)
        if dummy.count("."):
            gotname=1
            i=i-1
        else:
            i=i+1  
    return projname[:i]

def completeinT0(cu,RUNNR,filename,ctime):
    records = 'RUNNR,NFILES,DATASETNAME,TYPE,MODIFICATIONTIME'
    Q=namedtuple('Q',records)
    stream=filename
    if filename.count("CostMonitoring"):
        stream=filename
    else:
        stream="debug_all"
    q = "SELECT DISTINCT %s FROM dataset WHERE RUNNR=%s and TYPE!='LOG' and DATASETNAME like '%%%s%%'" %(records,RUNNR,stream)
    cu.execute(q)
    t = time.mktime(ctime)
    t=t-3600.0
    fintime=time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(t))
    list=[]
    calrawfilenum=-1
    costmonfilenum=-1
    for r in map(Q._make, cu.fetchall()):
 #       print r
        if r.TYPE=="CALRAW" and filename.count("CostMonitoring"):# and str(r.MODIFICATIONTIME)<str(fintime)
            calrawfilenum=r.NFILES
        elif r.TYPE=="DBGRAW" and not filename.count("CostMonitoring"):# and str(r.MODIFICATIONTIME)<str(fintime)
            calrawfilenum=r.NFILES
        elif r.TYPE=="NTUP_COSTMON":# and str(r.MODIFICATIONTIME)<str(fintime)
            costmonfilenum=r.NFILES
        elif r.TYPE.count("NTUP") and r.DATASETNAME.count("TrigCost"):# and str(r.MODIFICATIONTIME)<str(fintime)
            costmonfilenum=r.NFILES
            #print "T0 file:",r.DATASETNAME
        elif r.TYPE.count("RAW") and r.DATASETNAME.count("TrigCost"):# and str(r.MODIFICATIONTIME)<str(fintime)
            calrawfilenum=r.NFILES
            #print "T0 file raw:",r.DATASETNAME
    list.append(calrawfilenum)
    list.append(costmonfilenum)
    return list


def getTRANSFILElist(cu,alreadydone,lastprocessed):
    records = 'RUNNR,PFN,LFN,FILENR'
    F=namedtuple('F',records)
    q="SELECT %s FROM SFO_TZ_FILE WHERE RUNNR<1000000 and RUNNR>%s and TRANSFERSTATE='TRANSFERRED' and STREAM like '%%CostMonitoring'"%(records,lastprocessed)
    cu.execute(q)
    runlist=[]
    for r in map(F._make, cu.fetchall()):
        runlist.append(r.RUNNR)
    return runlist

def getT0TRANSFILElist(cu,minNEWRUNNR):
    records = 'DATASETNAME,RUNNR,NFILES,TOTEVENTS,STREAM,PROJECT'
    Q=namedtuple('Q',records)
    q = "SELECT DISTINCT %s FROM dataset WHERE RUNNR>=%s and TYPE!='LOG' and DATASETNAME like '%%CostMonitoring%%'" %(records,minNEWRUNNR)
    cu.execute(q)
    list=[]
    for r in map(Q._make, cu.fetchall()):
        list.append(r.RUNNR)
    return list

def writeRUNSTATLOG(tuple,cu_T0):
    records = 'DATASETNAME,RUNNR,NFILES,TOTEVENTS,STREAM'
    Q=namedtuple('Q',records)
    stream_temp=tuple.stream
    if not stream_temp.count("CostMonitoring"):
        stream_temp="debug_all"
    qu="SELECT DISTINCT %s FROM dataset WHERE RUNNR=%s and TYPE!='LOG' and DATASETNAME like '%%%s%%'" %(records,tuple.runnr,stream_temp)
    cu_T0.execute(qu)
    for r in map(Q._make, cu_T0.fetchall()):
        if (r.DATASETNAME).count("daq.RAW"):
            tuple=tuple._replace(nFilesSFO=str(r.NFILES),nEventsSFO=str(r.TOTEVENTS))
        elif (r.DATASETNAME).count("NTUP_COSTMON"):
            tuple=tuple._replace(nFilesT0=str(r.NFILES))
    return tuple

def versionFinder(newversionL2,fullnewversionL2,versionlistL2):
    for it in versionlistL2:
        #print "the versions list for stream:",it
        if str(it).count("COSTMON") or str(it).count("TrigCost"):
            version=(it.strip())[-1:]
            #print "version number:",version
            i=0
            while version.isdigit():
                #print "looking for digit:",version
                i=i-1
                version=(it.strip())[-1+i:]
            newversionL2.append(int((it.strip())[i:]))
    if len(newversionL2):
        newverL2=max(newversionL2)
    else:
        newverL2="redherring"
    for it in versionlistL2:
        if it.count("c"+str(newverL2)) and it.count("COSTMON"):
            fullnewversionL2.append(it)
            #print "the version list set:",str(it)
        if it.count("g"+str(newverL2)) and it.count("TrigCostEF") and opts.stream.count("EF"):
            fullnewversionL2.append(it)
            #print "the version list set:",str(it)
        if it.count("g"+str(newverL2)) and it.count("TrigCostL2") and opts.stream.count("L2"):
            fullnewversionL2.append(it)
            #print "the version list set:",str(it) 
    if not len(fullnewversionL2):
        fullnewversionL2.append("No COSTMON in directory")
    return newverL2
def getSFOnFiles(cu,runnr,stream):
    records = 'SFOID,STREAM,RUNNR,FILENR,NREVENTS'
    R=namedtuple('R',records)
    q = "SELECT %s FROM SFO_TZ_FILE WHERE RUNNR=%s and RUNNR<10000000 and STREAM like '%s'" % (records,runnr,stream)
    cu.execute(q)
    nfiles=0
    for r in map(R._make, cu.fetchall()):
        nfiles=nfiles+1
    return nfiles

def getSFOnEvents(cu,runnr,stream):
    records = 'SFOID,STREAM,RUNNR,FILENR,NREVENTS'
    R=namedtuple('R',records)
    q = "SELECT %s FROM SFO_TZ_FILE WHERE RUNNR=%s and RUNNR<10000000 and STREAM like '%s'" % (records,runnr,stream)
    cu.execute(q)
    nevents=0
    for r in map(R._make, cu.fetchall()):
        nevents=nevents+r.NREVENTS
    return nevents

def waitforfile(file,waitfiles,item,maxRUNNR,IsRUNstillgoing,f_wait):
    #print "NO FILES processed by T0! num files transferred to T0:",T0processednum[0]
    #print "writing file out to look at next time!"
    if file.LFN+"1" not in waitfiles and item==maxRUNNR and IsRUNstillgoing:
        f_wait.write(file.LFN+"1"+"\n")
        return 1
    elif file.LFN+"1" in waitfiles and item==maxRUNNR and IsRUNstillgoing:
        f_wait.write(file.LFN+"1"+"\n")
        return 1
    elif file.LFN+"1" not in waitfiles and file.LFN+"2" not in waitfiles and file.LFN+"3" not in waitfiles and item==maxRUNNR and not IsRUNstillgoing:
        f_wait.write(file.LFN+"1"+"\n")
        return 1
    elif file.LFN+"1" in waitfiles and item==maxRUNNR and not IsRUNstillgoing:
        f_wait.write(file.LFN+"2"+"\n")
        return 1
    elif item!=maxRUNNR and file.LFN+"1" not in waitfiles and  file.LFN+"2" not in waitfiles and file.LFN+"3" not in waitfiles:
        f_wait.write(file.LFN+"1"+"\n") 
        return 1
    elif file.LFN+"1" in waitfiles and item!=maxRUNNR:
        f_wait.write(file.LFN+"2"+"\n")
        return 1
    elif file.LFN+"2" in waitfiles:
        f_wait.write(file.LFN+"3"+"\n")
        return 1
    elif file.LFN+"3" in waitfiles:
        f_wait.write(file.LFN+"3"+"\n")
        return 0
def getLogLevel(debug):
    print "debug line:",debug
    if debug=="debug":
        return logging.DEBUG
    elif debug=="info":
        return logging.INFO
    elif debug=="warning":
        return logging.WARNING
    elif debug=="error":
        return logging.ERROR
    elif debug=="critical":
        return logging.CRITICAL
    else:
        print "Error debug_DOUG name is not acceptable. Try debug or warning"
        return logging.ERROR
#        break
if __name__=="__main__":
    #
    # Downloading the rob-ros map
    #
    if not opts.stopMap:
        print "Collecting ROS-ROB Map!"
        import TrigCostExec.TrigCostExecPython as cost
        cost.writeROB2ROSXML(opts.runnr, opts.xml, useROBIN=True)
#setting up the parser for runRates.py
    run = TrigCostConfig.prepareT0Config(opts,args)
    reg = run.GetRegistryConfig()
    reg.WriteXML(opts.runmodxml)
    run.Config(reg)
    run.Init()

    logging.basicConfig(filename=opts.outputDir+'/logfiles/log_'+str(opts.runnr)+'_'+opts.stream+'.log',level=getLogLevel(opts.debugDOUG))
    IsRUNstillgoing=1
    logging.info("Called selectFILE.py for runnr:"+str(opts.runnr)+" and stream:"+opts.stream+" at "+str(time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(time.time()))))
    Quest = namedtuple('Quest', 'runnr, stream, nFilesT0, nEventsT0, processStat, note, nFilesSFO, nEventsSFO')
    quest=Quest(runnr=str(opts.runnr),stream=str(opts.stream),nFilesT0="N/A",nEventsT0="N/A",processStat="Submitted to selectFILE.py",note="",nFilesSFO="N/A",nEventsSFO="N/A")
    donefiles=[]
    if os.path.exists(opts.outputDir+'/donefiles/filesthataredone_'+str(opts.runnr)+'_'+opts.stream+'.dat'):
        f_donetemp=open(opts.outputDir+'/donefiles/filesthataredone_'+str(opts.runnr)+'_'+opts.stream+'.dat','r+')
        for line in f_donetemp:
            if len(line.strip())>0:
                donefiles.append(str(line.rstrip("\n")).strip())
                logging.debug("donefiles list:"+line.rstrip("\n"))
        f_donetemp.close()
    while IsRUNstillgoing:        
        maxtime=[]
        waitfiles=[]
        T0processednum_PERM=[]
        T0processednum_PERM.append(-10)
        T0processednum_PERM.append(-10)
        logging.info("This is the first loop over the run looking for CostMon Files!")
        ctime=time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(time.time()))
        ctime=time.strptime(ctime,"%Y-%m-%d %H:%M:%S")
        logging.debug(ctime)
        co = cx_Oracle.connect("ATLAS_SFO_T0_R/readmesfotz2008@atlas_config")
        cu = co.cursor()
    #tier0
        co_T0 = cx_Oracle.connect("ATLAS_T0_R1/titft0ra@atlas_t0")
        cu_T0 = co_T0.cursor()
        
######NEED TO MODIFY all things dependent on castorL2path
######nsls /castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data10_comm/debug_all/0156682/data10_comm.00156682.debug_all.Reproc.NTUP_COSTMON.c153                    #data10_comm.00156682.debug_EFHltTimeout.Reproc.NTUP_COSTMON.c153._lb0000._CAF_0001._0001.CAF_0006.root.1
#data10_comm.00156682.debug_EFHltTimeout.Reproc.NTUP_COSTMON.c153._lb0000._CAF_0002._0001.CAF_0007.root.1
#data10_comm.00156682.debug_EFHltTimeout.Reproc.NTUP_COSTMON.c153._lb0000._CAF_0003._0001.CAF_0004.root.1
######
######
######
        logging.debug("Finding the project Name")
        optscurrRUNNR=getMAXRUNNR(cu)
        castorL2path=""
        ProjName=getProjName(opts.runnr,opts.stream,cu_T0)
        if (opts.stream).count("EFCostMonitoring"):
            castorL2path="/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/"+ProjName+"/calibration_"+opts.stream
        elif (opts.stream).count("CostMonitoring"):
            castorL2path="/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/"+ProjName+"/calibration_"+opts.stream
        else:
            castorL2path="/castor/cern.ch/grid/atlas/caf/atlcal/temp/trigger/"+ProjName+"/debug_all"
        logging.debug("The project name for this run is:"+str(ProjName))
        #print "project name:",castorL2path
        #alreadydone_completeRUNNR will be a list of RUNNR that have already been processed. I will extract the largest, and only search for RUNNR larger than the biggest.
        #this fin file carries the information about all runnrs that have been completely run over and transferred out at least three hours ago. 
        #fin = open('transferredfilelist.dat', 'r+')
        if os.path.exists(opts.outputDir+'/waitfiles/waitforfiles_'+str(opts.runnr)+'_'+opts.stream+'.dat'):
            f_temp=open(opts.outputDir+'/waitfiles/waitforfiles_'+str(opts.runnr)+'_'+opts.stream+'.dat','r+')
            for line in f_temp:
                if len(line)>0:
                #waitfile_attempts.append(line[-1:])
                    waitfiles.append(str(line.rstrip("\n")))
                    logging.debug("runnr attempt:"+str(line)[-1:])
                    logging.debug("the runnr:"+str(line)[:-1])
            f_temp.close()
        for y in waitfiles:
            logging.info("waitfiles:"+str(y))
        if os.path.exists(opts.outputDir+'/donefiles/filesthataredone_'+str(opts.runnr)+'_'+opts.stream+'.dat'):
            f_donetemp=open(opts.outputDir+'/donefiles/filesthataredone_'+str(opts.runnr)+'_'+opts.stream+'.dat','r+')
            for line in f_donetemp:
                if len(line.strip())>0  and str(line.rstrip('\n')).strip() not in donefiles:
                    donefiles.append(line.rstrip("\n"))
                    logging.debug("donefiles list:"+line.rstrip("\n"))
            f_donetemp.close()
        
        #open the wait file for writing
        f_wait=open(opts.outputDir+'/waitfiles/waitforfiles_'+str(opts.runnr)+'_'+opts.stream+'.dat','w')
    
        # this reads in the last file that was fully transferred out from the SFO to the T0
        SFOTRANSlist=getTRANSlist(cu,str(opts.runnr),ctime,opts.stream)
        #if len(SFOTRANSlist):
         #   currentRUNNR=max(SFOTRANSlist)
        for y in SFOTRANSlist:
            logging.debug("The SFO RUNNR translist:"+str(y))
        logging.info("the max runner from sfo transferred files:"+str(optscurrRUNNR))
        #print "the castor Path:",castorL2path
        castorL2dir=getCastorDirectoryList(castorL2path)
        for z in castorL2dir:
            logging.debug("castor dir:"+str(z))
        IsRUNstillgoing=isRUNstillgoing(cu,ctime,str(opts.runnr),maxtime)
        logging.debug("isrunstillgoing:"+str(IsRUNstillgoing))
        SFOFILES=getSFOFILESforRUNNR(opts.stream,str(opts.runnr))
        #logging.debug("SFOFILES:",SFOFILES)
        #check to make sure that run is on castor
        if "0"+str(opts.runnr) not in castorL2dir and "00"+str(opts.runnr) not in castorL2dir:
            logging.info("NOTE RUNNR did not copy to castor!")
            for file_L2 in SFOFILES:
                logging.info("NOTE write html to state file not copied YET!")
                waitvar=waitforfile(file_L2,waitfiles,opts.runnr,optscurrRUNNR,IsRUNstillgoing,f_wait)
                if not waitvar:
                    logging.info("NOTE: failed to copy file to castor! Write html about this!")
            logging.info("NOTE file not processed YET!")
            #logging.info("NEED TO WRITE WEB PAGE ABOUT THIS !!!!!!!!!!!!@@@@@@@@@!!!!!!!!")
        elif not len(SFOTRANSlist):
            if not IsRUNstillgoing and opts.runnr==optscurrRUNNR:
                logging.info("NOTE! current run has Zero files! We will wait to see if some appear")
            elif opts.runnr!=optscurrRUNNR:
                logging.info("NOTE! Zero files contained in run! -- This may be trouble because run is not still going!")
        #elif str(opts.runnr) in weblist or str(opts.runnr)+"-"+str(opts.stream)[:2] in weblist:
        #    logging.debug("already ran this runnr and the results are posted to html!:"+str(opts.runnr))
        #    logging.debug("Is this run already on weblist?:"+str(opts.runnr)+"-"+str(opts.stream)[:2])
        #elif str(opts.runnr) in weblist or str(opts.runnr)+"-"+str(opts.stream) in weblist:
        #    logging.debug("already ran this runnr and the results are posted to html!:"+str(opts.runnr))
        #    logging.debug("Is this run already on weblist?:"+str(opts.runnr)+"-"+str(opts.stream))
    ##########
    #Finish this!
    #Check that I am getting all new files. Not just after they have all transferred. check sfo for costmon files and see if they made it to castor
    #Also add in a script for runs that have no files transfer out. the a call to write to an html
    #Add call to write an html for runs with problems
    #
    ########
         #waitforfile(file,waitfiles,opts.runnr,optscurrRUNNR,IsRUNstillgoing,alreadydone_completeRUNNR,f_wait)
        else:
            logging.debug("made it into the directory on castor")
             #finding castor directory
            versionlistL2 = getCastorDirectoryList(castorL2path+"/0"+str(opts.runnr))
            if len(versionlistL2)>0:
                if versionlistL2[0].count(': No such file or directory'):
                    #print ': No such file or directory'
                    versionlistL2 = getCastorDirectoryList(castorL2path+"/00"+str(opts.runnr))
            newversionL2=[]
            fullnewversionL2_temp=[]
            newverL2=versionFinder(newversionL2,fullnewversionL2_temp,versionlistL2)
            fullnewversionL2=fullnewversionL2_temp[0]
            castorL2RunVerdir=[]
            logging.debug("may be continuing incorrectly here"+fullnewversionL2)
            #print "may be continuing incorrectly here"+fullnewversionL2
            if not fullnewversionL2.count("COSTMON") and not fullnewversionL2.count("TrigCost"):
                logging.info("NOTE the version directory has not been copied yet!:"+fullnewversionL2)
                #print "NOTE the version directory has not been copied yet!:"+fullnewversionL2
                continue
            elif fullnewversionL2.count("COSTMON") and fullnewversionL2.count('data12_'):
                logging.info("found a costmon directory:"+castorL2path+"/00"+str(opts.runnr)+"/"+fullnewversionL2)
                #print "found a costmon directory:"+castorL2path+"/0"+str(opts.runnr)+"/"+fullnewversionL2
                castorL2RunVerdir=getCastorDirectoryList(castorL2path+"/00"+str(opts.runnr)+"/"+fullnewversionL2)
            elif fullnewversionL2.count("COSTMON"):
                logging.info("found a costmon directory:"+castorL2path+"/0"+str(opts.runnr)+"/"+fullnewversionL2)
                #print "found a costmon directory:"+castorL2path+"/0"+str(opts.runnr)+"/"+fullnewversionL2
                castorL2RunVerdir=getCastorDirectoryList(castorL2path+"/0"+str(opts.runnr)+"/"+fullnewversionL2)                
            elif fullnewversionL2.count("TrigCost"):
                logging.info("found a costmon directory:"+castorL2path+"/00"+str(opts.runnr)+"/"+fullnewversionL2)
                #print "found a costmon directory:"+castorL2path+"/00"+str(opts.runnr)+"/"+fullnewversionL2
                castorL2RunVerdir=getCastorDirectoryList(castorL2path+"/00"+str(opts.runnr)+"/"+fullnewversionL2)
            else:
                log.error('unable to understand input: %s' %fullnewversionL2)
            for y in castorL2RunVerdir:
                logging.debug("castorL2 RUNVERDIR:"+str(y))
            #LFN='data10_comm.00153565.calibration_L2CostMonitoring.daq.RAW._lb0000._SFO-4._0033.data
            #data10_comm.00156024.calibration_EFCostMonitoring.Reproc.NTUP_COSTMON.c152._lb0000._SFO-5._0012.CAF_0071.root.1
            #print "length of SFO files:",len(SFOFILES)
            for file in SFOFILES:
                logging.debug("found file:"+str(file))
                if file.TRANSFERSTATE=='TRANSFERRED' and (file.LFN).count(str(opts.stream)) and not (file.LFN).count("writing"):
                    locAndNum=file.LFN
                    if (file.LFN).count("CostMonitoring"):
                        locAndNum=locAndNum[:-4]
                        locAndNum=locAndNum[46+len(ProjName):]
                    else:
                        locAndNum="._lb0000._CAF_"#+"0001"+"._0001.CAF_"+"0006"+".root.1"
######NEED TO MODIFY code to accomdate the debug streams!!!!!!!!!!!
######
######R(SFOID='SFO-9', STREAM='efdProcTimeout', RUNNR=156682, FILENR=1, TRANSFERSTATE='TRANSFERRED', LFN='data10_comm.00156682.debug_efdProcTimeout.daq.RAW._lb0000._SFO-9._0001.data', PFN='/castor/cern.ch/grid/atlas/DAQ/2010/00156682/debug_efdProcTimeout/data10_comm.00156682.debug_efdProcTimeout.daq.RAW._lb0000._SFO-9._0001.data', SFOPFN='/raid_cntl1/data/data10_comm.00156682.debug_efdProcTimeout.daq.RAW._lb0000._SFO-9._0001.data')
#R(SFOID='SFO-10', STREAM='EFHltTimeout', RUNNR=156682, FILENR=1, TRANSFERSTATE='TRANSFERRED', LFN='data10_comm.00156682.debug_EFHltTimeout.daq.RAW._lb0000._SFO-10._0001.data', PFN='/castor/cern.ch/grid/atlas/DAQ/2010/00156682/debug_EFHltTimeout/data10_comm.00156682.debug_EFHltTimeout.daq.RAW._lb0000._SFO-10._0001.data', SFOPFN='/raid_cntl2/data/data10_comm.00156682.debug_EFHltTimeout.daq.RAW._lb0000._SFO-10._0001.data')
######
######
######
######STREAM='efdProcTimeout', RUNNR=157108, FILENR=1, TRANSFERSTATE='ONDISK', LFN='data10_comm.00157108.debug_efdProcTimeout.daq.RAW._lb0000._SFO-5._0001.data.writing
                        #check in T0 for processed files
                    sfofilename=""
                    castorfilenameL2=""
                    if str(opts.stream).count("CostMonitoring"):
                        sfofilename=ProjName+".00"+str(file.RUNNR)+".calibration_"+str(opts.stream)
                        castorfilenameL2=ProjName+".00"+str(file.RUNNR)+".calibration_"+str(opts.stream)#+".Reproc.NTUP_COSTMON.c"+str(newverL2)+locAndNum
                        #print "the full castor name:",castorfilenameL2
                    else:
                        sfofilename=ProjName+".00"+str(file.RUNNR)+".debug_"+str(opts.stream)
                        castorfilenameL2=ProjName+".00"+str(file.RUNNR)+".debug_"+str(opts.stream)#+".Reproc.NTUP_COSTMON."
                        #print "castor name for debug stream:",castorfilenameL2
#data10_comm.00156682.debug_EFHltTimeout.Reproc.NTUP_COSTMON.c153._lb0000._CAF_0001._0001.CAF_0006.root.1
#data11_cos.00175254.debug_L2HltTimeout.TrigCostL2.NTUP.g9._lb0000._CAF_0002.root.1
                    #logging.debug("castor file name:",castorfilenameL2)
                    T0processednum=completeinT0(cu_T0,str(file.RUNNR),str(opts.stream),ctime)
                    quest=writeRUNSTATLOG(quest,cu_T0)
                    quest=quest._replace(nFilesSFO=str(getSFOnFiles(cu,opts.runnr,opts.stream)).strip())
                    quest=quest._replace(nEventsSFO=str(getSFOnEvents(cu,opts.runnr,opts.stream)).strip())
                    if not str(opts.stream).count('CostMonitoring'):
                        numT0Files=0
                        for castFile in castorL2RunVerdir:
                            if castFile.count(opts.stream):
                                numT0Files=1+numT0Files
                        quest=quest._replace(nFilesT0=str(numT0Files))
                    if str(quest.nFilesSFO).count('N/A') and str(quest.nFilesT0).count('N/A'):
                        T0processednum_PERM=[int(-1),int(-1)]
                        T0processednum=[int(-1),int(-1)]
                        #print "Found zero SFO files"
                    elif str(quest.nFilesSFO).count('N/A') and str((quest.nFilesT0).strip()).isdigit():
                        T0processednum_PERM=[int(-1),int((quest.nFilesT0).strip())]
                        T0processednum=[int(-1),int((quest.nFilesT0).strip())]
                        #print "Found zero SFO files but t0"
                    elif str(quest.nFilesT0).count('N/A') and str((quest.nFilesSFO).strip()).isdigit():
                        T0processednum_PERM=[int((quest.nFilesSFO).strip()),int(-1)]   
                        T0processednum=[int((quest.nFilesSFO).strip()),int(-1)]
                        #print "Found zero T0 files"
                    elif str((quest.nFilesT0).strip()).isdigit() and str((quest.nFilesSFO).strip()).isdigit():
                        T0processednum_PERM=[int((quest.nFilesSFO).strip()),int((quest.nFilesT0).strip())] 
                        T0processednum=[int((quest.nFilesSFO).strip()),int((quest.nFilesT0).strip())]
                        #print "Found files T0:",quest.nFilesT0
                        #print "Found files SFO:",quest.nFilesSFO
                    if T0processednum[1]>0:
                        #check if it is in castor
                        #print "castorL2run with version directory:",castorL2RunVerdir
                        for file_cast in castorL2RunVerdir:
                            logging.debug("files on castor:"+file_cast)
                            #print "file on castor",file_cast
#                            IsProcessedbyT0=isProcessedbyT0(cu_T0,file.LFN)
                            if file_cast.count(castorfilenameL2) and len(file_cast)>0:
                                if file_cast not in donefiles:
                                    #f_done.write(file_cast+"\n")
                                    print "File Submitted to runRates.py::::::::::",file_cast
                                    donefiles.append(file_cast.strip())
                                    logging.info("submit file to runCost.py!")
                                    #runcommand="./submitCost.py -r "+str(run)+" --tag 'data10_comm' --proj "+currentEFversion[i]+" -p -o /data/schaefer -n 1000000 --submit"#/afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/rustem/online
                                    if castorL2path.count('data12_'):
                                        runcommand="./runCost.py "+castorL2path+"/00"+str(opts.runnr)+"/"+fullnewversionL2+"/"+str(file_cast)+" -o '/afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/rustem/online' --data -k 'COSTMON' --do-time -n 100000 &> log &"
                                    else:
                                        runcommand="./runCost.py "+castorL2path+"/0"+str(opts.runnr)+"/"+fullnewversionL2+"/"+str(file_cast)+" -o '/afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/rustem/online' --data -k 'COSTMON' --do-time -n 100000 &> log &"
       #./macros/runRates.py /data/rustem/data10_comm.00155116.calibration_EFCostMonitoring.Reproc.NTUP_COSTMON.c152._lb0000._SFO-1._0001.CAF_0001.root.1 -o "/data/schaefer" --data -k "COSTMON" --do-time -n 100000 &> log & tail -f log
                                    logging.info(runcommand)
                                    if str(file_cast).count('TrigCostL2') and opts.stream.count('L2'):
                                        this_file_name = castorL2path+"/00"+str(opts.runnr)+"/"+fullnewversionL2+"/"+str(file_cast)
                                        print "the path L2CostMonitoring to castor:",this_file_name
                                        run.AddInputFile("root://castoratlas/"+this_file_name)
                                    elif str(file_cast).count('TrigCostEF') and opts.stream.count('EF'):
                                        this_file_name = castorL2path+"/00"+str(opts.runnr)+"/"+fullnewversionL2+"/"+str(file_cast)
                                        print "the path EFCostMonitoring to castor:",this_file_name
                                        run.AddInputFile("root://castoratlas/"+this_file_name)
                                    elif str(file_cast).count('data12_'):
                                        this_file_name = castorL2path+"/00"+str(opts.runnr)+"/"+fullnewversionL2+"/"+str(file_cast)
                                        print "the path EFCostMonitoring to castor:",this_file_name
                                        run.AddInputFile("root://castoratlas/"+this_file_name)                                        
                                    elif not str(file_cast).count('TrigCost'):
                                        this_file_name = castorL2path+"/0"+str(opts.runnr)+"/"+fullnewversionL2+"/"+str(file_cast)
                                        print "the path to TrigCost castor:",this_file_name
                                        run.AddInputFile("root://castoratlas/"+this_file_name)
                                    print 'CastPath: ',castorL2path
                                    print 'fullnewversion: ',fullnewversionL2
                                    print 'file_cast: ',file_cast
                                    run.Exec()
                                    run.ClearInputFiles()
                                elif file_cast in donefiles:
                                    logging.debug("file already done")
                                if opts.runnr!=int(optscurrRUNNR) and T0processednum[0]==T0processednum[1] and T0processednum[1]>0 and not len(waitfiles):
                                    logging.info("done with this runnr!"+str(opts.runnr))
                                    IsRUNstillgoing=0
                      
                    elif T0processednum[1]==0 and T0processednum[0]>0 and not opts.reSubmit:
                        logging.info("NOTE: NO Files Processed by T0 - GOING TO WAIT ON THESE FILES! SHOULD BE WRITTEN TO WAITFILES")
                        waitVAR=waitforfile(file,waitfiles,opts.runnr,optscurrRUNNR,IsRUNstillgoing,f_wait)
                        if not waitVAR:
                            logging.info("NOTE file never processed!")
                    elif T0processednum[1]==0 and T0processednum[0]>0 and opts.reSubmit:
                        logging.info("NOTE: This is Forced resubmission!")
                        if len(castorL2RunVerdir):
                            for file_cast in castorL2RunVerdir:
                                logging.debug("Forced::::files on castor:"+file_cast)
                                if file_cast.count(castorfilenameL2) and len(file_cast)>0:
                                    if file_cast not in donefiles:
                                        #f_done.write(file_cast+"\n")
                                        donefiles.append(file_cast)
                                        logging.info("submit file to runCost.py!")
                                        runcommand="./runCost.py "+castorL2path+"/0"+str(opts.runnr)+"/"+fullnewversionL2+"/"+str(file_cast)+" -o '/afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/rustem/online' --data -k 'COSTMON' --do-time -n 100000 &> log &"
                                        logging.info(runcommand)
                                        run.AddInputFile("root://castoratlas/"+castorL2path+"/0"+str(opts.runnr)+"/"+fullnewversionL2+"/"+str(file_cast))
                                        run.Exec()
                                        run.ClearInputFiles()
                                    elif file_cast in donefiles:
                                        logging.debug("file already done")
                                        #f_done.write(file_cast+"\n")
                                else:
                                    logging.error('FORCED:::::no COSTMON files on Castor'+castorL2path+"/0"+str(opts.runnr)+"/"+fullnewversionL2)
                        elif not len(castorL2RunVerdir):
                            logging.error('Forced::::Error There are no Files on Castor!!!!!!')
                            logging.error('Forced::::Non-existent directory:'+castorL2path+"/0"+str(opts.runnr)+"/"+fullnewversionL2)

                    elif T0processednum[1]==0 and T0processednum[0]==0:
                        logging.info("NOTE: NO FILES transferred to T0: - GOING TO WAIT ON THESE FILES! SHOULD BE WRITTEN TO WAITFILES")
                        waitVAR=waitforfile(file,waitfiles,opts.runnr,optscurrRUNNR,IsRUNstillgoing,f_wait)
                        if not waitVAR:
                            logging.warning("NOTE file never processed!")
                            logging.warning("NEED TO WRITE WEB PAGE ABOUT THIS !!!!!!!!!!!!@@@@@@@@@!!!!!!!!")
                    elif T0processednum[0]<0:
                        logging.info("NOTE: Did not find the file in T0!!! - GOING TO WAIT ON THESE FILES! SHOULD BE WRITTEN TO WAITFILES")
                        waitVAR=waitforfile(file,waitfiles,opts.runnr,optscurrRUNNR,IsRUNstillgoing,f_wait)
                        if not waitVAR:
                            logging.warning("NOTE file never processed!")
                            #logging.info("NEED TO WRITE WEB PAGE ABOUT THIS !!!!!!!!!!!!@@@@@@@@@!!!!!!!!")
        f_done=open(opts.outputDir+'/donefiles/filesthataredone_'+str(opts.runnr)+'_'+opts.stream+'.dat','w')
        for file_done in donefiles:
            f_done.write(file_done+'\n')
        f_done.close()
        if not IsRUNstillgoing:
            if "0"+str(opts.runnr) not in castorL2dir:
                logging.error("ERROR:::::::: RUNNR did not copy to castor!")
                for file_L2 in SFOFILES:
                    logging.warning("ERROR:::::::: write html to state file not copied!")
                    waitvar=waitforfile(file_L2,waitfiles,opts.runnr,optscurrRUNNR,IsRUNstillgoing,f_wait)
                    if not waitvar:
                        logging.error("ERROR::::::::: failed to copy file to castor! Write html about this!")
                logging.error("ERROR:::::::: file never processed!")
                logging.error("NEED TO WRITE WEB PAGE ABOUT THIS !!!!!!!!!!!!@@@@@@@@@!!!!!!!!")
            elif not len(SFOTRANSlist):
                if not IsRUNstillgoing and opts.runnr==optscurrRUNNR:
                    logging.info("ERROR::::::::! current run Zero files contained in run! Write html about this")
                elif opts.runnr!=optscurrRUNNR:
                    logging.info("ERROR::::::::! Zero files contained in run! Write html about this")
            logging.info("The monitoring of this run "+str(opts.runnr)+" with stream "+str(opts.stream)+" has concluded!")
            logging.info("The number of SFO files transferred to T0 is:"+str(T0processednum_PERM[0]))
            logging.info("The number of files processed by T0 is:"+str(T0processednum_PERM[1]))
            if T0processednum_PERM[0]==0 and len(SFOFILES):
                logging.info("ERROR: NO SFO FILES TRANSFERRED TO T0! NEED TO WRITE WEB PAGE ABOUT THIS !!!!!!!!!!!!@@@@@@@@@!!!!!!!!")
            elif T0processednum_PERM[0]<0:
                logging.info("ERROR: "+str(len(SFOFILES))+" SFO FILES TRANSFERRED TO T0! NEED TO WRITE WEB PAGE ABOUT THIS !!!!!!!!!!!!@@@@@@@@@!!!!!!!!")
            elif T0processednum_PERM[0]>0 and len(SFOFILES) and T0processednum_PERM[1]==0:
                logging.info("ERROR: NO SFO FILES Processed by T0! NEED TO WRITE WEB PAGE ABOUT THIS !!!!!!!!!!!!@@@@@@@@@!!!!!!!!")
            elif T0processednum_PERM[0]>0 and len(SFOFILES) and T0processednum_PERM[1]>T0processednum_PERM[0]:
                logging.info("ERROR: NOT ALL FILES PROCESSED BY T0! NEED TO WRITE WEB PAGE ABOUT THIS !!!!!!!!!!!!@@@@@@@@@!!!!!!!!")
                logging.info("ERROR: These include the following files failed files:")
                for disappointment in waitfiles:
                    if (disappointment.strip())[-1:]=="3":
                        logging.info(disappointment)
       #fin.close()
        f_wait.close()
        #logging the output from this iteration of selectFILE.py
        runStatusFile=open('/afs/cern.ch/user/t/trigcomm/w0/rates/t0cost_archives/logs/runStatus_'+str(opts.runnr)+'_'+opts.stream+'_html.dat',"w")
        quest=writeRUNSTATLOG(quest,cu_T0)
        quest=quest._replace(nFilesSFO=str(getSFOnFiles(cu,opts.runnr,opts.stream)).strip())
        quest=quest._replace(nEventsSFO=str(getSFOnEvents(cu,opts.runnr,opts.stream)).strip())
        if IsRUNstillgoing:
            quest=quest._replace(processStat="Wait for RUN to Finish")
        elif str(optscurrRUNNR)==str(opts.runnr):
            quest=quest._replace(processStat="Run In Progress")
        elif not IsRUNstillgoing:
            quest=quest._replace(processStat="Run Finished: Wait for T0 to Process Files")
            if len(waitfiles)>0:
                quest=quest._replace(note=str(len(waitfiles))+' NOT processed by T0: see files at: '+opts.outputDir+'/waitfiles/waitforfiles_'+str(opts.runnr)+'_'+opts.stream+'.dat')
        runStatusFile.write(quest.runnr+'\n'+quest.stream+'\n'+quest.nFilesT0+'\n'+quest.nEventsT0+'\n'+quest.processStat+'\n'+quest.note+'\n'+quest.nFilesSFO+'\n'+quest.nEventsSFO)
        runStatusFile.close()
        if IsRUNstillgoing:
            logging.info("About to sleep for 300 seconds before looking for more files for run:"+str(opts.runnr)+"and stream:"+opts.stream+"\n")
            logging.info("current end of the run is at:"+maxtime[0]+" and the current time is:"+str(time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(time.time())))+"\n")
        elif not IsRUNstillgoing:
            logging.info("FINISHED:::::Terminating Search for CostMon files for:"+str(opts.runnr)+"and stream:"+opts.stream+" at "+str(time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(time.time())))+"\n"+"     See "+opts.outputDir+'/donefiles/filesthataredone_'+str(opts.runnr)+'_'+opts.stream+'.dat'+" for more info about the files submitted to runCost.py"+"\n")
        logging.debug("Now sleeping for 1000 seconds. Then will wake for look for more T0 files.")
        #f_done.close()
        if IsRUNstillgoing:
            time.sleep(1000)
    #about to terminate - send flag to let the operation know that things should finish in an hour
    #if it does not finish, then resubmit
    runStatusFile_check=open('/afs/cern.ch/user/t/trigcomm/w0/rates/t0cost_archives/logs/runStatus_'+str(opts.runnr)+'_'+opts.stream+'_html.dat',"w")
    quest=quest._replace(note="Calling run.Done!")
    runStatusFile_check.write(quest.runnr+'\n'+quest.stream+'\n'+quest.nFilesT0+'\n'+quest.nEventsT0+'\n'+quest.processStat+'\n'+quest.note+'\n'+quest.nFilesSFO+'\n'+quest.nEventsSFO)
    runStatusFile_check.close()
    #terminate the process by calling run.Done()
    run.Done()
    TrigCostConfig.doAfterExecWork(opts, args, run)
    runStatusFile=open('/afs/cern.ch/user/t/trigcomm/w0/rates/t0cost_archives/logs/runStatus_'+str(opts.runnr)+'_'+opts.stream+'_html.dat',"w")
    if not str(opts.stream).count('CostMonitoring'):
        quest=quest._replace(nFilesT0=str(len(donefiles)))
    quest=quest._replace(processStat="selectFile Exited")
    quest=quest._replace(note="Number of Files that selectFILE found:"+str(len(donefiles)))
    runStatusFile.write(quest.runnr+'\n'+quest.stream+'\n'+quest.nFilesT0+'\n'+quest.nEventsT0+'\n'+quest.processStat+'\n'+quest.note+'\n'+quest.nFilesSFO+'\n'+quest.nEventsSFO)
    runStatusFile.close()
    print "wrote the log file: "+'/afs/cern.ch/user/t/trigcomm/w0/rates/t0cost_archives/logs/runStatus_'+str(opts.runnr)+'_'+opts.stream+'_html.dat'
    print "Finished!"
