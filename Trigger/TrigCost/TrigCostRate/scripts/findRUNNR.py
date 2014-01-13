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
print "starting findRUNNY"
from optparse import OptionParser
p = OptionParser()
p.add_option('--que', type="string", default="atlasb1", dest="bqueue",    help="batch queue")
p.add_option( '--input', type="string", default="/Trigger/TrigCost/TrigCostRate/scripts/foundruns.dat", dest="input",         help="path to input file that has previously run over RUNNRs. NOTE:File must exist")
p.add_option( '--testarea', type="string", default="/afs/cern.ch/user/t/trigrate/testarea/rel16", dest="testarea",         help="path to work Area")
p.add_option( '--log', type="string", default="/Trigger/TrigCost/TrigCostRate/scripts/CostMon.log", dest="log",         help="path to log file")
p.add_option( '--upperLim', type="int", default=1000000, dest="upperLim",help="Sets the upper limit so that you can run old runs without rewriting over the newer ones")#next 154500
p.add_option( '--newStart', type="int", default=154000, dest="newStart",help="Sets the lower limit for running over old runs")
p.add_option('--runFull',   action='store_true', default=False,          dest="runFull",   help="run over all files")
(options, args) = p.parse_args()

from collections import namedtuple
import cx_Oracle
import os,commands
import time
from datetime import datetime

#########
# Write area
#########
writearea='/afs/cern.ch/user/t/trigcomm/scratch0'

print "done import files"
def getCastorDirectoryList(path):
    cmd  = 'nsls %s/ ' % (path)
    if path.count('/eos/'):
        cmd  = 'eos ls %s/ ' % (path)
    file = ["%s" %i for i in commands.getoutput(cmd).split('\n')]
    return file

def getFirstStableLB(run):
    try:
        from TrigCostCool import LBReadForPhysics
        return LBReadForPhysics(run)
    
    except:
        return -1

def getLastStableLB(run):
    try:
        from TrigCostCool import LBReadForPhysics
        return LBReadForPhysics(run,getMin=False)
    
    except:
        return -1

def getMAXRUNNR(cu):
    q = "SELECT MAX(RUNNR) FROM SFO_TZ_RUN WHERE RUNNR<1000000"
    cu.execute(q)
    for r in cu.fetchall():
        return r[0]
def getCostMonRUNlist(cu,currrun,alreadydone):
    records='RUNNR,T_STAMP'
    F=namedtuple('F',records)
    q="SELECT %s FROM SFO_TZ_FILE WHERE RUNNR<=%s and RUNNR>=%s and (STREAM like '%%CostMonitoring' or STREAM like '%%ProcTimeout' or STREAM like '%%HltTimeout' or STREAM like '%%MissingData' or STREAM like '%%LateEvents' or STREAM like '%%ForcedAccept' or STREAM like '%%PtCrash' or STREAM like '%%StopTransition' or STREAM like '%%HltError')"%(records,str(currrun),alreadydone)
    cu.execute(q)
    runlist=[]
    for r in  map(F._make, cu.fetchall()):
        if r.RUNNR not in runlist:
            runlist.append(int(r.RUNNR))
    runlist.sort()
    return runlist

