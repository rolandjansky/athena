# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# RunLister.py

import sys
import time
from PyCool import cool
from CoolConvUtilities.AtlCoolLib import RangeList,indirectOpen
from DetectorStatus.DetStatusCoolLib import statusCutsToRange

def timeRep(value):
    """Utility function to turn COOL 63bit time into string, or unknown"""
    if (value is not None):
        stime=int(value/1000000000L)
        return time.strftime('%F:%T',time.gmtime(stime))
    else:
        return 'unknown'

def boolRep(value):
    """Utility function to turn bool into yes/no"""
    if (value is not None):
        if (value):
            return 'Yes'
        else:
            return 'No'
    else:
        return '-?-'

def noneZero(value):
    "Utility function to turn None into 0 (int) for use with integer/None args"
    if (value is None):
        return 0
    else:
        return int(value)

def noneStr(value):
    "Utility to turn None into '---' (string) for use with string/None args"
    if (value is None):
        return "---"
    else:
        return str(value)

def maskDecode(mask):
    """Decode the detector mask into a string list of detectors"""
    # data taken from get_detectormask.cc from Giovana L, August 2007
    masklist=['Pix B','Pix EA','Pix EC','Pix B-layer','SCT BA','SCT BC','SCT EA','SCT EC','TRT BA','TRT BC','TRT EA','TRT EC','LAr EMBA','LAr EMBC','LAr EMECA','LAr EMECC','LAr HECA','LAr HECC','LAR FCALA','LAR FCALC','Til BA','Til BC','Til EA','Til EC','MDT BA','MDT BC','MDT EA','MDT EC','RPC BA','RPC BC','TGC EA','TGC EC','CSC EA','CSC EC','L1calo pre','L1calo cluDAQ','L1calo cluRoI','L1calo JetEDAQ','L1calo JetERoI','MUCTPI','CTP','L2SV','SFI','SFO','LVL2','EF']
    res=""
    for i in range(0,64):
        if (mask & (1 << i)): res+=masklist[i]+", "
    # chop off last comma-space
    if len(res)>1: res=res[:-2]
    return res


class LBParams:
    """Class to hold information from the LB_Params folder"""
    def __init__(self,run,lb,start,stop):
        self.run=run
        self.lb=lb
        self.start=start
        self.stop=stop

class RunParams:
    """class to hold information from the Params folders"""
    def __init__(self,run,start,runtype,daqconfig,detmask,filetag,rec,datasource):
        self.run=run
        self.start=start
        self.runtype=str(runtype)
        self.daqconfig=str(daqconfig)
        self.detmask=int(detmask)
        self.filetag=str(filetag)
        # in M4, the logic on the recording flag is wrong!
        self.rec=bool(not rec)
        self.datasource=str(datasource)
        # informaton to be filled in from EOR_Params
        self.stop=None
        self.totaltime=None
        self.cleanstop=None
        # information to be filled in from LB_Params
        self.maxlb=None
        # information to be filled in from FinalSFOStat
        self.partname=None
        self.recevents=None
        self.storedevents=None
        # information to be filled in from EventCounters
        # partname and storedevents can come from EventCounters or FinalSFOStat
        self.l1events=None
        self.l2events=None
        self.efevents=None
        self.errcode=0
        self.errlist=[]

    def addEORInfo(self,stop,totaltime,cleanstop,maxlb):
        self.stop=stop
        self.totaltime=totaltime
        self.cleanstop=cleanstop
        if maxlb is not None:
            self.maxlb=maxlb

    def updateEORInfo(self,daqconfig):
        if (self.daqconfig=="unknown"):
            self.daqconfig=daqconfig
        else:
            if (self.daqconfig!=daqconfig):
                if (self.loglevel>1):
                    self.addMsg("EOR updates DAQConfig")
                
    def addSFOStat(self,partname,recevents,storedevents):
        self.partname=partname
        self.recevents=int(recevents)
        self.storedevents=int(storedevents)
        
    def addEventCounters(self,partname,l1events,l2events,efevents,storedevents):
        self.partname=partname
        self.l1events=l1events
        self.l2events=l2events
        self.efevents=efevents
        if (self.storedevents is None or self.storedevents==0):
            self.storedevents=int(storedevents)

    def patchEndTime(self,stop):
        "Patch end of run time from Trigger info when missing EOR record"
        self.stop=stop
        self.totaltime=int((stop-self.start)/1.E9)
        self.setErr(4)

    def setErr(self,bit,msg=""):
        "Set errorcode for bit x"
        self.errcode=self.errcode | (1 << bit)
        if (msg!=""): self.errlist+=[msg]

    def addMsg(self,msg):
        self.errlist+=[msg]

    def decodeErr(self):
        "Decode the errors for the run into a string representation"
        # Error bits are as follows
        # 0 NoEOR    no EOR record found
        # 1 BadEOR   EOR and SOR inconsistent (NYI)
        # 2 NoLB_Params  No LB_Params record found for SOR
        # 3 BadLB_Params  Bad LB_Params record found for SOR (NYI)
        # 4 PatchEOR EOR information patched from trigger information
        
        errtxt=['NoEOR','BadEOR','NoLBParams','BadLBParams',
                'PatchEOR']
        errstr='Run %i errcode %i : ' % (self.run,self.errcode)
        for i in range(0,len(errtxt)):
            if (self.errcode & (1 << i)): errstr+=errtxt[i]+' '
        return errstr
        


