#!/usr/bin/env python
# Author: Hegoi Garitaonandia <hegoi.garitaonandia@cern.ch>
# Master TRF function for the trigger reprocessing jobs.

# --2009-10-19  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - Processing release got from the Cool database and set on top 
#     of the child process that performs the trigger processing tasks.
#   - Interface and parameters reworked in order to comply with the 
#     automatic job submission system.
# --2009-12-03  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - versioning via the amiTag implemented;
# --2010-02-05  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - handling of the stream tag filters added
# --2010-03-05  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - handling of streamed data improved: missing dataset do not cause job abort
#   - setup for the debug stream reprocessing modified:
#        partial build events removed;
#        debug streamed events put into the crashed dataset
# --2011-03-03  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - fix for running from job options
# 2011-07-11 Graeme Stewart <graeme.andrew.stewart@cern.ch>
#  * Improved exception handling to print errors


import logging
import time
import os
import re
import glob
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
import pprint

from TrigTransformOld import TrigConf
from TrigTransformOld import RunWithEnv

from datetime import datetime


# This is an example of gpickle-d dictionary 
#
# Note: for the moment only input a single RAW file (a couple of minor modifications need to be applied to FileCollection and HLTJobLib in order to handle more files)  
#
# {'inputRAWFiles': ['data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW#data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW._lb0000._SFO-1._0001.data',
#                   'data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW#data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW._lb0000._SFO-1._0002.data',
#                   'data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW#data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW._lb0000._SFO-1._0003.data',
#                   'data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW#data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW._lb0000._SFO-3._0001.data',
#                   'data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW#data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW._lb0000._SFO-3._0002.data',
#                   'data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW#data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW._lb0000._SFO-3._0003.data'],
#                   'data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW#data09_muoncomm.00125189.debug_L2ForcedAccept.daq.RAW._lb0000._SFO-3._0003.data'],
#  'JobID':1, 
#
#  'applicationConfig: {'doCTP':True, 'doEF':True, 'doL2': False, ...}'
#
#   These fields will be filled be filled by my application (NOT BY THE TRF WRAPPER):
#
#  'outputRAWFiles': [
#  'data09_cos.00121330.debug_hlterror.dbgrec.RAW#data09_cos.00121330.debug_hlterror.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0001.data',
#  'data09_cos.00121330.debug_hlterror.dbgrec.RAW#data09_cos.00121330.debug_hlterror.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0002.data',
#  'data09_cos.00121330.physics_CosmicDownwardMuons.dbgrec.RAW#data09_cos.00121330.physics_CosmicDownwardMuons.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0001.data',
#  'data09_cos.00121330.physics_IDCosmic.dbgrec.RAW#data09_cos.00121330.physics_IDCosmic.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0001.data',
#  'data09_cos.00121330.physics_IDCosmic.dbgrec.RAW#data09_cos.00121330.physics_IDCosmic.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0002.data',
#  'data09_cos.00121330.physics_IDCosmic.dbgrec.RAW#data09_cos.00121330.physics_IDCosmic.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0003.data',
#  'data09_cos.00121330.physics_IDCosmic.dbgrec.RAW#data09_cos.00121330.physics_IDCosmic.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0004.data',
#  'data09_cos.00121330.physics_IDCosmic.dbgrec.RAW#data09_cos.00121330.physics_IDCosmic.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0005.data',
#  'data09_cos.00121330.physics_IDCosmic.dbgrec.RAW#data09_cos.00121330.physics_IDCosmic.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0006.data',
#  'data09_cos.00121330.physics_L1Calo.dbgrec.RAW#data09_cos.00121330.physics_L1Calo.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0001.data',
#  'data09_cos.00121330.physics_L1Calo.dbgrec.RAW#data09_cos.00121330.physics_L1Calo.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0002.data',
#  'data09_cos.00121330.physics_L1CaloEM.dbgrec.RAW#data09_cos.00121330.physics_L1CaloEM.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0001.data',
#  'data09_cos.00121330.physics_RPCwBeam.dbgrec.RAW#data09_cos.00121330.physics_RPCwBeam.dbgrec.RAW.g1.-L2ForcedAccept-JobID-1._0001.data',
#          ],
#  'outputSummaryRootFiles': ['summary_debug.00121330#expert-EF-JobId-1.root','summary_debug.00121330#expert-L2-JobId-1.root','summary_debug.00121330#result-JobId-1.pickle', '...']}

