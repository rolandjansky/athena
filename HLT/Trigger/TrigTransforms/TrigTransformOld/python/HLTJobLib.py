#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Author: Hegoi Garitaonandia <hegoi.garitaonandia@cern.ch>
# Implement the basic tasks used in the main processing sequence 
#
# --2009-10-19  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - DummyStreamer added;
#   - CTP reconstruction updated to the latest format of the CTP/Muctpi;
#   - logging improved;
#   - file naming schema changed;
#   - cleanup and few fixes;
#   - handling of multiple input files into a single stream implemented.
# --2009-12-03  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - versioning via the amiTag implemented;
# --2009-12-09  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - trigger histograms merged into 1 single file per Level (L2 and EF);
#   - lumi block tag added to the histogram files
# --2010-02-05  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - class for filter output file added
# --2010-03-03  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - root file name creation modified to be compliant with 'debug_all' dataset name
# --2010-03-05  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - Build recovery dataset filters out debug stream events from the accepted ones
# --2010-04-07  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - Add class for handling the remapping of the LVL1 items
# --2010-09-04  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - Add protection to handle cases when the output is missing
# 2011-07-11 Graeme Stewart <graeme.andrew.stewart@cern.ch>
#  * Improved exception handling to print errors



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
import glob
import shutil

from TrigTransformOld.BatchIPC import Result
from TrigTransformOld.CASTORapi import *


import eformat
from eformat import *
from TrigTransformOld.addL1 import *
from TrigTransformOld.replaceMUCTPI import *
from TrigTransformOld.FileCollection import *
from TrigTransformOld.Utils import *

HLTJob_log = logging.getLogger("BatchHLTApp.HLTJobLib")


# some files that can be retrieved by ganga if requested by end user with "--verbose" option
#athenaPTlog = 'athenaPT.log'
#athenaMTlog = 'athenaMT.log'
rfcpInlog   = 'rfcpIn.log'
rfcpOutlog  = 'rfcpOut.log'  
recoCTPlog  = 'recoCTP.log'  
athenaMT    = 'athenaMT.py'        
athenaPT    = 'athenaPT.py'        


# top class basic performance monitoring services
class Step: 
        """A Base class for all the rest. Provides timing and crashing with style services. It has a Result object as a class member, so that statistics for the different steps that are involved in a run can be accumulated. A Step will create statistics independently of whether the it succeded or not. An Step doesn't leave any temporary files except for a possible output file that will be used in following steps."""
        result=Result()
        history=EventHistorySummary()
        result_pickle_name  = "result.pickle"
        history_pickle_name = "history.pickle"
        
        def __init__(self,name='DEFAULT'):
                """Builds a step and names it."""
                self.name=name
                self.startime=0
                self.finish=0
                self.total=0

        def __str__(self):
                """Returns a string version of the object."""
                print "HERE GOES A STRING VERSION OF STEP"

        def getStats(self):
                return Step.result

        def getHistory(self):
                return Step.history

        def start(self):
                """Sets the start time."""
                self.startime=time.time()

        def stop(self):
                """Sets the stop time. And the total time."""
                self.finish=time.time()
                self.total = self.finish - self.startime

        def recoverable(self,msg,logfile=None):
                """Cleans up, serializes the statistics and exits with a crash report."""
                HLTJob_log.error(msg)
        #        cleanup()
                #self.result.setError(self.name,comment=tail(logfile,700))
                #self.result.serialize("result.pickle")
                #sys.exit(errorcode)

        def crash(self,msg,logfile=None,errorcode=1):
                """Cleans up, serializes the statistics and exits with a crash report."""
                HLTJob_log.error(msg)
                cleanup()
                if logfile:
                        comment=tail(logfile,700)
                else:
                        comment='no comment'
                self.result.setError(self.name,comment=comment)
                self.result.serialize(self.result_pickle_name)
                self.history.serialize(self.history_pickle_name)
                sys.exit(errorcode)

        def cancel(self,msg,logfile=None,errorcode=0):
                """Cleans up, serializes the statistics and exits with a report (this is not a crash, it's a good run that cannot continue)"""
                HLTJob_log.info(msg)
                cleanup()
                #self.result.setGood()
                self.result.serialize(self.result_pickle_name)
                self.history.serialize(self.history_pickle_name)
                sys.exit(errorcode)

        def toDict(self):
                """Returns a dictionary represenation of the object. It is used to add the statistics to the step. This is a function that will be overriden by the children classes.""" 
                return  {\
                                'name':self.name,  \
                                'time':self.total, \
                        }

        def wrapUp(self):
                self.result.addStep(self.toDict())


        def setHistoryName(self,name):
                self.history_pickle_name = name


        def setResultName(self,name):
                self.result_pickle_name = name

                        
        def setJobId(self,jobid):
                self.result.setJobId(jobid)

        def getJobId(self):
                try:
                        return self.result.result['jobid']
                except Exception, x:
                        self.crash("%s attempts to use the JobId while it is not set: %s" % (self.name, x))

class CastorIO(Step):
        """Provides rfcp basic functionality."""
        def __init__(self,name='COPY',svcclass='atlcal'):
                Step.__init__(self,name)
                self.svcclass = svcclass
        
        def copy(self,src,dst):
                """Performs the copy and writes the """
                self.start()
                #thelogfile=file(std,'w')
                erroracronym,tottime,lastcptime,allattempts =  rfcp(src,dst,maxattempt=1,maxtime=600,svcclass=self.svcclass)
                self.summary = {'erroracronym': erroracronym, 'tottime':tottime,'lastcptime':lastcptime,'allattempts':allattempts,'src':[src],'dst':[dst]}
                #thelogfile.close()
                self.stop()
                self.wrapUp()
                if erroracronym!='OK':
                    self.crash("error copying file from/to castor "+str(self.summary))

        def toDict(self):
                self.summary.update({'name':self.name,'time':self.total})
                return self.summary

#class Stager(Step):
#        """A class to copy input files from castor. Manages the stager, etc."""
#        def __init__(self):
#                Step.__init__(self,"STAGER")
#
#        def stageIt(self,file):
#                cmd="stager_get -S atlcal -M  %s" % file
#                self.start()
#                (ret, out, err)        =  runcmd( cmd.split(' ') )
#                HLTJob_log.info("file stage request submitted")
#                self.stop()
#                self.wrapUp()

#        def unstageIt(self,file):
#                cmd="stager_rm -S atlcal -M  %s" % file
#                self.start()
#                (ret, out, err)        =  runcmd( cmd.split(' ') )
#                HLTJob_log.info("file stage request submitted")
#                self.stop()
#                self.wrapUp()

class CastorDataIn(CastorIO):
        """A class to copy input files from castor. Manages the stager, etc."""
        def __init__(self):
                CastorIO.__init__(self,"COPY_IN")

        def get(self,src):
                # Jo, don't forget it can be more than one file, so you got 
                # to loop over the thing.
                dst=os.path.basename(src)
                HLTJob_log.info("copying file from castor...")
                HLTJob_log.info("remote name: " + src)
                HLTJob_log.info("local name: "  + dst)
                ret=self.copy(src,dst)
                HLTJob_log.info("file copied from castor")
                #theinput=UnProcessedFileSet(files=[dst])
                #theinput.buildSummary(full=True)
                #(initialSummary,initialList)=\
                #theinput.getEventInfo()
                #Step.history.appendSimpleHistory(self.name,initialSummary)
                #return theinput
                

