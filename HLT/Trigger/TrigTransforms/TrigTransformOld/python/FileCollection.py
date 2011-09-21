#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: Hegoi Garitaonandia <hegoi.garitaonandia@cern.ch>
# Classes to provide additional functionalities on top to the EFormat i/o stream module. 
#
# --2009-10-19  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - logging improved;
#   - cleanup;
#   - handling of multiple input files into a single stream implemented.
# --2009-12-03  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - versioning via the amiTag implemented;


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
import xml.dom.minidom
import time
import shutil
import re
from copy import *
from TrigTransformOld.EventHistorySummary import *

from  TrigTransformOld.Utils import *
#from  TrigTransformOld.Streamer import *

import eformat
from eformat import *
import TrigTransformOld.EventDump


file_log = logging.getLogger("BatchHLTApp.FileCollection")


def mergeHistory(all=[]):
        merged = EventHistorySummary()
        for s in all:
                merged.add(s)
        return merged

def cleanName(core_name):        
        # we need something better than that
        prefix = core_name
        for f in os.listdir('.'):
                if        re.compile(prefix).match(f):
                        file_log.info("file %s removed from working dir" % f)
                        os.unlink(f)

class ProcessAbleFileSet:
        """Provides some unix file operation capabilities on top of eformat.(i/o)stream module. Eformat is not enough because it does not allow to get the physical names of the files that belong to a certain stream, rename them in block, etc. This class also provides some functionality based on eformat."""        
        def __init__(self,core_name=None,files=None,location=None):
                # either core_name or files is provided, not both.
                # if core name is provided search for files that match that core_name regexp.
                self.summary        =None
                self.list        =None
                if location and location[len(location)-1]!="/": 
                        dire = location+"/"
                else:        dire = "./"
                if core_name and not files:
                        listfile = []
                        if location and location.find("/castor/")!=-1:
                                (ret, out, err)        =  runcmd(["nsls",location])
                                if ret == 0:        listfile=out.split()
                        else: listfile=os.listdir(dire)

                        file_log.info("creating ProcessableFileSet with core_name %s" % core_name)
                        if location:
                                self.files = [dire+f for f in listfile if re.compile(core_name).match(f)]
                        else: self.files   = [f for f in listfile if re.compile(core_name).match(f)]
                
                elif not core_name and files:
                        if location:self.files = [ dire+f for f in files]
                        else            :self.files = files

                else:
                        raise Exception("either core_name or files must be provided")

                temp=istream(self.files)
                del(temp)
                self.buildSummary(full=False)

        def getEventInfo(self):
                return self.summary,self.list

        def buildSummary(self,full=False):
                """Returns a dictionary with all the HLT information in a similar way to HLTErrorAnalysisJob. This information is packed in such a way that can be added to the event history."""
                # summary and list contain the same information (with either one you can 
                #        build the other), but they have different ways of indexing. 
                # the keys are the global_ids
                summary        ={}
                # the array is ordered by the relative position of the event within the stream (not the file)
                list        =[]
                count        =0
                for file in self.files:        
                        try:
                                input= istream(file)
                                for event in input: 
                                        if full:
                                                # the next is just an example of what can be done this should 
                                                # be filled in by event_analysis2(input), or event better, by analyze_this_event(event) 
                                                # for the moment I convert to string the stream tags object
                                                chains = EventDump.dump_chains(event)
                                                eventinfo =        {                                                \
                                                                        'streams':str(event.stream_tag()),        \
                                                                        'lumi':event.lumi_block(),                \
                                                                        'run_no':event.run_no(),                \
                                                                        'chains':chains,                        \
                                                                }
                                        else:
                                                eventinfo = None                
                
                                        # location is always necessary to keep track of the guys
                                        # for the moment I only set a default file to be the first, 
                                        # but I have to take into account several files, reseting 
                                        # counter, etc.
                                        location =                 {                                        \
                                                                        'file':file,                        \
                                                                        'offset':count,                        \
                                                                }
                                        fullInfo=                {                                        \
                                                                        'global_id':event.global_id(),        \
                                                                        'location':location,                \
                                                                        'hlt_info':eventinfo,                \
                                                                }
                                        summary[ event.global_id() ] = fullInfo
                                        list.append(fullInfo)
                                        count+=1
                                del(input)
                        except NameError:
                                file_log.info("cannot open file")
                                summary = None
                                raise 
                        except IOError:
                                file_log.info("cannot open file")
                                summary= None
                                raise         
                self.summary        =summary
                self.list        =list

        def rename2(self,newname):
        # for the moment
                self.files[0] = renameIt(self.files[0],newname)

        def merge(self,processable):
                pass

        def fileLumiBlock(self):
                try:
                        #lb = int(self.files[0].split('.')[5].split('_lb')[1])
                        #lblis = [int(f.split('.')[5].split('_lb')[1]) for f in self.files]
                        lblis = []
                        for f in self.files:
                                for i in f.split('.'):
                                        if i.find('_lb')!=-1: 
                                                lblis.append(int(i.split('_lb')[1]))
                        for l in lblis:
                                if l!=lblis[0]: return 0
                        return lblis[0]
                        
                except:
                        return 0
                        
        def fileSize(self):
                # needs to be adapted to several files
                size = 0
                for file in self.files:
                        tmp = fileSize(file)
                        if tmp: size += int(tmp)
                return size
        
        def fileSummary(self):
                summary=None        
                try:
                        ist= istream(self.files)
                        count=0
                        for eve in ist:
                                count = count + 1 
                        summary={'eventnumber':count}
                        if count==0:
                                file_log.info("no events in the file")
                                summary = None
                        del(ist)

                except NameError:
                        file_log.info("cannot open file")
                        #raise
                        summary = None
                except IOError:
                        file_log.info("cannot open file")
                        summary= None

                return summary

        def Size(self):
                return self.fileSize(self.files)