#logging.basicConfig(level=logging.DEBUG, format='%(asctime)s %(levelname)s %(message)s')

raw_storage_dir = '/castor/cern.ch/grid/atlas/DAQ/reprocessing'
his_storage_dir = '/castor/cern.ch/atlas/atlascerngroupdisk/tdaq-mon/coca/reprocessing'

NoOutputDataset = 'No output dataset'
#class NoOutputDataset(Exception):
#   """Exception raised when there is no output dataset."""
#   pass

def get_recovered_files(out_lis=[],outdsname=[],proc_session=''):
        global raw_storage_dir
        global his_storage_dir
        pf = [f for f in os.listdir('.') \
            if re.compile('PICKLE').search(f) and re.compile('Result').search(f)]

        #try to open file
        try:
            f = open(pf[0], 'r')
            Res = pickle.load(f)
            f.close()
        except Exception, x:
            print "Unexpected exception in get_recovered_files while loading pickle file: %s" % x
            return
                
        #try to loop on STREAMED file
        try:
                #retrieve streamed file
                out_stat = [i for i in Res['steps'] if i.get('name') == 'STREAMER'][0]

                out_dic = out_stat['outputfiles']

                for k in out_dic.keys():
                        print "... checking output file %s" % k

                        run_number = k.split('.')[1]
                        stream     = k.split('.')[2]

                        if k.split('.')[2].split('_')[0] == 'debug':
                                fields  = k.split('.')[:5]
                                fields[2] = 'debug_all'
                                stream  = 'debug_all'
                                dataset = [d for d in outdsname
                                 if re.compile(".".join(fields)).search(d)] 
                        else:
                                dataset = [d for d in outdsname 
                                  if re.compile(".".join(k.split('.')[2:5])).search(d)]
                        
                        #for streamed file is not possible to pre-generate the ds
                        if len(dataset)==0: 
                                dataset = [".".join(k.split('.')[:5])]
                                raise NoOutputDataset,k
                        
                        if proc_session!='': 
                                stream = '%s.%s'%(stream,proc_session)
                        year = datetime.now().strftime("%Y")
                        dire = '%s/%s/%s/%s' % (raw_storage_dir,year,run_number,stream)
                        storage    = { 'type': 'CASTOR', 'basepath': dire, 'nodsdir' : True, 'autodir' : False }
                        out_lis.append( { 'lfn'     : k,
                                          'dataset' : dataset[0],
                                          'events'  : out_dic[k],
                                          'storage' : storage,
                                      } )

        except NoOutputDataset,fil:
            print "No output dataset found for the file %s" % fil
            print "STREAMER output dataset list:"
            pprint.pprint(outdsname)
            print "%s will not be saved on castor!" % fil
            pass
                
        except Exception, x:
            print "Unexpected exception in get_recovered_files in streamed files loop: %s" % x


        #try to loop on DUMMY STREAMED file
        try:
                out_stat = [i for i in Res['steps'] if i.get('name') == 'DUMMY_STREAMER'][0]

                out_dic = out_stat['outputfiles']

                print out_stat
                print out_dic
                print outdsname
                for k in out_dic.keys():
                        dataset = [d for d in outdsname 
                                  if re.compile(".".join(k.split('.')[3:5])).search(d)]
                        
                        if len(dataset)==0: raise NoOutputDataset,k
                        
                        run_number  = k.split('.')[1]
                        file_stream = k.split('.')[2]
                        
                        if file_stream.find('debug')!=-1:
                                stream = 'unstreamed_unknown'
                        else:
                                stream = file_stream
                        if proc_session!='': 
                                stream = '%s.%s'%(stream,proc_session)
                        
                        year = datetime.now().strftime("%Y")
                        dire = '%s/%s/%s/%s' % (raw_storage_dir,year,run_number,stream)
                        storage    = { 'type': 'CASTOR', 'basepath': dire, 'nodsdir' : True, 'autodir' : False}
                        out_lis.append( { 'lfn'     : k,
                                          'dataset' : dataset[0],
                                          'events'  : out_dic[k],
                                          'storage' : storage,
                                      } )

        except NoOutputDataset,fil:
                print "No output dataset found for the file %s" % fil
                print "DUMMY_STREAMER output dataset list:"
                pprint.pprint(outdsname)
                print "Batch execution terminates here!"
                outmap={'prodsys' : { 'trfCode' : 9, 'trfAcronym' : 'TRF_MISSING_FILES', 'jobOutputs': '', 'jobInputs' : '', 'more' : {}}}
                finalreports(outmap,outmap)
                sys.exit(1)
        
        except Exception, x:
            print "Unexpected exception in get_recovered_files in dummy streamed files loop: %s" % x


        #try to loop on RECOVERY_DATASETS files
        try:
                out_stat = [i for i in Res['steps'] if i.get('name') == 'RECOVERY_DATASETS_STREAMER'][0]

                out_dic = out_stat['outputfiles']

                for k in out_dic.keys():
                        dataset = [d for d in outdsname 
                                  if re.compile(".".join(k.split('.')[2:5])).search(d)]
                        
                        if len(dataset)==0: raise NoOutputDataset,k

                        out_lis.append( { 'lfn'     : k,
                                          'dataset' : dataset[0],
                                          'events'  : out_dic[k]
                                      } )

        except NoOutputDataset,fil:
                print "No output dataset found for the file %s" % fil
                print "RECOVERY_DATASETS_STREAMER output dataset list:"
                pprint.pprint(outdsname)
                print "Batch execution terminates here!"
                outmap={'prodsys' : { 'trfCode' : 9, 'trfAcronym' : 'TRF_MISSING_FILES', 'jobOutputs': '', 'jobInputs' : '', 'more' : {}}}
                finalreports(outmap,outmap)
                sys.exit(1)
        
        except Exception, x:
            print "Unexpected exception in get_recovered_files in recovery datasets loop: %s" % x

        return out_lis