class CastorDataOut(CastorIO): 
        """A class to copy output files to castor. Manages the stager, etc."""
        def __init__(self,svcclass='atlcal'):
                CastorIO.__init__(self,"COPY_OUT",svcclass)
        
        def putFileColl(self,src,dst,subdir=None):
                if not create_dir(dst):
                        self.cancel('cannot create %s dir to store %s' % (dst,src) )

                if subdir: dir = os.path.join(dst,subdir)
                else:      dir = dst

                if not create_dir(dir):
                        self.cancel('cannot create %s dir to store %s' % (dir,src) )

                for f in src.files:
                        HLTJob_log.info(' ... copying %s' % f)
                        HLTJob_log.info("into remote name: " + os.path.join(dir,f))
                        ret=self.copy(f,os.path.join(dir,f))
                HLTJob_log.info("output copied to destination")


        def putLogColl(self,src,dst,subdir=None):
                if not create_dir(dst):
                        self.cancel('cannot create %s dir to store %s' % (dst,src) )

                if subdir: dir = os.path.join(dst,subdir)
                else:      dir = dst

                if not create_dir(dir):
                        self.cancel('cannot create %s dir to store %s' % (dir,src) )

                HLTJob_log.info('sending output to %s' % dir)

                for l in src.getLogs():
                        HLTJob_log.info(' ... copying %s' % l)
                        HLTJob_log.info("into remote name: " + os.path.join(dir,l.name))
                        ret=self.copy(l.name,os.path.join(dir,l.name))
                HLTJob_log.info("output copied to destination")


        def putDataset(self,token,dst):
                src = [f for f in os.listdir('.') if re.compile(token).search(f)]
                if not create_dir(dst):
                        self.cancel('cannot create %s dir' % dst )

                try:
                        dir = os.path.join(dst,src[0].split(token)[0]+token)
                except Exception, x:
                        HLTJob_log.info('Error joining path in putDataset: %s', x)
                        return

                if not create_dir(dir):
                        self.cancel('cannot create %s dir' % dir )

                for f in src:
                        HLTJob_log.info(' ... copying %s' % f)
                        HLTJob_log.info("into remote name: " + os.path.join(dir,f))
                        ret=self.copy(f,os.path.join(dir,f))
                HLTJob_log.info("output copied to destination")



        def put(self,src,dst,subdir=None):
                logging.info("sending output to castor...")
                logging.info("local name: "  + " ".join(src.files))
                # this should be a loop
                if subdir:
                        dst=os.path.join(dst,subdir)
                        logging.info("remote name: " + os.path.join(dst,src.files[0]))
                ret=self.copy(src.files[0],os.path.join(dst,src.files[0]))
                logging.info("output sent to castor")


class DummyStreamer(Step):
        """A class to change the name of data file out from last athena processing step."""
        def __init__(self,name='DUMMY_STREAMER'):
                Step.__init__(self,name)
                self.summary={
                          'name':self.name,
                          'time': 0,                
                        'outputfiles': {},
                          'total_writen': 0,
                }
                
        def streamFile(self,token,stream):
                """Performs renaming of the file from last process step with the mv command"""
                
                HLTJob_log.info("Staring Dummy Streamer ...")
                
                matchstr = token+"_"
                
                self.start()
                flst = [f for f in os.listdir('.') if re.compile(matchstr).search(f)]
                if flst==None:
                        self.crash("DummyStreamer: could not find *%s* files" % token)
                
                #matchstr = token+"_"
                reg      = re.compile(matchstr)
                
                try:
                        num = [int(reg.split(f)[1].split('.RAW')[0]) for f in flst]
                        num.sort()
                        last = num[len(num)-1]
                except Exception, x:
                        self.crash("DummyStreamer: could not find last processing step for *%s* files: %s" % (token, x))


                full_token = token+"_"+str(last)
                files      = [f for f in flst if re.compile(full_token).search(f)] 
                dest_core  = files[0].split(full_token)[0]+stream+'.RAW'

                
                for f in files:                
                        try:
                                HLTJob_log.info(" processing file %s"% f) 
                                dest   = f.replace(full_token,stream)
                                HLTJob_log.info(" dest=%s"% dest )
                                shutil.move(f,dest)
                                num = ProcessedFileSet(files=[dest]).fileSummary()['eventnumber']
                                self.summary['outputfiles'].update( {dest: num} )
                                self.summary['total_writen'] += num
                        except Exception, x:
                                HLTJob_log.info(" File %s contains no events! Skipping ... (Exception was: %s)" % (f, x))
                                pass
                                #self.crash("DummyStreamer: error moving from %s to %s" %(files[0],dest_core))
                                
                streamed = ProcessedFileSet(dest_core)
                streamed.buildSummary(full=True)
                (eventSummary,eventList) = streamed.getEventInfo()
                stats    = streamed.fileSummary()
                Step().history.addStreamingGood(self.name,eventSummary)
                
                self.stop()
                
                self.summary['time']         = self.total
                #self.summary['outputfiles']  = streamed.files
                #self.summary['total_writen'] = stats['eventnumber']

                self.wrapUp()
                
                for f in streamed.files:
                        HLTJob_log.info("Output data streamed to %s" % f)
                
                return streamed

                
        def toDict(self):
                return self.summary