class UnProcessedFileSet(ProcessAbleFileSet):
        """Simple un-processed file. Not related to  ProcessingInfo objects yet."""
        def __init__(self,core_name=None,files=None):
                ProcessAbleFileSet.__init__(self,core_name,files)

class ProcessedFileSet(ProcessAbleFileSet):
        def __init__(self,core_name=None,files=None):
                ProcessAbleFileSet.__init__(self,core_name,files)

        #        self.L2Stats                = None
        #        self.EFStats                = None
                self.logs                = {}
                self.EventHistSummary        = None

        def setLogFile(self,type,log):        
                self.logs[type]=log

        def getLogs(self):
                file_log.info("logfiles associated to %s are:" % " ".join(self.files))
                for f in self.logs.values():
                        file_log.info("   %s" % f.name )

                return self.logs.values()

        def splitIt(self,eventnum,prefix):
                """Splits the fileset into two. Returns a pair of ProcessedDataSet (ret,newFile,newBad) together with a 0 in case there is something left, or 1 in case there is nothing left."""

                inputname        =self.myfile

                newFile                ="%s_good"  % prefix.split('.RAW')[0]
                newBad                 ="%s_bad"   % prefix.split('.RAW')[0]
                
                try:
                        newFile = '%s.RAW%s' % (newFile,prefix.split('.RAW')[1])
                        newBad         = '%s.RAW%s' % (newBad,prefix.split('.RAW')[1])
                except:
                        pass
                        
                newFileSet        =None
                newBadSet        =None

                file_log.info("splitting into two ...")
                ########################################
                ist= istream(self.files)
                num= eventnum
                
                good = eformat.ostream(core_name=newFile)
                bad = eformat.ostream(core_name=newBad)
                
                file_log.info( "before we have %d events"         % len(ist[:(num-1)]))
                file_log.info( "after we have %d events"          % len(ist[num:]))
                file_log.info( "splitting 1 event into bad"          )
                file_log.info( "   good events into file %s" % newFile)
                file_log.info( "   bad  events into file %s" % newBad)

                for eve in ist[:(num-1)]:
                        good.write( eformat.write.FullEventFragment(eve) )
                for eve in ist[num:]:
                        good.write( eformat.write.FullEventFragment(eve) )
                
                bad.write(ist[num-1])
                del(bad)
                del(good)
        
                # in case we don't have more good events to process 
                if len(ist[:(num-1)])==0 and  len(ist[num:]) == 0:
                        ret=1
                        newFileSet=ProcessedFileSet(core_name=newFile)
                        newFileSet.logs=deepcopy(self.logs)
                # otherwise go ahead
                else:
                        ret=0
        
                newBadSet=ProcessedFileSet(core_name=newBad)
                newBadSet.logs=deepcopy(self.logs)
                
                ########################
                return (ret,newFile,newBad)


        def splitIn3(filename):
                pass

        def streamIt(self):
                """Returns the ProcessedFileSets"""
                pass

        

class ProcessingInfo:
        """ProcessingInfo is a type of object that goes associated to a single ProcessedDataFileSet, and describes some part of the processing stage. It can only point to objects that are contained insde the ProcessedDataFileSet to whom it refers. Processing info objects get accumulated upon stages of processing."""
        def __init__(self):
                pass

class LogFile(ProcessingInfo):
        def __init__(self,name):
                file_log.info('creating LogFile object with name %s' % name)
                ProcessingInfo.__init__(self)
                self.name=name
                
        def rename2(self,newname):
                self.name = renameIt(self.name,newname)        

        def tail(lines):
                cmd="tail -%d %s" % (lines, name)
                (ret, out, err)        =  runcmd(cmd.split(' '))
                if ret!=0: out='error in tail'
                return out

class Stats:
        """It is a root object, that contains statistics about the processing of the ProcessedFileSet to which it is associated. Note that it will also include the statistics for the rejected events."""
        def __init__(self):
                pass

                
                
                
def getProcessedName2(runprefix=None,jobid=None,stream_type="unstreamed",stream_name="unstreamed"):
        name  = "%s.%s_%s.dbgrec.RAW.%s"        % (runprefix,stream_type,stream_name,jobid)                
        return name

def getProcessedName(filename,runname=None,jobid=None):
        try:
                tags=filename.split('.')
                tags.pop()
                filenumber=tags.pop()
                tags.append('processed')
                outputname = '.'.join(tags)
        except Exception,e:
                file_log.warning(e)
                outputname    = 'default'
                filenumber        = 1
                if runname:
                        outputname = runname
                if jobid:        
                        filenumber = jobid
        return (outputname,filenumber)

def fileSize(filename):
        if(filename.find("/castor/")!=-1):
                cmd = "nsls"
                arg = "-l"
                pos = 4
        else:
                cmd = "du"
                arg = filename
                pos = 0
                
        (ret, out, err)        =  runcmd([cmd,arg,filename])
        if ret != 0: 
                return None
        temp=out.split()
        size=temp[pos]
        return size

def renameIt(name,newname):
        try:           
                os.rename(name,newname)
                file_log.info( "file %s renamed to %s" % (name,newname) )
                return newname
        except OSError:        
                file_log.warning("weird: no log file found with name %s" % name)
        
        return newname