def get_files(typ,out_lis=[],outdsname=[]):
        global raw_storage_dir
        global his_storage_dir
        pf = [f for f in os.listdir('.') if re.compile(typ).search(f) ]
        
        try:
                for f in pf:
                        dataset = [d for d in outdsname 
                                  if re.compile(".".join(f.split('.')[3:5])).search(d)]
                        
                        if len(dataset)==0: raise NoOutputDataset,f
                        
                        run_number = f.split('.')[1]
                        year = datetime.now().strftime("%Y")
                        if typ == 'HIST':
                                dire = '%s/%s/%s' % (his_storage_dir,year,run_number)
                        else:
                                dire = '%s/%s/%s' % (raw_storage_dir,year,run_number)

                        #storage    = { 'type': 'CASTOR', 'basepath': dire }
                        storage    = { 'type': 'CASTOR'}
                        #if typ=="HIST": storage['svcclass'] = 't0merge'
                        
                        out_lis.append( { 'lfn'     : f,
                                          'dataset' : dataset[0],
                                          'events'  : 0,
                                          #'storage' : storage,
                                        } )

        except NoOutputDataset,fil:
            print "No output dataset found for the file %s" % fil
            print "output dataset list:"
            pprint.pprint(outdsname)
            print "Batch execution terminates here!"
            outmap={'prodsys' : { 'trfCode' : 9, 'trfAcronym' : 'TRF_MISSING_FILES', 'jobOutputs': '', 'jobInputs' : '', 'more' : {}}}
            finalreports(outmap,outmap)
            sys.exit(1)

        except Exception, x:
            print "Unexpected exception thrown in get_files: %s" % x

        return out_lis