class TrigLBParams:
    "Class to hold info on runs and LBs read from TRIGGER schema"
    def __init__(self,run,lb,start,stop):
        self.run=run
        self.maxlb=lb
        self.start=start
        self.stop=stop
        

class coolRunLister:
    """Extract information on ATLAS runs from online COOL information"""
    def __init__(self,cooltdaqdbconn,cooltrigdbconn,coolstatusdbconn,
                 oracle=False,loglevel=1):
        debug=(loglevel>1)
        try:
            self.cooldb=indirectOpen(cooltdaqdbconn,True,oracle,debug)
            if (loglevel>=1):print "Connected to",cooltdaqdbconn,"for RunControl data"
        except Exception,e:
            print e
            sys.exit(-1)
        if (len(cooltrigdbconn)>0):
            try:
                self.cooltrigdb=indirectOpen(cooltrigdbconn,True,oracle,debug)
                if (loglevel>=1): print "Connected to",cooltrigdbconn,"for CTP data"
                self.usetrig=True
            except Exceptioe,e:
                print e
                sys.exit(-1)
        else:
            self.usetrig=False
        if (len(coolstatusdbconn)>0):
            try:
                self.coolstatusdb=indirectOpen(coolstatusdbconn,True,oracle,debug)
                if (loglevel>=1): print "Connected to",coolstatusdbconn,"for detector status data"
            except Exception,e:
                print e
                sys.exit(-1)
        
        # store other parameters
        self.loglevel=loglevel
        self.coolpath='/TDAQ/RunCtrl'
        self.cooltlbpath='/TRIGGER/LUMI'
        self.nowtime=time.time()*1000000000L
        # no restriction on initial selection
        self.onlyRec=False
        self.mask=0
        self.runType=""
        self.filenameTag=""

        # intial values
        self.lbrunmap={}
        self.runmap={}
        self.triglbmap={}

    def close(self):
        "Close the database connections"
        self.cooldb.closeDatabase()
        self.cooltrigdb.closeDatabase()
        self.cooldb=None
        self.cooltrigdb=None

    def setSelection(self,onlyRec=False,mask=0,runType="",filenameTag="",detStatus="",detStatusTag="HEAD"):
        self.onlyRec=onlyRec
        self.mask=mask
        self.runType=runType
        self.filenameTag=filenameTag
        self.detstatus=detStatus
        self.detstatustag=detStatusTag

    def listFromTime(self,time1=cool.ValidityKeyMin,time2=cool.ValidityKeyMax):
        "Main entry point - setup and check data given a range of times"
        self.lbrunmap=self.runsFromTime(time1,time2)
        # set the limits on run and time from the list of runs returned
        self.minrun=(1 << 31)-1
        self.maxrun=0
        self.mintime=cool.ValidityKeyMax
        self.maxtime=cool.ValidityKeyMin
        for (run,value) in self.lbrunmap.items():
            if (self.loglevel>1): print "listFromTime: Process run",run
            if (run<self.minrun): self.minrun=run
            if (run>self.maxrun): self.maxrun=run
            if (value.start<self.mintime): self.mintime=value.start
            if (value.stop>self.maxtime and value.stop<cool.ValidityKeyMax): self.maxtime=value.stop
        if (self.loglevel>0):
            print "Run range [%i,%i] times (%s,%s)" % (self.minrun,self.maxrun,timeRep(self.mintime),timeRep(self.maxtime))
        # now get the runmap
        self.runmap=self.runsFromSEOR(self.minrun,self.maxrun)
        # add in the time/LB information and check consistency
        self.correlateLB()
        if (self.usetrig):
            self.correlateTrigger()
            
    def listFromRuns(self,run1=0,run2=(1 << 31)-1):
        "Main entry point - setup and check data given inclusive range of runs"
        self.runmap=self.runsFromSEOR(run1,run2)
        # set the limits on run and time from the list of runs returned
        self.minrun=(1 << 31)-1
        self.maxrun=0
        self.mintime=cool.ValidityKeyMax
        self.maxtime=cool.ValidityKeyMin
        for runp in self.runmap.values():
            run=runp.run
            if (run<self.minrun): self.minrun=run
            if (run>self.maxrun): self.maxrun=run
            if (runp.start<self.mintime): self.mintime=runp.start
            if (runp.start>self.maxtime): self.maxtime=runp.start
            if (runp.stop is not None and runp.stop>self.maxtime): self.maxtime=runp.stop
        # get the LBParams info based on the derived time range
        self.lbrunmap=self.runsFromTime(self.mintime,self.maxtime)
        # add in the time/LB information and check consistency
        self.correlateLB()
        if (self.usetrig):
            self.correlateTrigger()
        

    def runsFromTime(self,time1=cool.ValidityKeyMin,time2=cool.ValidityKeyMax):
        """Query /TDAQ/RunCtrl/LB_Params to get list of runs/LBs in time range,
        returning a map of run numbers to LB_Params objects"""
        folderLB_Params=self.cooldb.getFolder(self.coolpath+'/LB_Params')
        itr=folderLB_Params.browseObjects(time1,time2,cool.ChannelSelection.all())
        srun=-1
        runlist={}
        while itr.goToNext():
            obj=itr.currentRef()
            # ignore openended IOVs which start before the time interval
            # these will be unclosed LB records from old runs
            if (obj.until()<cool.ValidityKeyMax or obj.since()>=time1):
                payload=obj.payload()
                run=payload['RunNumber']
                lb=payload['LuminosityBlock']
                if (run!=srun):
                    # seeing a new run - store old one if needed
                    if (srun>-1):
                        runlist[srun]=LBParams(srun,slbmax,sstart,send)
                    srun=run
                    slbmax=lb
                    sstart=obj.since()
                    send=obj.until()
                else:
                    if (lb>slbmax): slbmax=lb
                    if (obj.until()>send): send=obj.until()
                    if (obj.since()<sstart): sstart=obj.since()
        itr.close()
        # store last run
        if (srun>-1):
            runlist[srun]=LBParams(srun,slbmax,sstart,send)
        if (self.loglevel>0): print "Run list from LB_Params has %i entries" % len(runlist)
        return runlist

    def runsFromSEOR(self,run1=0,run2=(1 << 31)-1):
        """Query /TDAQ/RunCtrl/LB_Params to get details of runs in runrange
        Use both SOR_Params and EOR_Params to catch runs which ended badly.
        Return a map of runs to RunParams objects"""
        # get detector status information if needed
        iov1=run1 << 32
        iov2=(run2+1) << 32
        if (iov2>cool.ValidityKeyMax): iov2=cool.ValidityKeyMax
        if (self.detstatus!=""):
            if (self.loglevel>0):
                print "Applying detector status cuts: %s" % self.detstatus
            gooddetstatus=statusCutsToRange(self.coolstatusdb,'/GLOBAL/DETSTATUS/LBSUMM',iov1,iov2,self.detstatustag,self.detstatus)
        else:
            gooddetstatus=RangeList(iov1,iov2)
            
        runlist={}
        folderSOR_Params=self.cooldb.getFolder(self.coolpath+'/SOR_Params')
        itr=folderSOR_Params.browseObjects((run1 << 32),(run2 << 32),cool.ChannelSelection.all())
        while itr.goToNext():
            obj=itr.currentRef()
            payload=obj.payload()
            run=payload['RunNumber']
            # correction for inverted RecEnabled logic here
            if (self.checkSelection(not payload['RecordingEnabled'],payload['DetectorMask'],payload['RunType'],payload['FilenameTag']) and len(gooddetstatus.getAllowedRanges(run << 32, (run+1) << 32))>0):
                runlist[run]=RunParams(run,payload['SORTime'],payload['RunType'],payload['DAQConfiguration'],payload['DetectorMask'],payload['FilenameTag'],payload['RecordingEnabled'],payload['DataSource'])
        itr.close()
        if (self.loglevel>0):
            print "SOR_Params has data for %i runs" % len(runlist)

        # now query EOR_Params and fill in missing info
        neor=0
        folderEOR_Params=self.cooldb.getFolder(self.coolpath+'/EOR_Params')
        itr=folderEOR_Params.browseObjects((run1 << 32),(run2 << 32),cool.ChannelSelection.all())
        while itr.goToNext():
            obj=itr.currentRef()
            payload=obj.payload()
            run=payload['RunNumber']
            #maxlb=(obj.until() & 0xFFFFFFF)-1
            #if (maxlb==0 or maxlb==0xFFFFFFFF): maxlb=None
            maxlb=None
            # note that run may not be found if filtered from SOR list by
            # selection
            if (run in runlist.keys()):
                runlist[run].addEORInfo(payload['EORTime'],payload['TotalTime'],payload['CleanStop'],maxlb)
                runlist[run].updateEORInfo(payload['DAQConfiguration'])
                neor+=1
        if (self.loglevel>0):
            print "EOR_Params has data for %i runs" % neor
        itr.close()
        
        # now query FinalSFOStats and fill in event counts
        nsfo=0
        folderSFOStat=self.cooldb.getFolder(self.coolpath+'/FinalSFOStat')
        itr=folderSFOStat.browseObjects((run1 << 32),(run2 << 32),cool.ChannelSelection.all())
        while itr.goToNext():
            obj=itr.currentRef()
            payload=obj.payload()
            run=obj.since() >> 32
            if (run in runlist.keys()):
                runlist[run].addSFOStat(payload['PartitionName'],payload['ReceivedEvents'],payload['StoredEvents'])
                nsfo+=1
        itr.close()
        if (self.loglevel>0):
            print "FinalSFOStat has data for %i runs" % nsfo

        # now query EventCounters and fill in trigger counts/partition name
        nevc=0
        folderEvtCount=self.cooldb.getFolder(self.coolpath+'/EventCounters')
        itr=folderEvtCount.browseObjects((run1 << 32),(run2 << 32),cool.ChannelSelection.all())
        while itr.goToNext():
            obj=itr.currentRef()
            payload=obj.payload()
            run=obj.since() >> 32
            if (run in runlist.keys()):
                runlist[run].addEventCounters(payload['PartitionName'],payload['L1Events'],payload['L2Events'],payload['EFEvents'],payload['RecordedEvents'])
                nevc+=1
        itr.close()
        if (self.loglevel>0):
            print "EventCounters has data for %i runs" % nevc

        return runlist

    def correlateLB(self):
        """Go through the runmap and lookup LB info, also flag missing EOR
        in the error structures"""
        for runp in self.runmap.values():
            run=runp.run
            if run in self.lbrunmap.keys():
                lbinfo=self.lbrunmap[run]
                if (runp.maxlb is None):
                    runp.maxlb=lbinfo.lb
            else:
                runp.setErr(2)
            if runp.cleanstop is None:
                runp.setErr(0)
                
    def checkSelection(self,rec,mask,runtype,tag):
        "Check whether run passes selection cuts"
        if (self.onlyRec and not rec): return False
        # mask - all specified bits must be set
        if ((self.mask & mask) != self.mask): return False
        # run type and filenametag: specified string must be included in value
        if (self.runType!="" and runtype.upper().find(self.runType.upper())==-1): return False
        if (self.filenameTag!="" and tag.find(self.filenameTag)==-1): return False
        return True

    def correlateTrigger(self):
        "Retrieve run/LB information from trigger and correlate with RunCtrl"
        srun=-1
        self.triglbmap={}
        folderTrigLBLB=self.cooltrigdb.getFolder(self.cooltlbpath+'/LBLB')
        itr=folderTrigLBLB.browseObjects((self.minrun << 32),((self.maxrun+1) << 32),cool.ChannelSelection.all())
        while itr.goToNext():
            obj=itr.currentRef()
            since=obj.since()
            run=since >> 32
            lb=since & 0xFFFFFFFF
            payload=obj.payload()
            start=payload['StartTime']
            end=payload['EndTime']
            if (run!=srun):
                # seeing a new run - store old one if needed
                if (srun>-1):
                    self.triglbmap[srun]=TrigLBParams(srun,slbmax,sstart,send)
                srun=run
                slbmax=lb
                sstart=start
                send=end
            else:
                if (lb>slbmax): slbmax=lb
                if (start<sstart): sstart=start
                if (end>send): send=end
        itr.close()
        # store last run
        if (srun>-1):
            self.triglbmap[srun]=TrigLBParams(srun,slbmax,sstart,send)
        if (self.loglevel>0):
            print "Trigger LB map has data for %i runs" % len(self.triglbmap)
        
        # now loop through primary run list and add trigger information
        nbad=0
        npatch=0
        for runp in self.runmap.values():
            run=runp.run
            if run in self.triglbmap.keys():
                triglb=self.triglbmap[run]
                if (runp.stop is None):
                    if (self.loglevel>1):
                        print "Trigger end time will substitute %i %i" % (run,triglb.stop)
                    runp.patchEndTime(triglb.stop)
                    npatch+=1
            else:
                nbad+=1
        if (self.loglevel>0):
            print "Missing trigger information for %i runs, patched EOR for %i" % (nbad,npatch)

    def listErrors(self):
        "List runs which have errors to text output"
        nerr=0
        for runp in self.runmap.values():
            if (runp.errcode>0):
                print runp.decodeErr()
                if (len(runp.errlist)>0):
                    for i in runp.errlist:
                        print "Run %i %s" % (runp.run,i)
                nerr+=1
        print "Total of %i runs with errors" % nerr

    def listRuns(self,format="",lastfirst=False):
        "List run details to text output"
        if (self.loglevel>=1):
            title="     Run   Events  LumiB"
            if ('t' in format):
                title+='            StartTime             StopTime'
            if ('a' in format):
                title+=' L1Events L2Events EFEvents'
            if ('e' in format):
                title+=' ErrC'
            if ('r' in format):
                title+=' Rec Cln'
            if ('c' in format):
                title+=' RunType                  DetectorMask'
            if ('d' in format):
                title+=' DAQConfiguration     PartitionName    FilenameTag         '
            print title
        runkeys=self.runmap.keys()
        runkeys.sort(reverse=lastfirst)
        for irun in runkeys:
            runp=self.runmap[irun]
            line="%8i %8i %6i" % (runp.run,noneZero(runp.storedevents),noneZero(runp.maxlb))
            if ('t' in format):
                line+=' %20s %20s' % (timeRep(runp.start),timeRep(runp.stop))
            if ('a' in format):
                line+=' %8i %8i %8i' % (noneZero(runp.l1events),noneZero(runp.l2events),noneZero(runp.efevents))
            if ('e' in format):
                line+=' %4i' % runp.errcode
            if ('r' in format):
                line+=' %3s %3s' % (boolRep(runp.rec),boolRep(runp.cleanstop))
            if ('c' in format):
                line+=' %-20s %16x' % (runp.runtype,runp.detmask)
            if ('d' in format):
                line+=' %-20s %-16s %-20s' % (runp.daqconfig,noneStr(runp.partname),runp.filetag)
            print line

        

    def makeWebSummary(self,filename,header,lastfirst=False):
        htfile=open(filename,'w')
        htfile.write("""<head>
<title>Run Summary for %s</title></head>
<body>
<h2>COOL/RunControl Run Summary for %s</h2>

Data extracted from /TDAQ/RunCtrl SOR_Params, EOR_Params and LB_Params folders
for runs %i to %i (times %s to %s) on <b>%s</b>. All time are in <b>UTC</b>,
two hours behind Geneva local time. <font color=\"008000\">Green</font> runs
were recorded with clean stop, <font color=\"FF0000\">red</font> runs were
recorded without clean stop or RunCtrl EOR record.
Black runs were not recorded.
<p>""" % (header,header,self.minrun,self.maxrun,timeRep(self.mintime),timeRep(self.maxtime),timeRep(self.nowtime)))
        htfile.write("<table border=\"0\">\n<tr align=\"left\"><th>Run</th><th>Events</th><th>L1</th><th>L2</th><th>EF</th><th>MaxLB</th><th>Start time</th><th>Stop time</th><th>Duration</th><th>Rec</th><th>Clean</th><th>RunType</th><th>DetMask</th><th>DAQConfig</th><th>Partition</th><th>FilenameTag</th></tr>\n")
        # loop over the runs
        detmasklist=[]
        runkeys=self.runmap.keys()
        runkeys.sort(reverse=lastfirst)
        #for runp in self.runmap.values():
        for irun in runkeys:
            runp=self.runmap[irun]
            if (runp.storedevents is not None):
                storeevt=str(runp.storedevents)
            else:
                storeevt="---"
            if (runp.maxlb is not None):
                maxlb=str(runp.maxlb)
            else:
                maxlb='---'
            start=timeRep(runp.start)
            stop=timeRep(runp.stop)
            if (runp.totaltime is not None):
                tottime=str(runp.totaltime)
            else:
                tottime='---'
            rec=boolRep(runp.rec)
            clean=boolRep(runp.cleanstop)
            # set colour based on status
            col='000000' # black for non-recorded runs
            if (runp.rec):
                if (runp.cleanstop==True):
                    col='008000' # green - for runs with EOR and cleanstop
                else:
                    col='FF0000' # red - for runs without EOR or cleanstop
            detmask=runp.detmask
            if (detmask==0):
                ldetmask=""
            else:
                ldetmask="<a href=#maskdecode%x>" % detmask
                if (detmask not in detmasklist): detmasklist+=[detmask]
            htfile.write("<tr style=\"color:#%s\"><td>%i</td><td>%s</td><td>%i</td><td>%i</td><td>%i</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s%x</td><td>%s</td><td>%s</td><td>%s</td></tr>\n" % (col,runp.run,storeevt,noneZero(runp.l1events),noneZero(runp.l2events),noneZero(runp.efevents),maxlb,start,stop,tottime,rec,clean,runp.runtype,ldetmask,detmask,runp.daqconfig,noneStr(runp.partname),runp.filetag))
        htfile.write("</table>\n")
        htfile.write("<p>\nTotal of %i runs analysed\n<p>" % len(self.runmap))
        # add information on decoding the detmask
        detmasklist.sort()
        htfile.write("Detector mask decoding for these runs:\n<ul>\n")
        for mask in detmasklist:
            htfile.write("<li><a name=maskdecode%x><b>%x</b>: %s</li>\n" % (mask,mask,maskDecode(mask)))
        htfile.write("</ul>\n")
        htfile.write("\n</body>\n")
        htfile.close()
        