class BuildRecoveryFiles(Step):
        """A class to pack the processed events into three datasets:
              - accepted events
              - rejected events
              - crashed  events
           
             events are merged across lumi-blocks."""
        
        def __init__(self,prefix,suffix,tail,pver,name='RECOVERY_DATASETS_STREAMER'):
                Step.__init__(self,name)
                
                self.prefix = prefix
                self.suffix = suffix
                self.tail   = tail
                self.pver   = pver
                
                self.summary={
                          'name':self.name,
                          'time': 0,                
                        'outputfiles': {},
                          'total_writen': 0,
                }
                
        def packEvents(self,fileList,stream):
                """Packs events in the fileList into a single dataset"""
                
                event_list = []
                names_dict = {}
                
                if len(fileList)==0: 
                        return (event_list,names_dict,'')                
                
                #open the accept event file
                isf = eformat.istream(fileList[0])
                dr=EventStorage.pickDataReader(fileList[0])
                output = eformat.ostream(core_name="recov_evts",
                                         run_number=dr.runNumber(),
                                         trigger_type=dr.triggerType(),
                                         detector_mask=dr.detectorMask(),
                                         beam_type=dr.beamType(),
                                         beam_energy=dr.beamEnergy())

                st_type = stream.split('_')[1]
                for f in fileList:
                        ifs = eformat.istream(f)
                        for event in ifs:
                                doWrite = True
                                for t in event.stream_tag():
                                        if t.type=='debug' and st_type=='hltacc':
                                                doWrite=False
                                
                                if doWrite: 
                                        output.write(event.__raw__())        
                                        event_id = event.global_id()
                                
                                        error_msg = "BuildRecoveryFile: event id %d " % event_id +\
                                                    "repeated into the dataset for %s" % stream
                                
                                        if event_id in event_list:
                                                self.crash(error_msg)
                                        else:
                                                event_list.append(event_id)

                del output
                if self.pver!='':
                        tag = '%s.%s' % (self.suffix,self.pver)
                else:
                        tag = self.suffix
                filename = "%s.%s.%s._lb0000.%s" %     \
                                (self.prefix,stream,tag,self.tail)
        
                acfl = [f for f in os.listdir('.') if re.compile('recov_evts').search(f)]

                for f in acfl:
                        new_name = '%s.%s'%   \
                            (filename,'.'.join(f.split('.')[-2:]) )                
                        os.rename(f,new_name)
                        
                        try:
                                data = ProcessedFileSet(new_name)
                                evts = data.fileSummary()['eventnumber']
                        except Exception, x:
                                HLTJob_log.info('Exception in packEvents: %s' % x)
                                return (event_list,names_dict,'')
                        
                        names_dict.update( {new_name: evts} )
        
                return event_list, names_dict, filename

        
        def produceOutputDatasets(self,inputFiles,outputToken):
                """Performs the packing of the events into three datasets"""
                
                HLTJob_log.info("Staring to build the Recovery Output Datasets ...")
                                
                self.start()
                
                if outputToken:
                        # process accepted events

                        matchstr = outputToken+"_"
                
                        flst = [f for f in os.listdir('.') if re.compile(matchstr).search(f)]                
                        if len(flst)==0:
                                self.crash("BuildRecoveryFile: could not find *%s* files" % outputToken)
                
                        reg = re.compile(matchstr)
                
                        try:
                                num = [int(reg.split(f)[1].split('.RAW')[0]) for f in flst]
                                num.sort()
                                last = num[len(num)-1]
                        except Exception, x:
                                self.crash("BuildRecoveryFile: could not find last processing step for *%s* files: %s" % (outputToken, x))


                        full_token = outputToken+"_"+str(last)
                        files      = [f for f in flst if re.compile(full_token).search(f)] 
                
                        acc_ev,acc_fi,acc_na = self.packEvents(files,'debugrec_hltacc')
                else:
                        acc_ev = []
                        HLTJob_log.info("No events in output file!")

                
                #process crashed events
                files = [f for f in os.listdir('.') if re.compile('(Crash.RAW._crash)|(Error.RAW)').search(f)]
                if len (files) == 0:
                        HLTJob_log.info("No file with crashed events!")                
                
                cra_ev,cra_fi,cra_na = self.packEvents(files,'debugrec_crashed')

                
                
                #process rejected events
                list_of_processed = acc_ev + cra_ev
                
                isf = eformat.istream(inputFiles.files[0])
                dr=EventStorage.pickDataReader(inputFiles.files[0])
                output = eformat.ostream(core_name="rejected_events",
                                         run_number=dr.runNumber(),
                                         trigger_type=dr.triggerType(),
                                         detector_mask=dr.detectorMask(),
                                         beam_type=dr.beamType(),
                                         beam_energy=dr.beamEnergy())        
                
                haveRejection = False
                for f in inputFiles.files:
                        ifs = eformat.istream(f)
                        for event in ifs:
                                if event.global_id() not in list_of_processed:
                                        output.write(event.__raw__())
                                        haveRejection = True
                del output
                files = []
                if haveRejection:
                        files = [f for f in os.listdir('.') if re.compile('rejected_events').search(f)]

                
                if len(files) == 0:
                        HLTJob_log.info("No file with rejected events!")
                
                rej_ev,rej_fi,rej_na = self.packEvents(files,'debugrec_hltrej')
                
                
                
                # collect output and finalize
                total_events  = 0
                output_files  = {}
                recovery_data = []
                
                if len(acc_ev) != 0:
                        accepted = ProcessedFileSet(core_name=acc_na)
                        accepted.buildSummary(full=True)
                        (eventSummary,eventList) = accepted.getEventInfo()
                        Step().history.addStreamingGood(self.name,eventSummary)
                        output_files.update(acc_fi)
                        entry = (accepted,'debugrec','hltacc')
                        recovery_data.append(entry)
                        
                if len(cra_ev) != 0:
                        crashed = ProcessedFileSet(core_name=cra_na)
                        crashed.buildSummary(full=True)
                        (eventSummary,eventList) = crashed.getEventInfo()
                        Step().history.addStreamingGood(self.name,eventSummary)
                        output_files.update(cra_fi)
                        entry = (crashed,'debugrec','hltcrashed')
                        recovery_data.append(entry)
                
                if len(rej_ev) != 0:
                        rejected = ProcessedFileSet(core_name=rej_na)
                        rejected.buildSummary(full=True)
                        (eventSummary,eventList) = rejected.getEventInfo()
                        Step().history.addStreamingGood(self.name,eventSummary)
                        output_files.update(rej_fi)
                        entry = (rejected,'debugrec','hltrej')
                        recovery_data.append(entry)
                        
                
                self.stop()
                
                self.summary['time']         = self.total
                self.summary['outputfiles']  = output_files
                self.summary['total_writen'] = total_events

                self.wrapUp()
                
                HLTJob_log.info("Recovery Output Datasets built")
                
                return recovery_data

                
        def toDict(self):
                return self.summary


# HLT stuff
class HltStep(Step):
        def __init__(self,name,env):
                self.myenv=env
                Step.__init__(self,name)

        def getEnv(self):
                return self.myenv

class CtpReconstruction(HltStep): 
        def __init__(self,myenv):
                HltStep.__init__(self,'CTP_RECONSTRUCTION',myenv)

        def reconstruct(self,input,prefix):        
                cleanName(prefix)
                self.start()
                HLTJob_log.info("reconstructing CTP information ...")
                for f in input.files:
                        if f.find('.RAW.') != -1:
                            postfix = ".RAW."+f.split('.RAW.')[1]
                        else:
                            postfix = ''
                
                        outfile = '%s%s'%(prefix,postfix) 
                        CTPreco(f,outfile)
                
                        try:
                                CTPreco(f,outfile)                
                        except Exception, x:
                                self.crash("error reconstructing CTP: %s" % x)
                
                HLTJob_log.info("CTP information reconstructed")        
                self.stop()
                self.wrapUp()
                reconstructed=UnProcessedFileSet(core_name=prefix)
                reconstructed.buildSummary(full=False)        
                (eventSummary,eventList)=\
                        reconstructed.getEventInfo()
                #Step.history.appendSimpleHistory(self.name,eventSummary)
                Step.history.addStreamingGood(self.name,eventSummary)
                return reconstructed


class MuctpiFix(HltStep): 
        def __init__(self,myenv):
                HltStep.__init__(self,'MUCTPI_REPLACEMENT',myenv)

        def reconstruct(self,input,prefix):        
                cleanName(prefix)
                self.start()
                HLTJob_log.info("replacing MUCTPI information ...")
                for f in input.files:
                        if f.find('.RAW.') != -1:
                            postfix = ".RAW."+f.split('.RAW.')[1]
                        else:
                            postfix = ''
                
                        outfile = '%s%s'%(prefix,postfix) 
                
                        try:
                                ReplaceMUCTPI(f,outfile)                
                        except Exception, x:
                                self.crash("error replacing MUCTPI: %s" % x)
                
                HLTJob_log.info("MUCTPI information replaced")        
                self.stop()
                self.wrapUp()
                reconstructed=UnProcessedFileSet(core_name=prefix)
                reconstructed.buildSummary(full=False)        
                (eventSummary,eventList)=\
                        reconstructed.getEventInfo()
                #Step.history.appendSimpleHistory(self.name,eventSummary)
                Step.history.addStreamingGood(self.name,eventSummary)
                return reconstructed


class RemapLVL1(HltStep): 
        def __init__(self,name,module,myenv):
                HltStep.__init__(self,name,myenv)
                self.module = module
                
        def remap(self,input,prefix):        
                cleanName(prefix)
                self.start()
                HLTJob_log.info("remapping LVL1 items according to %s"%self.module)
                for f in input.files:
                        if f.find('.RAW.') != -1:
                            postfix = ".RAW."+f.split('.RAW.')[1]
                        else:
                            postfix = ''
                
                        outfile = '%s%s'%(prefix,postfix) 
                
                        try:
                                Import = 'from  mod_re%s import *' %   self.module 
                                Method = '%s(f,outfile)' % self.module
                                
                                HLTJob_log.debug(" executing ... %s" % Import)
                                exec(Import)
                                
                                HLTJob_log.debug(" executing ... %s" % Method)
                                exec(Method)
                        except Exception, x:
                                self.crash("error remapping LVL1: %s" % x)
                
                HLTJob_log.info("LVL1 items remapped according to %s"%self.module)        
                self.stop()
                self.wrapUp()
                remapped=UnProcessedFileSet(core_name=prefix)
                remapped.buildSummary(full=False)        
                (eventSummary,eventList)=\
                        remapped.getEventInfo()
                #Step.history.appendSimpleHistory(self.name,eventSummary)
                Step.history.addStreamingGood(self.name,eventSummary)
                return remapped