def check_for_crashes():
	"""Load Result pickle file and extract information on the number of crashes that occured during the L2 and EF processing; returns total number of crashes from L2 and EF."""
	
	import pickle,sys,re,os
	try:
		pf = [f for f in os.listdir('.') \
		      if re.compile('PICKLE').search(f) \
		      and re.compile('Result').search(f)][0]
	except IndexError:
		print "Failed to find a pickled Result file in BatchHLTTrf.py::check_for_crashes()"
		outmap={'prodsys' : { 'trfCode' : 9, 'trfAcronym' : 'TRF_MISSING_FILES', 'jobOutputs': '', 'jobInputs' : '', 'more' : {}}}
		finalreports(outmap,outmap)
		sys.exit(1)
		
	try:
		file = open(pf, 'r')
		data=pickle.load(file)
		file.close()
	except Exception, x:
		print "Failed to open and unpickle data from file %s\nwith error %s" % (pf,x)
		return
	
	l2=[ a['eventsbad'] for a in data['steps'] if a['name']=="L2_PROCESS_SESSION" ][0]
	ef=[ a['eventsbad'] for a in data['steps'] if a['name']=="EF_PROCESS_SESSION" ][0]
	if (len(l2)>0):
		print "L2 crashed/failed on %s events" % len(l2)
		print "pos,gid,time of crashes:\n%s" % (l2)
	else:
		print "L2 had no crashes/failures"
	if (len(ef)>0):
		print "EF crashed/failed on %s events" % len(ef)
		print "pos,gid,time of crashes:\n%s" % (ef)
	else:
		print "EF had no crashes/failures"

	return len(l2)+len(ef)