def getallRUNlist(cu,currrun,alreadydone,ctime):
    records = 'RUNNR,T_STAMP,STREAM'
    F=namedtuple('F',records)
    q="SELECT %s FROM SFO_TZ_FILE WHERE RUNNR<=%s and RUNNR>=%s"%(records,str(currrun),alreadydone)
    cu.execute(q)

    t = time.mktime(ctime)
    t=t-3*3600.0
    fintime=time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(t))
    RunTstamp=[]
    runlist=[]
    hasCostMon=[]
    allruns=[]
    for r in map(F._make, cu.fetchall()):
        if r.RUNNR not in allruns:
            allruns.append(str(r.RUNNR))
        wtime=time.strftime(str(r.T_STAMP))
        if wtime<fintime and r.RUNNR not in runlist and r.RUNNR!=currrun:
            runlist.append(int(r.RUNNR))
        if r.RUNNR==currrun:
            RunTstamp.append(r.T_STAMP)
    #inputStream=['efdProcTimeout','L2ProcTimeout','L2HltTimeout','EFHltTimeout','L2MissingData','EFMissingData','LateEvents','L2ForcedAccept','EFForcedAccept','efdPtCrash','L2PtCrash']
        if str(r.STREAM).count("CostMonitoring") or str(r.STREAM).count("ProcTimeout") or str(r.STREAM).count("HltTimeout") or str(r.STREAM).count("MissingData") or str(r.STREAM).count("LateEvents") or str(r.STREAM).count("ForcedAccept") or str(r.STREAM).count("PtCrash") or str(r.STREAM).count("StopTransition") or str(r.STREAM).count("EFHltError") or str(r.STREAM).count("L2HltError"):
            hasCostMon.append(int(r.RUNNR))
    #takes care of adding the run if the run last completed over three hours ago
    maxRunTstamp=str(ctime)
    if len(RunTstamp):
        maxRunTstamp=str(max(RunTstamp))
    if maxRunTstamp<str(fintime) and currrun not in runlist:
        runlist.append(currrun)
    #takes care of the fact that some of the files may have costmon and were already submitted. This keeps double submitting
    #print "hasCostMon:",hasCostMon
    if len(hasCostMon)>0:
        maxCostMon=max(hasCostMon)
        #print "maxCostMon:",maxCostMon
        maxrunlist=alreadydone
        if len(runlist):
            maxrunlist=max(runlist)
        while maxCostMon>maxrunlist:
            maxrunlist_temp=maxrunlist+1
            #print "maxrunlist_temp:",maxrunlist_temp
            if maxrunlist_temp not in runlist and str(maxrunlist_temp) in allruns:
                runlist.append(maxrunlist_temp)
            maxrunlist=maxrunlist+1
    if len(runlist):
        runlist.sort()
    #print "allruns:",allruns
    #print "runlist",runlist
    return runlist

def getStartTime(runnr):
    q="SELECT MIN(T_STAMP) FROM SFO_TZ_FILE WHERE RUNNR=%s"%runnr
    cu.execute(q)
    
    for r in cu.fetchall():
        return r[0]
def getEndTime(runnr,ctime):
    q="SELECT MAX(T_STAMP) FROM SFO_TZ_FILE WHERE RUNNR=%s"%runnr
    cu.execute(q)
    t = time.mktime(ctime)
    t=t-3600.0
    fintime=time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(t))
    for r in cu.fetchall():
        if str(r[0])>str(fintime):
            return "RUN still in progress or files still be processed"
        else:
            return "RUN and file processing ended at:"+str(r[0])
def getCostMonStreams(runnr):
    #inputStream=['efdProcTimeout','L2ProcTimeout','L2HltTimeout','EFHltTimeout','L2MissingData','EFMissingData','LateEvents','L2ForcedAccept','EFForcedAccept','efdPtCrash','L2PtCrash']
    records='RUNNR,STREAM'
    F=namedtuple('F',records)
    q="SELECT %s FROM SFO_TZ_FILE WHERE RUNNR=%s and (STREAM like '%%CostMonitoring' or STREAM like '%%ProcTimeout' or STREAM like '%%HltTimeout' or STREAM like '%%MissingData' or STREAM like '%%LateEvents' or STREAM like '%%ForcedAccept' or STREAM like '%%PtCrash' or STREAM like '%%StopTransition' or STREAM like '%%HltError')"%(records,runnr)
    cu.execute(q)
    streams=[]
    for r in map(F._make, cu.fetchall()):
        if str(r.STREAM) not in streams:
            streams.append(str(r.STREAM))
    return streams
def getSubmittedStreams(item,runInfo):
    streams=[]
    lbs=-1
    for it in runInfo:
        if len(it):
            if int(it[0])==int(item):
                numStreams=len(it)
                for tin in it:
                    if tin !=it[0] and tin not in streams:
                        streams.append(str(tin))
    #print "the streams collected for run:",str(item)
    print streams
    return streams,lbs
def getRecentRun(cu,runnr,ctime):
    q="SELECT MAX(T_STAMP) FROM SFO_TZ_FILE WHERE RUNNR=%s"%runnr
    cu.execute(q)
    t = time.mktime(ctime)
    t=t-3*3600.0
    fintime=time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(t))
    for r in cu.fetchall():
        if str(r[0])>str(fintime):
            return 1
        else:
            return 0