class FilterOutputFile(HltStep): 
        def __init__(self,myenv,filters):
                HltStep.__init__(self,'FILTER_OUTPUT_FILES',myenv)
                self.filters = filters
                self.summary={                        \
                        'name':self.name,        \
                        'time': 0,                \
                        'filters': filters,     \
                        'eventsin':0,                \
                        'eventsout':0,                \
                        'fileoutsize':0,        \
                        'fileinsize':0                \
                        }
        
        def filter(self,input,token):
                self.start()
                
                msg = "filtering output file according to "
                for fil in self.filters:
                        msg = msg+fil+" "
                HLTJob_log.info(msg)

                dataset_file_name = ''
                
                self.summary['fileinsize'] = input.fileSize( )
                
                for f in input.files:
                        pos = 0
                        fna = f.split('.')
                        for t in fna:
                                pos += 1
                                if re.compile(token).match(t):
                                        pos -= 1
                                        break

                        try:
                                tok = fna[pos].split('_')
                                tok[1] = str(int(tok[1])+1)
                                fna[pos] = '_'.join(tok) 
                        except Exception, x:
                                msg = 'Filter error: %s token not found in file name: %s' % (token, x)
                                self.crash(msg)
                
                        output_file_name  = '.'.join(fna)
                        dataset_file_name = '.'.join(fna[:pos+2]) 
                        
                        isf = eformat.istream(f)
                        dr=EventStorage.pickDataReader(f)
                        output = eformat.ostream(core_name="filter",
                                                 run_number=dr.runNumber(),
                                                 trigger_type=dr.triggerType(),
                                                 detector_mask=dr.detectorMask(),
                                                 beam_type=dr.beamType(),
                                                 beam_energy=dr.beamEnergy())

                        for event in isf:
                                self.summary['eventsin'] += 1
                                StreamTags = event.stream_tag()
                                wrote = False
                                #print StreamTags
                                for stream in StreamTags:
                                        t = stream.type+"_"+stream.name
                                        for fil in self.filters:
                                                if re.compile(fil).match(t):
                                                        if not wrote: 
                                                                #HLTJob_log.info ('%s'% type(event))
                                                                #HLTJob_log.info ('%s'% dir(event) )
                                                                output.write(event.__raw__())
                                                                self.summary['eventsout'] += 1
                                                                wrote = True                                                        
                                                        break
                        
                        tmp_file_name = output.last_filename()
                        del output
                        os.rename(tmp_file_name,output_file_name)
                
                try:
                        filtered=ProcessedFileSet(core_name=dataset_file_name)
                        filtered.buildSummary(full=False)
                except Exception, x:
                        HLTJob_log.info('Exception thown in filter: %s' % x)
                        self.summary['fileoutsize'] = 0
                        self.stop()
                        self.summary['time'] = self.total
                        self.wrapUp()
                        self.cancel("File in output of filter process does not contain events")
                
                (eventSummary,eventList)=\
                        filtered.getEventInfo()
                #Step.history.appendSimpleHistory(self.name,eventSummary)
                Step.history.addStreamingGood(self.name,eventSummary)
                self.summary['fileoutsize'] = filtered.fileSize( )
                
                HLTJob_log.info("Output file filtered")        
                self.stop()
                self.summary['time'] = self.total
                self.wrapUp()
                
                return filtered


        def toDict(self):
                return self.summary


class Analyze(HltStep):
        """A class that wraps around event header anlyze functions."""
        def __init__(self,env,name):
                HltStep.__init__(self,name,env)

        def analyze(self,input):
                cmd="SummaryAnalyze.py %s -1 %s" % (input.files[0],self.name+'_summary.root')
                #print cmd
                self.start()
                (ret, out, err)        =  runcmd( cmd.split(' ') )
                if ret!=0: 
                        self.recoverable("Analyze stage failed")
                        self.recoverable(err + out)
                self.stop()
                self.wrapUp()

class Merger(HltStep):
        def __init__(self,myenv):
                HltStep.__init__(self,'SPLIT_IN_2',myenv)

        def merge(self,input,filename):
                """Takes a list of files as input and merges them into a single one"""
                merge = eformat.ostream()
                for file in input:
                        ist=eformat.istream(file)
                        for event in ist:
                                merge.write( eformat.write.FullEventFragment(ist) )
                del(merge)

class SplitIn2(HltStep):
        def __init__(self,myenv,pver):
                HltStep.__init__(self,'SPLIT_IN_2',myenv)
                self.pver = pver

        def splitIt(self,inputname,eventnum,prefix):
                """Returns a pair (newFile,newBad)"""
                
                newBad  = prefix
                newFile = newBad.replace('._crash','._good')
                
                tg = '.'.join(prefix.split('.')[:-1])
                tb = tg.replace('RAW._crash','RAW._good')
                
                self.start()
                HLTJob_log.info( "splitting into two ..." )
                ###########################################

                ist= istream(inputname.files)
                num= eventnum
                
                
                cleanName(newFile)
                cleanName(newBad)
                good = eformat.ostream(core_name=newFile)
                bad  = eformat.ostream(core_name=newBad)
                
                HLTJob_log.info( "   before we have %d events" % len(ist[:(num-1)]))
                HLTJob_log.info( "   after we have %d events"  % len(ist[num:]))
                HLTJob_log.info( "   splitting 1 event into bad"  )
                
                for eve in ist[:(num-1)]:
                        good.write( eformat.write.FullEventFragment(eve) )
                for eve in ist[num:]:
                        good.write( eformat.write.FullEventFragment(eve) )
                
                bad.write( eformat.write.FullEventFragment(ist[num-1]) )
                del(bad)
                del(good)
                
                # rename the file
                fb = [f for f in os.listdir('.') if re.compile(tb).match(f)]
                fg = [f for f in os.listdir('.') if re.compile(tg).match(f)]
                
                try:
                        for f in fb:
                                dest   = '.'.join(f.split('.')[:-2]) + '._' + \
                                         self.getJobId() + '.' +              \
                                         '.'.join(f.split('.')[-2:])
                                if self.pver!='': 
                                        dest = dest.replace('RAW','RAW.%s'%self.pver)
                                shutil.move(f,dest)
                        
                        for f in fg:
                                dest   = '.'.join(f.split('.')[:-2]) + '._' + \
                                         self.getJobId() + '.'  +             \
                                         '.'.join(f.split('.')[-2:])
                                if self.pver!='': 
                                        dest = dest.replace('RAW','RAW.%s'%self.pver)
                                shutil.move(f,dest)  
                
                        if self.pver!='':
                                newBad  = newBad.replace('RAW','RAW.%s'%self.pver)
                                newFile = newFile.replace('RAW','RAW.%s'%self.pver)
                except Exception, x:
                        self.crash("HLTProcess, SplitIn2: error moving from %s to %s. %s" %(f, dest, x))
                
                HLTJob_log.info( "   good events into file %s" % newFile )
                HLTJob_log.info( "   bad  events into file %s" % newBad )
                
                # in case we don't have more good events to process 
                if len(ist[:(num-1)])==0 and  len(ist[num:]) == 0:
                        ret=1
                # otherwise go ahead
                else:
                        ret=0
        
                ########################
        
                if ret != 0:
                        HLTJob_log.warning("cannot split file more")
                        HLTJob_log.info("outputFile: None"         )
                        HLTJob_log.info("badFileSet: "+ newBad )
                        #os.rename('python_script._0101.data', newBad )
                        newBadSet=ProcessedFileSet(newBad)        
                        newFileSet=None        
                else:
                        # check if there are remaining events
                        
                        bad_files = [f for f in os.listdir('.') if re.compile(newBad).search(f)]
                        filesummary=fileSummary(bad_files,self.myenv)

                        if not filesummary:
                                self.crash("weird situation after spliting: no file summary")
                                newFile=None
                        else:
                                HLTJob_log.info("outputFile:"+ newFile)
                                HLTJob_log.info("badFile: "  + newBad)
                                newBadSet=ProcessedFileSet(newBad)        
                                newFileSet = ProcessedFileSet(newFile)        
                                HLTJob_log.info("file splited")
                # going on
                self.stop()
                self.wrapUp()
                return (newFileSet,newBadSet)