def main():

        if (len(sys.argv) != 2) and (not sys.argv[1].startswith('--argdict=')) :
          print "Input format wrong --- use "
          print "   --argdict=<pickled dictionary containing input info> "
          print "   with key/value pairs: "
          print "     i1) 'inputRAWFiles': python list ['datasetname#filename1','datasetname#filename2',...] "
          print "        (input dataset + file names) "
          print "     i2) 'JobID': int "
          print "        (this is the task id, it will be used to compose unique output names within the dataset) "
          print "     i3) 'applicationConfig': python dictonary { 'doCTP': True, ...}"
          print "        (everything I want to configure my app)"
          print "     o1) 'outputRAWFiles': python list [ 'datasetname#filename1, ... ' ]"
          print "        (merged output dataset name + file) "
          print "   and parameters like: "
          print "     o2) '...' (optional): int"
          print "        (...) "
          sys.exit(-1)
        
        else :
          picklefile         = sys.argv[1][len('--argdict='):]
          myfile         = open(picklefile,'r')        
          trf_iface        = pickle.load(myfile)
        

        # get the start of time
        t0 = time.time()

        # extract parameters from pickle file
        print
        print "*****************************"
        print "******** BatchHLTTrf ********"
        print "*****************************"
        print ""
        print "Using pickled file ", picklefile, " for input parameters"
        f = open(picklefile, 'r')
        map = pickle.load(f)
        f.close()
        
        
        print "\nFull run options:\n"
        pprint.pprint(map)


        # extract input file list from string coded info  
        infilelist = []
        indsname = (map['inputRAWFiles'][0]).split('#')[0]
        for val in map['inputRAWFiles'] :
          infilelist.append(val.split('#')[1]) 
        
        
        outdsname = []
        if trf_iface.has_key('outputRAWFiles'):
           # format of outputRAWFiles should be a list of strings but if it is a single file, it seems to be sent as a bare string instead of a list. Hack to fix this:
           if isinstance(trf_iface['outputRAWFiles'],basestring):
              trf_iface['outputRAWFiles']=[trf_iface['outputRAWFiles']]
              
           outdsname = [ d.split('#')[0] for d in trf_iface['outputRAWFiles'] ]
           print trf_iface
           print "set outdsname to ", outdsname
        else:
          if trf_iface.has_key('efcrashdata'):
            outdsname.append(trf_iface['efcrashdata'].split('#')[0])
          if trf_iface.has_key('efcrashlog'):
            outdsname.append(trf_iface['efcrashlog'].split('#')[0])
          if trf_iface.has_key('l2crashdata'):
            outdsname.append(trf_iface['l2crashdata'].split('#')[0])
          if trf_iface.has_key('l2crashlog'):
            outdsname.append(trf_iface['l2crashlog'].split('#')[0])
          if trf_iface.has_key('outputFile'):
            outdsname.append(trf_iface['outputFile'].split('#')[0])
          if trf_iface.has_key('unstreamedFile'):
            outdsname.append(trf_iface['unstreamedFile'].split('#')[0])
          if trf_iface.has_key('outputLog'):
            outdsname.append(trf_iface['outputLog'].split('#')[0])
          if trf_iface.has_key('outputPickle'):
            outdsname.append(trf_iface['outputPickle'].split('#')[0])
          if trf_iface.has_key('outputRoot'):
            outdsname.append(trf_iface['outputRoot'].split('#')[0])
          if trf_iface.has_key('outputHLTacc'):
            outdsname.append(trf_iface['outputHLTacc'].split('#')[0])
          if trf_iface.has_key('outputHLTrej'):
            outdsname.append(trf_iface['outputHLTrej'].split('#')[0])
          if trf_iface.has_key('outputHLTcra'):
            outdsname.append(trf_iface['outputHLTcra'].split('#')[0])
          if trf_iface.has_key('outputDebug'):
            outdsname.append(trf_iface['outputDebug'].split('#')[0])
          if trf_iface.has_key('outputCostL2'):
            outdsname.append(trf_iface['outputCostL2'].split('#')[0])
          if trf_iface.has_key('outputCostEF'):
            outdsname.append(trf_iface['outputCostEF'].split('#')[0])
          if trf_iface.has_key('outputRateL2'):
            outdsname.append(trf_iface['outputRateL2'].split('#')[0])
          if trf_iface.has_key('outputRateEF'):
            outdsname.append(trf_iface['outputRateEF'].split('#')[0])
          # include the additional dataset from the processing streaming
          for k in trf_iface:
            if k.find('stream_')!=-1: outdsname.append(trf_iface[k].split('#')[0])


        # extract more string coded info, Atlas general practice. 
        # extract run/stream info from input dataset name, assume official dataset nomenclature
        fdsname = '.'.join(os.path.basename(infilelist[0]).split('.')[:5])
        try :
            runType    = fdsname.split('.')[0]
            runNumber  = int(fdsname.split('.')[1])
            streamType = (fdsname.split('.')[2]).split('_')[0] 
            streamName = (fdsname.split('.')[2]).split('_')[1] 
        except Exception, x:
            runType    = 'unknown'
            runNumber  = 0
            streamType = 'unknown'
            streamName = 'unknown'
            print "Exception thrown parsing input dataset name: %s" % x
        
        try:
            proc_session = '.'.join(outdsname[0].split('.')[5:])
        except Exception, x:
            proc_session = ''
            print "Exception thrown parsing proc_session: %s" % x
          
        
        print
        print 'processing session: %s' % proc_session
        print
        
          
        # this is what BatchHLTApps takes as input, start loading
        # configuration parameters from pickle
        app_input_map = {}
        if trf_iface.has_key('applicationConfig'): 
          app_input_map  = trf_iface['applicationConfig']
        
        
        if app_input_map.has_key('proc_session'):
          print '\"proc_session\" key not allowed to be a input parameter'
          print 'exiting ....'
          sys.exit(1)
        else:
          if proc_session!='': app_input_map['proc_session'] = proc_session
        
        
        cmd = ""
        if(app_input_map.has_key('extvar')):
                for var in app_input_map['extvar'].split(';'):
                        cmd += 'export %s; ' % var
                del app_input_map['extvar']
        
        if(app_input_map.has_key('comdir')):
                exe_rep = app_input_map['comdir'] 
                cmd += "export PYTHONPATH="+exe_rep+":$PYTHONPATH; "
                cmd += "export PATH="+exe_rep+":$PATH; "
                cmd += exe_rep+"/"
                del app_input_map['comdir']
        else:
                cmd += "./"

        cmd += "BatchHLTApps.py " + ",".join(infilelist)
        

        
        
        app_input_map['files']= ",".join(infilelist)                # conversion to string a bit weird to do, 
                                                                # but this way both BatchHLT and BatchHLTTrf work  
        
        app_input_map['run_number'] = runNumber

        
        #########################################
        # Configuration of the reprocessing job #
        #########################################
        global raw_storage_dir
        global his_storage_dir
        
        # activate modular processing according to the stream type
        if streamType=='debug':
                raw_storage_dir = os.path.join(raw_storage_dir,'error_recovery')
                his_storage_dir = os.path.join(his_storage_dir,'error_recovery')
                if not app_input_map.has_key('doStreaming'):
                        app_input_map['doStreaming'] = True
                if not app_input_map.has_key('stream_out'):
                        app_input_map['stream_out'] = "debug"
                if not app_input_map.has_key('filters'):
                        app_input_map['filters'] = "physics;debug"
                if not app_input_map.has_key('post_commands'):
                        app_input_map['post_commands'] = \
                        '\'ignoreOnlineKeys;confMagFromDCS;setHardTimeout;costMonitor\''
                if streamName.find('L2')!=-1 or streamName=='EFMissingData':
                        if not app_input_map.has_key('doCTP') and streamName!='L2HltTimeout':
                                app_input_map['doCTP'] = True
                        if not app_input_map.has_key('doL2'):
                                app_input_map['doL2']  = True
                        if not app_input_map.has_key('doEF'):
                                app_input_map['doEF']  = True
                elif streamName.find('efd')!=-1 or streamName.find('EFHlt')!=-1:
                        if not app_input_map.has_key('doEF'):
                                app_input_map['doEF']  = True
                else:
                        if not app_input_map.has_key('doL2'):
                                app_input_map['doL2']  = True
                        if not app_input_map.has_key('doEF'):
                                app_input_map['doEF']  = True

        else:
                raw_storage_dir = os.path.join(raw_storage_dir,'physics')
                his_storage_dir = os.path.join(his_storage_dir,'physics')
                if not app_input_map.has_key('doDummyStreaming'):
                        app_input_map['doDummyStreaming'] = True
                if not app_input_map.has_key('post_commands'):
                        app_input_map['post_commands'] = \
                        '\'confMagFromDCS;setHardTimeout;costMonitor\''
                if not app_input_map.has_key('doL2'):
                        app_input_map['doL2']  = True
                if not app_input_map.has_key('doEF'):
                        app_input_map['doEF']  = True

        #raw_storage_dir = raw_dir
        #his_storage_dir = his_dir
        
        # this is the info BatchHLTApps uses to reconstruct unique names
        file_stream =  "%s_%s" % ( streamType, streamName)
        file_prefix =  "%s.%08d" % ( runType, int(runNumber))

        #set the job_id
        if trf_iface.has_key('_partid'): job_id = trf_iface['_partid']
        else: job_id = '%X' % trf_iface['JobID']
        

        if not app_input_map.has_key('file_stream'):
                app_input_map['file_stream'] = file_stream
        if not app_input_map.has_key('file_prefix'):
                app_input_map['file_prefix'] = file_prefix
        if not app_input_map.has_key('job_id'):
                app_input_map['job_id'] = job_id
                

        # setting the ATLAS release for the reprocessing run
        if(app_input_map.has_key('release')): 
                release = app_input_map['release']
                del app_input_map['release']
        else:
                trigconf = TrigConf.get(runNumber,"AtlasP1HLT,16.1.1.2",True)
                try:
                        release = 'AtlasP1HLT,%s' % trigconf.Release()
                except Exception, x:
                        print "BatchHLT: cannot get the code release from TrigConf: %s" % x
                        print tigconf.str()
                        sys.exit(1)

        local_patch_area=''
        if(app_input_map.has_key('local_patch_area')):
                local_patch_area = app_input_map['local_patch_area']
                del app_input_map['local_patch_area']
                
                
        # copy all the input_param dict into a command line 
        for t in app_input_map:
                if t != 'run_number'    and \
                   t != 'files'         and \
                   t != 'job_id'        and \
                   t != 'conditions'    and \
                   t != 'filters'       and \
                   t != 'stream_out'    and \
                   t != 'smk'           and \
                   t != 'lvl1ps'        and \
                   t != 'hltps'         and \
                   t != 'proc_session'  and \
                   t != 'service_class' and \
                   t != 'options'       and \
                   t != 'doLVL1Remapping':
                        try:
                            cmd = cmd +" --"+t+"="+app_input_map[t]
                        except:
                            # This code block makes me weep - GS
                            if app_input_map[t]: cmd = cmd +" --"+t
                elif t=='run_number' or t=='job_id' or t=='smk' or t=='lvl1ps' or t=='hltps' or t=='proc_session' or t=='service_class' or t=='doLVL1Remapping': 
                        cmd = cmd +" --"+t+"="+str(app_input_map[t])
                elif t=='conditions' or t=='filters' or t=='stream_out':
                        for c in app_input_map[t].split(';'):
                                cmd = cmd +" --"+t+"="+c
                elif t=='options':
                        cmd = cmd +" --"+t+"=\'"+app_input_map[t]+"\'"

        # check input files exist
        for infile in infilelist:
                if not os.path.exists(infile):
                        print "Warning: input file is not found: %s" % infile
			

        # RUN!
        print "\nRunning BatchHLTApps with %s" % release
        print cmd
        print
        try:
            (code, stdout, stderr) = RunWithEnv.run(release, cmd+" -v",local_patch_area)
            #(code,stdout,stderr)=(0,"stdout: test","stderr: test")
        except Exception, x:
            print "Exception thrown during RunWithEnv call: %s" % x
            code = 1

            print (code, stdout, stderr)

        crashes=check_for_crashes()


        # now tell TMS what files I have, and what I want to do with them
        infiles  = []
        outfiles = []
        num1 = 0
        num2 = int(time.time() - t0)

        if code == 0:
                print stdout
                acronym = 'TRF_OK'
                txt = 'BatchHLTTrf finished OK'
                outfiles = get_recovered_files(outfiles,outdsname,proc_session)
                for f in outfiles: num1 += f['events']
                outfiles = get_files('HIST',outfiles,outdsname)
                outfiles = get_files('NTUP',outfiles,outdsname)
                outfiles = get_files('LOGS',outfiles,outdsname)
                outfiles = get_files('PICKLE',outfiles,outdsname)
                ptCrashTag = 'PTCrash.RAW._crash'
                mtCrashTag = 'MTCrash.RAW._crash'
                if proc_session !='':
                        ptCrashTag = ptCrashTag.replace('RAW','RAW.%s'%proc_session)
                        mtCrashTag = mtCrashTag.replace('RAW','RAW.%s'%proc_session)
                outfiles = get_files(ptCrashTag,outfiles,outdsname)
                outfiles = get_files(mtCrashTag,outfiles,outdsname)
        else:
                print stderr
                acronym = 'TRF_PROBLEM'
                txt = 'problem in running BatchHLTTrf'

        if (crashes>0):
                txt = "crashes in L2/EF processing"

        outmap_tom = { 'prodsys' : { 'trfCode' : code,
				     'trfAcronym' : acronym, 
				     'jobOutputs': outfiles,
				     'jobInputs' : infiles,             
				     'more' : { 'num1' : num1, 'num2' : num2, 'txt1' : txt }
				     }
		       }

        if (crashes>0):
                code=crashes
                acronym = 'TRF_PROBLEM'

        outmap_panda= { 'prodsys' : { 'trfCode' : code,
				      'trfAcronym' : acronym, 
				      'jobOutputs': outfiles,
				      'jobInputs' : infiles,             
				      'more' : { 'num1' : num1, 'num2' : num2, 'txt1' : txt }
                               }
                 }

        finalreports(outmap_tom,outmap_panda)