def writeNewStream(runInfo,item,stream,lb=-1):
    bin=0
    inrunInfo=0
    for it in runInfo:
        if len(it):
            if int(it[0])==int(item):
                inrunInfo=1
                runInfo[bin].append('%s %s' %(stream,lb))
        bin=bin+1
    if not inrunInfo:
        runInfo.append([])
        runInfo[bin].append(int(item))
        runInfo[bin].append('%s %s' %(stream,lb))

def addRun(runInfo,item):
    bin=0
    inrunInfo=0
    print "item:",item
    for it in runInfo:
        if len(it):
            print "iter:",it        
            if int(it[0])==int(item):
                inrunInfo=1
        bin=bin+1
    if not inrunInfo:
        runInfo.append([])
        runInfo[bin].append(int(item))
        
def cpToT0(cu,run,stream):
    #
    # Look to see if files have been copied to castor
    #
    #/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data10_7TeV/debug_all/0 RUN#/
    castorRoot="/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/"
    records='LFN,STREAM'
    F=namedtuple('F',records)
    q="SELECT %s FROM SFO_TZ_FILE WHERE RUNNR=%s and STREAM like '%s'"%(records,run,stream)
    cu.execute(q)
    fname=""
    for r in map(F._make, cu.fetchall()):
        fname=r.LFN
        break;
    dataType=fname[:fname.find('.')]
    print "dataType: ",dataType.strip()
    castorRoot=castorRoot+dataType.strip()+'/'
    print "castorROOT:",castorRoot
    if(stream.count('CostMonitoring')):
        castorRoot=castorRoot+'calibration_'+stream+'/'
    else:
        castorRoot=castorRoot+'debug_all/'
    castorRoot=castorRoot+'0'+run+'/'
    print "castor path:",castorRoot
    castordir=getCastorDirectoryList(castorRoot)
    print castordir
    csubmit=False
    if len(castordir)>0:
        if not castordir[0].count(': No such file or directory'):                        
            csubmit=True
        else:
            print "no such file or directory"
            castorRoot=castorRoot.rstrip('0'+run+'/')
            castorRoot=castorRoot.rstrip('/')
            castorRoot=castorRoot+'/00'+run+'/'
            print "Second Look at Castor path:",castorRoot
            castordir=getCastorDirectoryList(castorRoot)
            if len(castordir)>0:
                if not castordir[0].count(': No such file or directory'):
                    csubmit=True
    if csubmit:
        print "castor path Successfully copied to Castor!!!"
        return True;
    else:
        return False;

class RunAndStream:

    def __init__(self, year, stream, runnr, stableLB, lastLB, currRUNNR, submittedStream):

        self.year            = year
        self.stream          = stream
        self.runnr           = runnr
        self.stableLB        = stableLB
        self.lastLB          = lastLB
        self.currRUNNR       = currRUNNR
        self.submittedStream = submittedStream

    def runCommand(self, isBatch, runFull):
        if not isBatch:
            script = 'run_sfo_run.py'
            if runFull:
                script = 'run_doug_run.py'

            output_path = '%s/' %(writearea)
            output='00%s-%s' %(item,stream)
            if self.stableLB>-0.5:
                output+='-LB%s' %(self.stableLB)
            output_path+=output
            runcommand='python %s/Trigger/TrigCost/TrigCostRate/scripts/run_doug_run.py --currRun=%s --data --do-time --runnr=%s --stream=%s -o %s %s &> logfiles/log_' %(options.testarea,self.currRUNNR,self.runnr,self.stream,output_path,output,self.year) 
        else:
            script = 'run_sfo_runs.sh'
            if runFull and not self.stream.count('CostMonitoring'):
                script = 'run_doug_runs.sh'

            runcommand='bsub -q %s -J job.log  source %s/Trigger/TrigCost/TrigCostRate/scripts/%s %s %s %s %s %s' %(options.bqueue,options.testarea,script,options.testarea,self.runnr,self.stream,self.year,self.stableLB)

            if self.submittedStream and self.stableLB>0:
                runcommand+=' %s' %(int(self.stableLB)+15)  
            elif self.stableLB>-0.5:
                runcommand+='; %s %s' %(runcommand,int(self.stableLB)+15)  

            if self.lastLB>self.stableLB:
                begLB=self.stableLB
                while begLB>self.lastLB:
                    runcommand+='; bsub -q %s -J job.log source %s/Trigger/TrigCost/TrigCostRate/scripts/%s %s %s %s %s %s' %(options.bqueue,options.testarea,'run_lbs_runs.sh',options.testarea,self.runnr,self.stream,self.year,begLB,int(begLB)+100)
                    begLB+=100

        return runcommand
    