class HltProcess(HltStep):
        def __init__(self,env,config,name):
                HltStep.__init__(self,name,env)
                self.name       = name
                self.env        = env
                self.config     = config
                self.logfile    = config['logfile']
                self.base_name  = config['basename']
                self.pversion   = config['proc_version']
                self.lumi_block = config['lumi_block']
                self.summary={                        \
                            'name':self.name,        \
                            'time': 0,                \
                            'eventsin':0,                \
                            'eventsout':0,                \
                            'rootsize':0,                \
                            'fileoutsize':0,        \
                            'fileinsize':0                \
                       }

        def toDict(self):        
                return self.summary

        def checkInput(self,input):        
                size=input.fileSize( )
                if not size:
                        HLTJob_log.warning("no file file found")
                        self.crash("intermediate file has no events")
                else:        
                        self.summary['fileinsize']=size
                filesummary=input.fileSummary()
                if not filesummary:
                        self.cancel("intermediate file has no events")
                self.summary['eventsin']=filesummary['eventnumber']
                HLTJob_log.info("file has %d" % filesummary['eventnumber'])

        def checkOutput(self,output):        
                if output:
                        filesummary=output.fileSummary()
                        if not filesummary:
                                HLTJob_log.warning("intermediate file has no events")        
                        else:
                                self.summary['eventsout']=filesummary['eventnumber']
                                size=output.fileSize( )
                                if not size:
                                        HLTJob_log.warning("no file file found")
                                else:
                                        self.summary['fileoutsize']=size
                                HLTJob_log.info("file has %d" % filesummary['eventnumber'])
                else:
                        HLTJob_log.warning("no intermidiate file")        

        def checkRootFiles(self):
                for f in ['debug-monitoring','expert-monitoring','lbn-monitoring','runstat-monitoring','shift-monitoring']:
                        self.checkRootFile("%s.root" % f)

        def checkRootFile(self,rootfilename):
                        size=fileSize(rootfilename)        
                        if not size:
                                HLTJob_log.warning("no root file %s was produced",rootfilename)
                        else:        
                                self.summary['rootsize'] += int(size)

        def process(self,inputset,prefix,maxevents=0):
                """Single pass of processing through a file. Returns 0 in case everything went ok. Returns 1 in case a problem previous to the RUNNING state, and returns 2 in case of a problem during RUNNING state."""
                
                cleanName(prefix)
                total=None
                HLTJob_log.info("processing %s to %s with %s" %  
                               (",".join(inputset.files),prefix,self.name)    )
                self.start()
                processed=None
                thelogfile = file( self.config['logfile'],'w')
                
                #post_command_name = self.createPostCommand(doMagFromDCS=True,
                #                                          doIgnorePrescales=True)
                post_command_name = self.createPostCommand()
        
                cmd = [ self.config['application'],"-R",self.config['run_number']]
                if self.name=='L2_PROCESS' and self.config['l2_special_options']!='':
                        cmd.extend( self.config['l2_special_options'].split(' ') )
                cmd.extend( ["-C", '\'include(\"%s\")\'' % post_command_name] )
                #        "-C", '\'include(\"%s\")\'' % post_command_name ]
                if maxevents!=None: cmd.extend(["-n",str(maxevents)])
                #if len(inputset.files)>1:
                cmd.extend(["-f","[ \""+"\",\"".join(inputset.files)+"\" ]",\
                            "-o",prefix])
                #else:
                #        cmd.extend(["-f",inputset.files[0],"-o",prefix]
                if self.config['type']=='DB':
                        conne = self.config['connection']
                        for k,v in self.config['pairs'].items():
                                conne += ":%s=%s" % (k,v)
                        cmd.extend(["-J","TrigConf::HLTJobOptionsSvc",\
                         "--use-database",conne, "-Z",self.config['preload']])
                else:
                        cmd.extend(["-Z",self.config['preload'] ] )
                        if self.config['options']:
                                cmd.extend(["-c",self.config['options']])
                        cmd.extend([self.config['joboptions']])

                for mod in self.config['modEvent']:
                        cmd.extend(["-Z",mod])


                HLTJob_log.info("executing: %s" % " ".join(cmd) )
                (ret, out, err)        =  runcmd( cmd ,stdout=thelogfile, stderr=thelogfile , env=self.myenv )
                thelogfile.close()

                # in order to know what the last event took
                current        =        time.time()
                if ret != 0:
                    self.recoverable("error processing "+self.name,self.config['logfile'])
                #else:
                #    os.rename(tempfile+'._0001.data',outputname)
                
                #connect the output file name with the JobId
                prefix = self.renameOutputFiles(prefix)
                
                # now get the serialized result
                try:
                        # get the times of the events created by the athenaMT/PT preprocesor
                        f=file(self.config['pickle'])
                        eventtimes        =pickle.load(f)
                        current        =time.time()        
                        total        =[]
                        i=0
                        #HLTJob_log.info("we got")        
                        while i < len(eventtimes) - 1 :
                                (counter,gid,start)         =  eventtimes[i]
                                (countern,gidn,startn)         =  eventtimes[i+1]
                                total.append( (counter,gid,startn-start)        )
                                i+=1        
                        (counterl,gidl,startl)         =  eventtimes[i]
                        total.append( (i,gidl,current-startl))
        
                except OSError, x: 
                        HLTJob_log.warning('cannot load the serialized result: %s' % x)
                except IOError, x: 
                        HLTJob_log.warning('cannot load the serialized result: %s' % x)
        
                # analyze the combinations
                if total and len(total)!=0 and ret ==0:
                        # this is good
                        try:
                                processed=ProcessedFileSet(prefix)
                        except IOError:
                                HLTJob_log.warning("output file does not contain events.")
                                processed=None 
                        newret=0
                elif not total and ret !=0:
                        # this a big time crash (configuration, etc.)
                        processed = None
                        newret=1
                elif total and ret !=0:
                        # this a crash during processing
                        processed = None
                        newret=2
                else: 
                        self.crash('unknown combination in the result')        
                return (newret,total,processed)

        def run(self,inputset,prefix,maxevents):
                """Will never stop the run. Only report"""
                HLTJob_log.info("processing %s..."% self.name)
                HLTJob_log.info("input name: " + " ".join(inputset.files))
                HLTJob_log.info("output name: " + prefix)
                self.start()
                self.checkInput(inputset)
                (ret,total,processed) = self.process(inputset,prefix,maxevents)
                self.checkRootFiles()
                #self.checkRootFile(self.rootfile)
                self.checkOutput(processed)
                self.stop()
                self.summary['time'] = self.total
                self.wrapUp()
                return (ret,total,processed)

        def getNext(self):        
                """Returns a copy of the object"""
                from copy import deepcopy
                new     = deepcopy(self)
                HltProcess.__init__(new , self.getEnv(),config=self.config,name=self.name)
                return new

        def renameCostMonFiles(self):
                ratelis = [f for f in os.listdir('.') if re.compile('TrigRate').match(f)]
                costlis = [f for f in os.listdir('.') if re.compile('TrigCost').match(f)]

                for f in ratelis:
                        try:
                                name="%s.%s.NTUP.%s._lb%04d._%s.root" %\
                                (self.base_name,f.split('.')[0],self.pversion,\
                                 self.lumi_block,self.getJobId()) 

                                shutil.move(f,name)
                                HLTJob_log.info('created %s for rate analysis'%name)
                        except Exception, x:
                                self.crash("HLTProcess, renameCostMonFiles: error moving from %s: %s" % (f, x))
                
                for f in costlis:
                        try:
                                name="%s.%s.NTUP.%s._lb%04d._%s.root" %\
                                (self.base_name,f.split('.')[0],self.pversion,\
                                 self.lumi_block,self.getJobId())

                                shutil.move(f,name)
                                HLTJob_log.info('created %s for cost monitor'%name)
                        except Exception, x:
                                self.crash("HLTProcess, renameCostMonFiles: error moving from %s: %s" % (f, x))

        def renameRootFiles(self):
                histlis = ['debug-monitoring.root',  \
                           'expert-monitoring.root', \
                           'lbn-monitoring.root',    \
                           'runstat-monitoring.root',\
                           'shift-monitoring.root' ]
                                
                name = 'hlt-monitoring_%s' % self.name.split('_')[0]
                ttag = 'HIST'
                if self.pversion != '':
                        ttag = '%s.%s'%(ttag,self.pversion)
                                        
                outfile="%s.Reproc.%s.%s._lb%04d._%s.root" % \
                        (self.base_name,ttag,name,self.lumi_block,self.getJobId())

                fields = outfile.split('.')
                if fields[2].find('debug_')!=-1: 
                        fields[2]='debug_all'
                        outfile = '.'.join(fields)
                
                cmd = ['hadd',outfile]
                cmd.extend(histlis)
                
                rem = ['rm','-rf']
                rem.extend(histlis)
                
                
                try:
                        (ret, out, err)        =  runcmd(cmd, env=self.myenv )
                        if ret!=0:
                                HLTJob_log.error('problem for merging the histogram')
                        else:
                                HLTJob_log.info('histogram files merged into %s'%outfile)
                
                        (ret, out, err)        =  runcmd(rem, env=self.myenv )
                        if ret!=0:
                                HLTJob_log.error('could not remove the old histograms')
                        else:
                                HLTJob_log.info('old histograms removed')

                except Exception, x:
                        print 'Exception thrown in renameRootFiles (%s): %s' % (cmd, x)

        def renameOutputFiles(self,prefix):
            '''Rename all output files matching the given prefix'''
            flst = [f for f in os.listdir('.') if re.compile(prefix).match(f)]
            if flst==[]:
                self.crash("HLTProcess: could not find any files starting '%s' to rename (directory contains: %s)" % (prefix, os.listdir('.')))
            
            src_tag = '%s-1' % self.getJobId()
            dst_tag = self.getJobId()
            pver = self.pversion
            try:
                for f in flst:
                    dest = f.replace(src_tag,dst_tag)
                    if pver != '':
                        dest = dest.replace('RAW','RAW.%s'%pver)
                    if f != dest:
                        shutil.move(f,dest)
                        HLTJob_log.info('Renamed %s to %s' % (f, dest))
                    else:
                        HLTJob_log.info('No rename for %s: did not contain string %s' % (f, src_tag))                        
                return '.'.join(dest.split('.')[:-2])
            except Exception, x:
                self.crash("HLTProcess, renameOutputFiles: error moving from %s: %s" % (f, x))
        
        def createPostCommand(self):#doMagFromDCS=False, doIgnorePrescales=False):
                post_command_name = '%s_postCommands.py' % self.name
                postCommands = file(post_command_name,"w")
                
                
                conUp_text = ['from GaudiPython.Bindings import iProperty\n','\n',]
                
                for c in self.config['conditions']:
                        cond = c.split(',')
                        string = "\"\'<prefix>%s</prefix> <tag>%s</tag>\'\"" % (cond[0],cond[1])
                        conUp_text += ['iProperty(\'IOVDbSvc\').overrideTags.push_back(%s)\n'%string]
                
                dgbMU_text = ['from GaudiPython.Bindings import iProperty\n',
                              '\n',
                              'iProperty(\'ByteStreamAddressProviderSvc\').OutputLevel=DEBUG\n',
                              '\n',
                              'iProperty(\'ToolSvc.MuCTPIByteStreamTool\').OutputLevel=VERBOSE\n',
                              'if len(iProperty(\'TrigSteer_L2.Lvl1Converter\').properties())>0:\n',
                              '    iProperty(\'muFast_900GeV\').OutputLevel   = DEBUG\n',
                              '    iProperty(\'muFast_900GeV\').MUlvl1INFO    = True\n',
                              '    iProperty(\'muFast_900GeV\').MUtrackINFO   = True\n',    
                              '    iProperty(\'muFast_900GeV\').MUroadsINFO   = True\n',
                              '    iProperty(\'muFast_900GeV\').MUdecoINFO    = True\n',
                              '    iProperty(\'muFast_900GeV\').MUcontINFO    = True\n',
                              '    iProperty(\'muFast_900GeV\').MUfitINFO     = True\n',
                              '    iProperty(\'muFast_900GeV\').MUsagINFO     = True\n',
                              '    iProperty(\'muFast_900GeV\').MUptINFO      = True\n', 
                              '    iProperty(\'muFast_900GeV\').MUtgcDecoINFO = True\n',
                              '\n',
                              '    iProperty(\'muFast_Muon\').OutputLevel   = DEBUG\n',
                              '    iProperty(\'muFast_Muon\').MUlvl1INFO    = True\n',
                              '    iProperty(\'muFast_Muon\').MUtrackINFO   = True\n',    
                              '    iProperty(\'muFast_Muon\').MUroadsINFO   = True\n',
                              '    iProperty(\'muFast_Muon\').MUdecoINFO    = True\n',
                              '    iProperty(\'muFast_Muon\').MUcontINFO    = True\n',
                              '    iProperty(\'muFast_Muon\').MUfitINFO     = True\n',
                              '    iProperty(\'muFast_Muon\').MUsagINFO     = True\n',
                              '    iProperty(\'muFast_Muon\').MUptINFO      = True\n', 
                              '    iProperty(\'muFast_Muon\').MUtgcDecoINFO = True\n',
                              '\n',
                              '    iProperty(\'Lvl2EventLoopMgr\').OutputLevel = DEBUG\n',
                              '\n',
                              #'    iProperty(\'ByteStreamAddressProviderSvc\').MuCTPIModuleID=0x0\n',
                              '    iProperty(\'ServiceMgr.RoIBResultByteStreamTool\').OutputLevel=VERBOSE\n',
                              '\n',
                              '    iProperty(\'ServiceMgr.PtLUTSvc\').OutputLevel=DEBUG\n'
                             ]
                
                magDB_text = ['from GaudiPython.Bindings import iProperty\n',
                              '\n',
                              '# Add the DCS folder (note the double-use of quotes)\n',
                              'iProperty(\'IOVDbSvc\').Folders.push_back(\"\'<db>COOLOFL_DCS/COMP200</db> /EXT/DCS/MAGNETS/SENSORDATA\'\")\n',
                              'iProperty(\'MagFieldAthenaSvc\').UseDCS = True\n',
                              'iProperty(\'MagFieldAthenaSvc\').NameOfTheSource = \'COOL\'\n\n',
                             ]

                ignPR_text = ['from GaudiPython.Bindings import iProperty\n',
                              '\n',
                              '# This will work for L2\n',
                              'if len(iProperty(\'TrigSteer_L2.Lvl1Converter\').properties())>0:\n',
                              '    iProperty(\'TrigSteer_L2.Lvl1Converter\').ignorePrescales = True\n',
                              '\n',  
                              '# and this for EF\n',
                              'if len(iProperty(\'TrigSteer_EF.Lvl2Converter\').properties())>0:\n',
                              '    iProperty(\'TrigSteer_EF.Lvl2Converter\').ignorePrescales = True\n\n',   
                             ]

                noPs_check = ['from GaudiPython.Bindings import iProperty\n',
                              '\n',
                              '# This will work for L2\n',
                              '#if len(iProperty(\'TrigSteer_L2.Lvl1Converter\').properties())>0:\n',
                              '#    iProperty(\'TrigSteer_L2.Lvl1Converter\').checkConfig = False\n',
                              '\n',
                              '# and this for EF\n',
                              'if len(iProperty(\'TrigSteer_EF.Lvl2Converter\').properties())>0:\n',
                              '    iProperty(\'TrigSteer_EF.Lvl2Converter\').checkConfig = False\n\n',
                             ]

                hardT_text = ['from GaudiPython.Bindings import iProperty\n',
                              '\n',
                              '# This will work for L2\n',
                              'if len(iProperty(\'TrigSteer_L2\').properties())>0:\n',
                              '    iProperty(\'TrigSteer_L2\').hardEventTimeout = 1800000000000\n',
                              '\n',  
                              '# and this for EF\n',
                              'if len(iProperty(\'TrigSteer_EF\').properties())>0:\n',
                              #'    iProperty(\'TrigSteer_EF\').hardEventTimeout = 1800000000000\n\n',   
                              '    iProperty(\'TrigSteer_EF\').hardEventTimeout = 1800000000000\n\n',
                             ]
                
                costM_text = ['from GaudiPython.Bindings import iProperty\n',
                              '\n',
                              'if \'doOperationalInfo\' in iProperty(\'TrigSteer_L2\').properties():\n',
                              '    iProperty(\'TrigSteer_L2\').doOperationalInfo=1\n',
                              '\n',
                              'if \'writeAlways\' in iProperty(\'TrigSteer_L2.TrigCostExecL2\').properties():\n',
                              '    iProperty(\'TrigSteer_L2.TrigCostExecL2\').writeAlways=True',
                              '\n',  
                              'if \'doOperationalInfo\' in iProperty(\'TrigSteer_EF\').properties():\n',
                              '    iProperty(\'TrigSteer_EF\').doOperationalInfo=1\n',
                              '\n',
                              'if \'writeAlways\' in iProperty(\'TrigSteer_EF.TrigCostExecEF\').properties():\n',
                              '    iProperty(\'TrigSteer_EF.TrigCostExecEF\').writeAlways=True\n',
                             ]
                
                file_text = []

                #if doMagFromDCS:      file_text.extend(magDB_text)
                #if doIgnorePrescales: file_text.extend(ignPR_text)
                
                conf = self.config['post_commands']
                
                if len(self.config['conditions']):    file_text.extend(conUp_text)
                if conf.find('ignorePrescales') !=-1: file_text.extend(ignPR_text)
                if conf.find('ignoreOnlineKeys')!=-1: file_text.extend(noPs_check)
                if conf.find('confMagFromDCS')  !=-1: file_text.extend(magDB_text)
                if conf.find('setHardTimeout')  !=-1: file_text.extend(hardT_text)
                if conf.find('debugMuon')       !=-1: file_text.extend(dgbMU_text)
                if conf.find('costMonitor')     !=-1: file_text.extend(costM_text)
                
                print >> postCommands, "".join(file_text)
                postCommands.close()
                return post_command_name

        