def finalreports(outmap_tom,outmap_panda):
        import os, pickle
        
        # output for TOM:
	# crashes in L2/EF processing for debug recovery are *not* bad
        f = open('jobReport.gpickle', 'w')
        pickle.dump(outmap_tom, f)
        f.close()

	# output for Prodsys/PANDA:
	# crashes in L2/EF processing for reprocessing are *very* bad
        from xml.dom.minidom import Document
        try:
            xmlDocument = Document()
            root = xmlDocument.createElement('root')
            xmlDocument.appendChild(root)
            dict2xmlNode(xmlDocument, root, outmap_panda)
            xmlFile = open("metadata.xml", "w")
            xmlFile.write(xmlDocument.toxml())
            xmlFile.close()
        except Exception,x:
            print "Exception thrown while writing summary.xml: %s" % x


def dict2xmlNode(doc, node, obj):
    for i in obj.keys():
        child = doc.createElement(i)
        if type(obj[i]) is dict:
            dict2xmlNode(doc, child, obj[i])
        else:
            child.setAttribute('value',str(obj[i]))
        node.appendChild(child)

        
        # THE END!

    
if __name__ == "__main__":
        main()

########################################
#        outdsname = (outputRAWFile']).split('#')[0]
#        outfile   = (map['outputRAWFile']).split('#')[1]
