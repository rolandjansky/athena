# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: Hegoi Garitaonandia <hegoi.garitaonandia@cern.ch>
# Module implementing the class for holding the EventHistory 
# of the reprocessing job.
#
# --2009-10-19  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - addStreamingGood()  method implemented

import logging
import time
import os
import sys
import signal
import select
import getopt
import pickle
import pprint
import string
import time
import shutil
import re
from copy import *

from  TrigTransformOld.Utils import *

import eformat
from eformat import *
import TrigTransformOld.HTMLCreation

class EventHistorySummary:
        """It is an object that contains a python dictionary ordered by event.global_id(). The values of the table is the history of that event through processing stages and the history through files. EventGlobalID->[History(Level,Status,Time,File,Content)]. FileHistory is a pair (filename,offset). Note for me: take a look at how to incorporate ErrorAnalysis info here."""
        def __init__(self,path='.'):
                self.summary={}
                self.path = path 

        def _appendHistory(self,gid,level,status,time,files,contentchange):
                pass

        def serialize(self,filename):
            thefile=file(filename,'w')        
            pickle.dump(self.summary,thefile)
        
        def load(self,filename):         
                thefile=file(filename,'r')        
                self.summary=pickle.load(thefile)

        def loadFromSandBox(self):
                self.load(os.path.join(self.path,'history.pickle'))

        def appendSimpleHistory(self,level,outputSummary):
                summary = self.summary
                for gid in outputSummary.keys():
                        if not summary.has_key(gid):
                                self.summary[gid]=[]
                        fullInfo=outputSummary[gid]
                        location = fullInfo['location']
                        hlt_info = fullInfo['hlt_info']
                        status          = "DEFAULT"
                        time          = 0
                        entry = (level,status,time,location,hlt_info)
                        self.summary[gid].append(entry)
                # simple summar

        def addStreamingGood(self,level,outputSummary):
                summary = self.summary
                for gid in outputSummary.keys():
                        if not summary.has_key(gid):
                                self.summary[gid]=[]
                        fullInfo=outputSummary[gid]
                        location = fullInfo['location']
                        hlt_info = fullInfo['hlt_info']
                        status          = "PASS"
                        time          = 0
                        entry = (level,status,time,location,hlt_info)
                        self.summary[gid].append(entry)
        
        
        def addProcessingGood(self,level,timesArray,outputSummary):
                # has to remove all the guys that are not in the file 
                # for all the events that are in the outputSummary we 
                # add an entry that says they have passed
                times={} 
                for (offset, gid, etime) in timesArray:
                        times[gid] = etime

                for gid in outputSummary.keys():
                        if not self.summary.has_key(gid):
                                self.summary[gid]=[]
                        fullInfo=outputSummary[gid]
                        location = fullInfo['location']
                        hlt_info = fullInfo['hlt_info']
                        status          = "PASS"
                        time          = times[gid]
                        entry = (level,status,time,location,hlt_info)
                        self.summary[gid].append(entry)
                # for all the events that are NOT in the outputSummary we 
                # add an entry that says they have not passed

        def addProcessingCrash(self,level,timesArray,outputSummary):
                times={}
                for (offset, gid, etime) in timesArray:
                        times[gid] = etime
        
                for gid in outputSummary.keys():
                        if not self.summary.has_key(gid):
                                self.summary[gid]=[]
                        fullInfo=outputSummary[gid]
                        location = fullInfo['location']
                        hlt_info = fullInfo['hlt_info']
                        status   = "CRASH"
                        time     = times[gid]
                        entry = (level,status,time,location,hlt_info)
                        self.summary[gid].append(entry)
        
        # has to remove all the guys that are not in the file 
        # for all the events that are in the outputSummary we 
        # add an entry that says they have passed

        def addProcessingTimeout(self,level,timesArray,outputSummary):
                pass
                # has to remove all the guys that are not in the file 

        def add(self,delta):
                # has to remove all the guys that are not in the file 
                for gid in delta.summary.keys():
                        # perform some checks for not overwriting
                        # self.summary.has_key(gid):
                        # not a copy only a reference
                        self.summary[gid]=delta.summary[gid]

        def buildFromSandBoxes(self,directories):         
                # first reset
                for dir in directories:
                        try:        
                                event_history =  EventHistorySummary(dir)
                                event_history.loadFromSandBox()
                                self.add(event_history)
                        except Exception, e: 
                                print e
                        self.finalize()        

        def finalize(self):
                self.serialize(os.path.join(self.path,'history.pickle'))

        def report(self,dirs):
                pass

        def toHTML(self):
                #header =  "%20s %10s %10s %20s %20s %20s %20s" % ("GID","EFStatus","L2Status","EFTime","L2Time","File" "Offset")
                header = ["GID","L2Status","EFStatus","L2Time","EFTime","File","Offset"]
                tohtml = [header]
#                thefile.write(header+'\n')
                for gid in self.summary.keys():
                        inputfile=None
                        inputoff= None
                        l2stat         =None
                        l2time         =None
                        efstat         =None
                        eftime         =None
                        for (step,status,thetime,location,info) in self.summary[gid]:
                                if step == "COPY_IN":
                                        inputfile = location['file']
                                        inputoff  = location['offset']
                                if step == "EF_PROCESS":
                                        efstat=status
                                        eftime=thetime
                                if step == "L2_PROCESS":
                                        l2stat=status
                                        l2time=thetime
                        #line = "%20d %10s %10s %10.2f %10.2f %30s %5d" % (gid,l2stat,efstat,l2time,eftime,inputfile,inputoff)
                        line =  [gid,l2stat,efstat,l2time,eftime,inputfile,inputoff]
                        tohtml.append(line)
                        #thefile.write(line+'\n')
                #thefile.close()
                t1  = HTMLCreation.table2D(tohtml)
                return t1
                
                