class ProcessSession(Step):
        """This is the guy that runs AthenaMT/PT. It takes care of streaming and failover (can be understood like a crapy EFD) """        
        def __init__(self,seed,name='PROCESS_SESSION'):
                """A ProcessSession is created with a seed, that is a HltProcess"""
                Step.__init__(self,name)
                self.seed    = seed
                self.summary={                        \
                        'name':self.name,        \
                        'time': 0,                \
                        'processingtime': 0,        \
                        'eventsin':0,                \
                        'eventsout':0,                \
                        'rootsize':0,                \
                        'fileoutsize':0,        \
                        'fileinsize':0,                \
                        'inputfiles':[],        \
                        'eventslist':[],        \
                        'eventsbad':[],                \
                        'filesbad':[],                \
                        'bads':{}
                        }
        
        def toDict(self):        
                return self.summary

        def run(self,input,prefix,maxevents):
                # less performant but more consistent option (lets hope there are not many crashes per file)
                return self.run2(input,prefix,maxevents)
                # more performant option but can lead to strange situations like 
                # self.run1(inputfile,outpuname,naxevents)

        def run1(self,input,prefix,maxevents):
                """Does the real work: processes, splits, proceses, etc. Finally it does a merge and a reprocess of the files containing good portions (in case there was at least one error). Note that this is necessary because currently "*.writting" files can not be recovered within eformat python bindings. When this reprocess fails the thing crashes because it is an inconsistency (probably due to a temporary issue). This is very far from performant."""        
                pass