if __name__=="__main__":

    print "RUNNING findRUNNR.py"
    
    if not os.path.exists('waitfiles'):
        os.mkdir('waitfiles')
    if not os.path.exists('logfiles'):
        os.mkdir('logfiles')
    if not os.path.exists('donefiles'):
        os.mkdir('donefiles')

    print "opening list of previous runs"

    fin = open(options.testarea+options.input,'r+')
    print "input file:",fin
    prevFoundRun=[]
    runInfo=[]
    ink=-1
    for line in fin:
        line_test=line.strip()
        line_test=line.lstrip("\n")
        line_test=line.rstrip("\n")
        if line_test.isdigit():
            ink=ink+1
            prevFoundRun.append(int(line_test))
            runInfo.append([])
            runInfo[ink].append(int(line_test))
        else:
            runInfo.append([])
            stream_and_stableLB=str(line_test).split(' ')
            if len(stream_and_stableLB)>0:
                runInfo[ink].append(stream_and_stableLB[0])
    #print "the run info:",runInfo
    fin.close()
    currtime=time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(time.time()))
    currtime=time.strptime(currtime,"%Y-%m-%d %H:%M:%S")
    fout=open(options.testarea+options.log,'a+')

    print "Connecting to Oracle database"
    co = cx_Oracle.connect("ATLAS_SFO_T0_R/readmesfotz2008@atlas_config")
    cu = co.cursor()
    fout.write("\n"+"Looking for new runs at "+str(currtime)+"\n")
    currRUNNR=getMAXRUNNR(cu)
    print "Current RUNNR: ",currRUNNR
    #
    # Writing files to submit once they have copied to castor
    #
    JobsNotCpToCastor=[]
    checkSubmition = open(options.testarea+'/Trigger/TrigCost/TrigCostRate/scripts/waitToSubmit.dat','r+')
    for entry in checkSubmition:
        comm=entry.strip()
        #comm: bsub -q atlasb1 -J job.log -o log.log  source /afs/cern.ch/user/t/trigrate/testarea/rel16/Trigger/TrigCost/TrigCostRate/scripts/run_doug_runs.sh /afs/cern.ch/user/t/trigrate/testarea/rel16 198920 L2ProcTimeout 2011
        print 'comm:',comm
        the_script =(comm.strip())[comm.find(' source ')+len(' source '):]
        the_arguments = the_script.split(' ')
        if len(the_arguments)>4:
            crun     = the_arguments[2] 
            cstream  = the_arguments[3]
        else:
            print 'Could not understand: ',comm 
            continue
        
        print "stream: "+cstream.strip()+" runnr: "+crun.strip()
        if cpToT0(cu,crun.strip(),cstream.strip()):
            print "RunCommand:",comm
            os.system(comm)
        else:
            print "Not copied to castor: ",comm
            JobsNotCpToCastor.append(comm)
    checkSubmition.close()
    print "Done checking that old runs are copied to castor!"
    runs_stillgoing=[]
    if len(prevFoundRun):
        lastchecked=max(prevFoundRun)
        for item in prevFoundRun:
            if getRecentRun(cu,item,currtime) and int(item)>currRUNNR-50:
                runs_stillgoing.append(str(item))
        if len(runs_stillgoing):
            lastchecked=max(runs_stillgoing)
    else:
        lastchecked=currRUNNR
    if options.upperLim<int(lastchecked):
        print "going to be an old run"
        lastchecked=options.newStart
        currRUNNR=options.upperLim

    CostMonRUNlist=getCostMonRUNlist(cu,currRUNNR,lastchecked)
    print "About to collect all runs:from ",currRUNNR
    allRUNlist=getallRUNlist(cu,currRUNNR,lastchecked,currtime)
    print "the current runnr is:",str(currRUNNR)
    print "the last checked is:",str(lastchecked)
    for ru in runs_stillgoing:
        if str(ru) not in allRUNlist:
            allRUNlist.append(str(ru))
    print "runs_stillgoing:",runs_stillgoing
    print "allRUNlist:",allRUNlist
    for item in allRUNlist:
        print "item/runnr:",item

        #
        # Set the Year
        #
        year='2011'
        if int(item)>200000:
            year='2012'
            
        submittedStreams=[]   
        addRun(runInfo,item)
        runInfo_streams,stableLB=getSubmittedStreams(item,runInfo)
        print "run:",item
        print "submitted streams:",runInfo_streams
        if int(item) not in prevFoundRun or str(item) in runs_stillgoing:
            runstatus=": found run with start time:"+str(getStartTime(str(item)))+":"+str(getEndTime(str(item),currtime))
            if item in CostMonRUNlist:
                CostMonStreams=getCostMonStreams(str(item))
                for stream in CostMonStreams:
                    if not runstatus.count(str(stream)) and stream not in runInfo_streams and (stream not in submittedStreams or stableLB<0):
                        fout.write("\n"+"RUN: "+str(item)+" has CostMonitoring with streams:")
                        runstatus=runstatus+"\n   "+"Call selectFILE.py because there are CostMon files for this run! \n      The streams submitted are:"
                        fout.write("\nstream:"+str(stream))
                        runstatus=runstatus+"\n      "+str(stream)

                        #
                        # Check for the first stable LB
                        #
                        lastLB=getLastStableLB(item)
                        if int(stableLB)<0:
                            stableLB=getFirstStableLB(item)
                            runLBs=' %s' %(stableLB)
                        if stableLB<0:
                            runLBs=''

                        job_desc = RunAndStream(year, stream, item, stableLB, lastLB, currRUNNR, stream in submittedStreams)

                        runcommand = job_desc.runCommand(len(options.bqueue), options.runFull)
                            
                        fout.write("runcommand:"+runcommand+"\n")
                        fout.write("For more info please see log file: "+options.testarea+"/Trigger/TrigCost/TrigCostRate/scripts/logfiles/log_"+str(item)+"_"+str(stream)+".log")
                        submittedStreams.append(str(stream))
                        writeNewStream(runInfo,item,stream,stableLB)
                        if options.upperLim>int(item):
                            if cpToT0(cu,str(item),stream):
                                os.system(runcommand)
                            else:
                                if runcommand not in JobsNotCpToCastor:
                                    JobsNotCpToCastor.append(runcommand)
                            
                           #writing the run status page!
                            Quest = namedtuple('Quest', 'runnr, stream, nFilesT0, nEventsT0, processStat, note, nFilesSFO, nEventsSFO')
                            quest=Quest(runnr=str(item),stream=str(stream),nFilesT0="N/A",nEventsT0="N/A",processStat="Submitted to selectFILE.py",note="",nFilesSFO="N/A",nEventsSFO="N/A")
                            runStatusFile=open(writearea+'/t0cost_archives/logs/runStatus_'+str(item)+'_'+str(stream)+'_html.dat',"w")
                            runStatusFile.write(quest.runnr+'\n'+quest.stream+'\n'+quest.nFilesT0+'\n'+quest.nEventsT0+'\n'+quest.processStat+'\n'+quest.note+'\n'+quest.nFilesSFO+'\n'+quest.nEventsSFO)
                            runStatusFile.close()
                            
            elif item not in CostMonRUNlist:
                runstatus=runstatus+"\n   "+"No Cost Monitoring for this RUN!"
            fout.write("\n"+str(item)+runstatus)  
    fout.write("\nThe current run is:"+str(currRUNNR))
    fout.write("\nSleeping for 1000 seconds. Then will reawaken to look for more runs!")
    fout.close()
    fin = open(options.testarea+options.input,'w')
    for RUN in runInfo:
        streams=[]
        if len(RUN):
            fin.write(str(RUN[0])+"\n")
        else:
            print "ERROR Length of new run info is wrong",RUN
        if len(RUN):
            numStreams=len(RUN)
            for tin in RUN:
                if not str(str(tin).strip()).isdigit() and tin not in streams:
                    streams.append(str(tin))
                    #stream_and_stable=lbstr(tin).split(' ')
                    #if len(stream_and_stable)>1:
                    #    streams.append(str(stream_and_stable[0]))
                    fin.write(str(tin)+"\n")
    fin.close()
    #
    # Writing files to submit once they have copied to castor
    #
    waitToSubmit = open(options.testarea+'/Trigger/TrigCost/TrigCostRate/scripts/waitToSubmit.dat','w')
    for entry in JobsNotCpToCastor:
        waitToSubmit.write(entry+'\n')
    waitToSubmit.close()
    