#        steps                =[]
#        filesGood        =[]
#        filesBad        =[]
#        curProc                =seed
#        curFileSet        =input
#        steps.append(self.seed)
#
#        # processed as many times as necessary
#        finished=False
#        while not finished:
#                HLTJob_log.info("pass #%d of %s" %(i,seed))
#                (ret,total)=curProc.run(curFileSet,tempFile,maxevents)
#                if   ret==1:
#                        # AthenaMT/PT configuration error, not likely to be recovered
#                        self.crash("")
#                elif ret==2:
#                        # AthenaMT/PT processing error (lets start the spliting)
#                        # split it
#                        (nameGood,nameBad,nameRemain)=self.splitIn3(self.curFileSet,eventNumberFailed,filePrefix)
#                        # check whether there is an initial part processed
#                        if nameGood: filesGood.append(nameGood)
#                        # there is always a bad guy        
#                        filesBad.append(nameBad)
#                        # if there is remaining file, set it to be next guy to process
#                        if nameRemain: 
#                                curFileSet=nameRemain
#                                curProc=curProc.getNext()
#                        # otherwise we are done        
#                        else: finished=True
#                else:
#                        nameGood.append(curFile)
#                        finished=True
#
#        # if there was at least one crash
#        if len(filesBad)!=0:
#                pass
#                # merge the Good files (it is necessary as "*.writting can not be recovered)
#        #        m=Merger(self.getEnv())
#        #        m.mergeIt()
#                # reprocess the merge & take proper statistics
#
#
#        # if everything went ok
#        else:
#                pass

        def run2(self,input,prefix,maxevents):
                """Does the real work: processes until it finds an error. When an error is found, it creates 2 files one with the problematic event, and one with all the rest. All the rest is reprocessed until an error is found, or until the end of the file. The procedure is repeated until no errors are found. Note that this is necessary because currently "*.writting" files can not be recovered within eformat python bindings. This is ver very very very far from performant."""        
                # I should pass a prefix and get the list of files that were produced.
                # The files will be two categories-> crashes, mytimeouts, and all the rest
                
                outputgood        ="%s.good" % prefix
                outputbad        ="%s.bad"  % prefix
                fileSetsBad        =[]                        # each file set will contain a single file with a single event with 
                logFilesBad        =[]                        # each file set will contain a single file with a single event with 
                eventsBad        =[]
                bads                ={}
                steps                =[]
                curProc                =self.seed
                curFileSet        =input
                steps.append(self.seed)
        
                self.summary['inputfiles'] = input.files
        
                # processed as many times as necessary
                HLTJob_log.info("starting processing session...")
                self.start()
                finished=False
                filePrefix="split_file"
                i=1
                while not finished:
                        # run it
                        HLTJob_log.info("pass #%d of %s" %(i,self.seed.name))
                        
                        tag = prefix.split('.')[len(prefix.split('.'))-1]
                        if  (tag.find('AthenaMT')!=-1): crash_tag = 'MTCrash'
                        elif(tag.find('AthenaPT')!=-1): crash_tag = 'PTCrash'
                        else: crash_tag = 'Crash'
                        
                        prf = ".".join(prefix.split('.')[:-1])
                        
                        tempFilePrefix ='%s_%d.RAW._lb%04d._%s'     % \
                                            (prefix,i,curFileSet.fileLumiBlock(),self.getJobId())
                        crashLogFile   = '%s.%s.LOGS._crash_%d._%s.log' % \
                                            (prf , crash_tag , i , self.getJobId())
                        crashRawPrefix = '%s.%s.RAW._crash_%d._%s' %  \
                                            (prf , crash_tag , i , self.getJobId() )
                        
                        pver = self.seed.pversion
                        if pver!='':
                                crashLogFile   = crashLogFile.replace('LOGS','LOGS.%s'%pver)
                        
                        HLTJob_log.info('current file set name is '        + " ".join(curFileSet.files))
                        HLTJob_log.info('temp file set prefix is '        + tempFilePrefix)
                        HLTJob_log.info('crash log file name is '        + crashLogFile)
                        HLTJob_log.info('bad events put into prefix '        + crashRawPrefix)
                        
                        (ret,total,processed)=curProc.run(curFileSet,tempFilePrefix,maxevents)
                        self.summary['eventslist']=total
                        # not even RUN state reached
                        if   ret==1:
                                HLTJob_log.warning("we got a crash previous to RUNNING state")
                                logfile=LogFile(self.seed.logfile)
                                logfile.rename2(crashLogFile)
                                logFilesBad.append(crashLogFile)
                                self.crash("error configuring HltProcess",logfile)
                        # was running but there was an error.
                        elif ret==2:
                                # so split it
                                HLTJob_log.info("we got a crash during RUNNING state")
                                HLTJob_log.info("cleaning the crashed datafile???????")
                                eventNumberFailed=len(total)
                                eventsBad.append(total[eventNumberFailed-1])
                                HLTJob_log.warning("event number %d failed (relative to the file)" % eventNumberFailed)
                                spliter=SplitIn2(curProc.getEnv(),curProc.pversion)
                                (newFileSet,newBadSet)=spliter.splitIt(curFileSet,eventNumberFailed,crashRawPrefix)
                                # add the crash log file
                                logfile=LogFile(self.seed.logfile)
                                logfile.rename2(crashLogFile)
                                logFilesBad.append(crashLogFile)
                                newBadSet.setLogFile(self.seed.name,logfile)
                                # now  build the summary to include it in the EventHistory
                                newBadSet.buildSummary(full=True)        
                                (eventSummary,eventList)=\
                                                newBadSet.getEventInfo()
                                Step.history.addProcessingCrash(self.seed.name,total,eventSummary)
                                # there is always a bad guy
                                fileSetsBad.append(newBadSet)
                                # if there is a remaining file, set it to be next guy to process (could be that there are no remaining events after the problematic)
                                if newFileSet:
                                        curFileSet=newFileSet
                                        curProc=curProc.getNext()
                                        i+=1
                                # otherwise we are done
                                else: 
                                        # set the outputfile of the loop to None and stop the loop
                                        newFileSet=None
                                        finished=True
                        # done
                        elif ret==0:
                                HLTJob_log.info("no errors, going ahead")
                                # it could be that no event went out from the stage
                                if processed:
                                        processed.buildSummary(full=True)
                                        (eventSummary,eventList)=\
                                                processed.getEventInfo()
                                        # note that we will not have log files for files that got no events out!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                        logfile=LogFile(self.seed.logfile)
                                        processed.setLogFile(self.seed.name,logfile)
                                else:
                                        eventSummary={}
                                Step.history.addProcessingGood(self.seed.name,total,eventSummary)
                                # add the logfile 
                                # stop the thing
                                finished=True
                        # unknown
                        else:
                                self.crash("unknown return code: %d" % ret)
        
                self.seed.renameRootFiles()
                self.seed.renameCostMonFiles()
        
                # check states after ending
                # whether there are bad files or not
                i=0
                for (pos,gid,time) in eventsBad:
                        self.summary['bads'][gid]={\
                                                        'log':logFilesBad[i],\
                                                        'file':fileSetsBad[i].files[0],\
                                                  }
                        i += 1
                
                self.summary['eventsbad']=eventsBad
                for fs in fileSetsBad:
                        for f  in fs.files:
                                self.summary['filesbad'].append(f)
                if len(fileSetsBad)!=0:
                        HLTJob_log.warning("there were crashes in the last run")
                # add it to the eventslist (we will only have the ones that passed.)
                HLTJob_log.info('adding events to the event list')
                self.summary['eventsin']=self.seed.summary['eventsin']
                self.summary['fileinsize']=self.seed.summary['fileinsize']
                self.summary['fileoutsize']=curProc.summary['fileoutsize']
                self.summary['rootsize']=curProc.summary['rootsize']
                self.summary['eventsout']=curProc.summary['eventsout']
                self.summary['crashlogs']=logFilesBad
                # 1) there is no output file (after spinning & splitting we ended up with no processed file)
                if not processed:
                        HLTJob_log.warning("no output file")
                # 2) there is output file: it can only come from a good reprocessing. 
                HLTJob_log.info("processing session finished")
                self.stop()
                self.summary['time'] = self.total
                self.wrapUp()
                return (processed,fileSetsBad,[])

        def run3(self,inputfile,outputname,maxevents):
        # athenaMT modified with python bindings
                pass

        def splitIn3(filename):
                pass

        def merge():
                pass

class EfProcess(HltProcess):
        def __init__(self,env,config,name='EF_PROCESS'):
                if config['type']== 'DB':
                        config['pairs']['Instance'] = 'EF'
                
                athenaPTlog = 'athenaPT._'+self.getJobId()+'.log'
                ttag = 'LOGS'
                if config.has_key('proc_version'):
                        pver = config['proc_version']
                        if pver!='': ttag = ttag+'.'+pver
                
                try:
                        logname = config['basename']+'.Reproc.%s.%s'%(ttag,athenaPTlog)
                except Exception, x:
                        print 'Exception in EfProcess: %s' % x
                        logname = "Reproc.LOGS."+athenaPTlog
                config['application']            = athenaPT
                config['logfile']            = logname
                config['preload']            = "PreloadEF"
                config['pickle']            = "pereventEF.pickle"
                HltProcess.__init__(self,env,config,name)

class L2Process(HltProcess):
        def __init__(self,env,config,name='L2_PROCESS'):
                if config['type']== 'DB':
                        config['pairs']['Instance'] = 'L2'
                
                athenaMTlog = 'athenaMT._'+self.getJobId()+'.log'
                ttag = 'LOGS'
                if config.has_key('proc_version'):
                        pver = config['proc_version']
                        if pver!='': ttag = ttag+'.'+pver
                try:
                        logname = config['basename']+'.Reproc.%s.%s'%(ttag,athenaMTlog)
                except Exception, x:
                        print 'Exception in L2Process: %s' % x
                        logname = "Reproc.LOGS."+athenaMTlog
                config['application']            = athenaMT
                config['logfile']            = logname
                config['preload']            = "PreloadL2"
                config['pickle']            = "pereventL2.pickle"
                HltProcess.__init__(self,env,config,name)

# someutilities
def fileSummary(filename,myenv):
        summary=None        
        #HLTJob_log.info ("opening ")
        #print filename
        try:
                        ist= istream(filename)
                        count=0
                        #HLTJob_log.info("opened!")
                        for eve in ist:
                                count = count + 1 
                        summary={'eventnumber':count}
                        if count==0:
                                HLTJob_log.info("no events in the file")
                                summary = None
                        del(ist)
        except NameError, x:
                        HLTJob_log.info("cannot open file: %s" % x)
                        #raise
                        summary = None
        except IOError, x:
                        HLTJob_log.info("cannot open file: %s" % x)
                        summary= None
        return summary
                
        
def tail(filename,lines):
        cmd="tail -%d %s" % (lines, filename)
        (ret, out, err)        =  runcmd(cmd.split(' '))
        if ret!=0: 
                        out='error in tail'
        return out

def fileSize(filename):
        HLTJob_log.info("checking file size of intermediate file %s"%filename)
        (ret, out, err)        =  runcmd(["du",filename])
        if ret != 0: 
                return None
        temp=out.split('\t')
        size=temp[0]
        return size


def usage():
        print "usage: write the doc yourself"

def cleanup():
        pass
        #HLTJob_log.info("refusing to kill abruptly ipc_server and is_server")
        #cmd        = "ps -u %s -f" % os.environ['USER']
        #(ret, out, err)        =  runcmd(cmd.split(' '))
        #HLTJob_log.info(out)
        #HLTJob_log.info(err)
        #HLTJob_log.info("pid  is %d" % os.getpid() )
        #HLTJob_log.info("ppid is %d" % os.getppid() )
        #HLTJob_log.info("now going ahead with an abrupt kill to ipc_server and is_server")
        #killall("ipc_server")
        #killall("is_server")
        
