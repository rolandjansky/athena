#!/usr/bin/env python
# Author: Hegoi Garitaonandia <hegoi.garitaonandia@cern.ch>
# Implement the main sequence task to reprocessing the HLT
#
# --2009-10-19  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - add options for standalone run;
#   - trigger configuration retrieved from Cool;
#   - cleanup and fix;
#   - logging improved.
# --2009-12-03  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - versioning via the amiTag implemented;
# --2010-02-05  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - handling of the output stream tag added
# --2010-03-05  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - filtering option for the data streaming added
# --2010-04-07  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - Added handler for remapping of the LVL1 items according to an 
#     external module 
# --2010-04-29  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - Reuse of exactly the same online prescales implemented
# --2010-09-04  Alessandro Di Mattia <dimattia@mail.cern.ch>
#   - Add protection to handle cases when the output is missing


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
from optparse import OptionParser

logging.basicConfig(level=logging.DEBUG,
      format='%(asctime)s - %(module)14s: %(funcName)11s %(levelname)s %(message)s', \
      datefmt='%d %b %Y %H:%M:%S')
app_log = logging.getLogger("BatchHLTApp")
handlerOut = logging.StreamHandler(sys.stdout)
handlerOut.setFormatter(logging.Formatter('%(module)14s: %(funcName)11s %(levelname)s %(message)s'))

app_log.addHandler(handlerOut)


from TrigTransformOld import TrigConf
from TrigTransformOld.BatchIPC import Result
from TrigTransformOld.Utils import *
from TrigTransformOld.HLTJobLib import *
from TrigTransformOld.Streamer import *

import eformat
from eformat import *

from datetime import datetime


    
def printenv():
    app_log.info('DUMPING THE WORKING ENVIRONMENT')
    env = os.environ
    for t in env:
        string = env[t].split(':')
        if len(string)==1:
            app_log.info('%s: %s' % (t,env[t]) )
        else:
            app_log.info('%s:' % t )
            for p in string:
                app_log.info('   %s' % p)

    app_log.info(' ')
    app_log.info(' ')


def run(input_params):

    # this are some defaults
    input             = None
    lvl1_remap_module = None
    en_lvl1pre_module = None
    file_stream              = None
    file_prefix       = None 
    job_id            = None
    maxevents         = None
    run_number              = None
    joboptions        = None
    options           = None
    smk               = None
    lvl1ps            = None
    hltps             = None
    doCTP             = False
    doMUCTPI          = False
    doCopyIn          = False
    doStreaming       = False
    doDummyStreaming  = False
    doRecording       = False
    doL2                     = False
    doEF              = False
    doPre             = False
    doPost            = False
    doHLTDebugWrite   = False

    L2_SpecialOptions = ''
    
    year            = datetime.now().strftime("%Y")
    post_commands   = ''
    conditions      = []
    filters         = []
    stream_out      = []
    connection            = 'DBServer=TRIGGERDB'
    crash_dir             = '/castor/cern.ch/grid/atlas/DAQ/reprocessing/%s' % year
    output_dir      = '/castor/cern.ch/grid/atlas/DAQ/reprocessing/%s' % year
    histo_dir       = '/castor/cern.ch/atlas/atlascerngroupdisk/tdaq-mon/coca/reprocessing/%s' % year
    service_class   = 'atlcal'
    proc_session    = ''

    #override default for trigmon manual submission
    #crash_dir       = '/castor/cern.ch/user/t/trigmon/reprocessing_2009'
    #output_dir      = '/castor/cern.ch/user/t/trigmon/reprocessing_2009'
    #histo_dir       = '/castor/cern.ch/user/t/trigmon/reprocessing_2009'
    
    action_dict = {}
    

    # now read the params from the input dictionary
    for o, a in input_params.items():
        if o in ("job_id"):
            job_id = a
        elif o in ("run_number"):
            run_number = a
        elif o in ("file_prefix"):
            file_prefix = a
        elif o in ("file_stream"):
            file_stream = a
        elif o in ("output_dir"):
            output_dir = a
        elif o in ("crash_dir"):
            crash_dir = a
        elif o in ("histo_dir"):
            histo_dir = a
        elif o in ("service_class"):
            service_class = a
        elif o in ("jobOptions"):
            joboptions = a
        elif o in ("options"):
            options = a
        elif o in ("smk"):
            smk = a
        elif o in ("lvl1ps"):
            lvl1ps = a
        elif o in ("hltps"):
            hltps = a
        elif o in ("post_commands"):
            post_commands = a
        elif o in ("proc_session"):
            proc_session = a
        elif o in ("files"):
            files = a.split(',')
        elif o in ("maxevents"):
            maxevents = int(a)
        elif o in ("doCopyIn"):
            doCopyIn = a
            action_dict[0]="doCopyIn "
        elif o in ("doPreAnalysis"):
            doPre = a
            action_dict[1]="doPre "
        elif o in ("doCTP"):
            doCTP = a
            action_dict[2]="doCTP "
        elif o in ("doMUCTPI"):
            doMUCTPI = a
            action_dict[2]="doMUCTPI "
        elif o in ("doLVL1Remapping "):
            lvl1_remap_module = a
            action_dict[3]="doLVL1Remapping(%s) " % lvl1_remap_module
        elif o in ("enLVL1prescales"):
            en_lvl1pre_module = a
        elif o in ("doL2"):
            doL2 = a
            action_dict[4]="doL2 "
        elif o in ("connection"):
            connection = 'DBServer=%s' % a
        elif o in ("conditions"):
            conditions = a
        elif o in ("filters"):
            filters = a
        elif o in ("stream_out"):
            stream_out = a
        elif o in ("doEF"):
            doEF = a
            action_dict[5]="doEF "
        elif o in ("doPostAnalysis"):
            doPost = a
            action_dict[6]="doPost "
        elif o in ("doStreaming"):
            doStreaming = a
            action_dict[7]="doStreaming "
        elif o in ("doDummyStreaming"):
            doDummyStreaming = a
            action_dict[8]="doDummyStreaming "
        elif o in ("doRecording"):
            doRecording = a
            action_dict[9]="doRecording "
        elif o in ("doHLTDebugWrite"): 
            doHLTDebugWrite = a
            action_dict[10]="doHLTDebugWrite "
        elif o in ("doNotTruncateL2R"):
            L2_SpecialOptions='--maximum-l2r-size 1200000' # size is in words
        else:
            assert False, "unhandled option"

    str_conf = "[ "
    for a in action_dict: str_conf += action_dict[a]
    str_conf += "]" 

    if output_dir[-1] == '/': output_dir = output_dir[:-1]
    if crash_dir [-1] == '/': crash_dir  = crash_dir [:-1]

    output_dir = '%s/%08d' % (output_dir, run_number)
    crash_dir  = '%s/%08d' % (crash_dir,  run_number)
    histo_dir  = '%s/%08d' % (histo_dir,  run_number)


    
    app_log.info("BatchHLTApp:")
    app_log.info("  executed to process run=%d, stream=%s, prefix_name=%s"%(run_number,file_stream,file_prefix))
    app_log.info("  configured as %s"% str_conf)
    app_log.info("  post commands set for %s" % post_commands)
    app_log.info("  update of conditions: %s" % '\''+('\' + \'').join(conditions)+'\'')
    app_log.info("  filters on the event stream tag: %s" % (' ').join(filters) )

    for f in files:
        app_log.info("  input file %s"% f)


    app_log.info("  output dir set to %s", output_dir)
    app_log.info("  crash dir set to %s", crash_dir)
    app_log.info("  histo dir set to %s", histo_dir)


    
    base_output_name = '%s.%s'                         % (file_prefix,file_stream)
    outputname       = "%s.recovered.RAW"         % base_output_name



    # temp files 
    AthenaMTtoken = 'AthenaMTout'
    AthenaPTtoken = 'AthenaPTout'  
    prefixCTP     ='%s.ctpreco' % (base_output_name)
    prefixMUCTPI  ='%s.muctpir' % (base_output_name)
    prefixREMAPL1 ='%s.LVL1rem' % ( base_output_name)
    prefixL2      ='%s.%s'      % (base_output_name,AthenaMTtoken)
    prefixEF      ='%s.%s'      % (base_output_name,AthenaPTtoken)
    

    
    Step().setJobId('CAF_%s' % job_id)
    app_log.info("  job id set to "+ Step().getJobId())
    
    history_pickle = '%s.Reproc.PICKLE.History._%s.pickle' % \
                                       (base_output_name,Step().getJobId())
    result_pickle  = '%s.Reproc.PICKLE.Result._%s.pickle'  % \
                                       (base_output_name,Step().getJobId())
    
    if proc_session!='':
      history_pickle = history_pickle.replace('PICKLE','PICKLE.%s'%proc_session)
      result_pickle  = result_pickle.replace('PICKLE','PICKLE.%s'%proc_session)
    
    Step().setHistoryName(history_pickle)
    Step().setResultName(result_pickle)
    
    #store the environment
    myenv=os.environ
    


    ###################
    # Start Processing
    ###################
    

    #############################       
    # get some data if requested
    if doCopyIn:
         for f in files:
            CastorDataIn().get(f)
         files = [os.path.basename(f) for f in files]
    
    
    #######################################
    # build the initial list for the events
    theinput = UnProcessedFileSet(files=files)    
    lumi_block = theinput.fileLumiBlock()

    # check options for trigger configuration (could need lumi_block)
    if joboptions:
        config = { 'type':'JOBOPTIONS',         \
                   'joboptions':joboptions,     \
                   'options'  :options,         \
                   'post_commands':'',          \
                   'conditions':conditions,     \
                   'run_number':str(run_number),\
                 }
        app_log.info("  trigger configuration from JobOptions %s, with options %s"%(joboptions,options))
    elif smk and hltps and lvl1ps and connection:
        config = { 'type':'DB',                  \
                   'pairs':{                     \
                          'DBSMKey'   :smk,      \
                          'DBHLTPSKey':hltps,    \
                          'DBL1PSKey' :lvl1ps,   \
                          },                     \
                   'post_commands':post_commands,\
                   'connection':connection,      \
                   'conditions':conditions,      \
                   'run_number':str(run_number), \
                  }
        app_log.info("  trigger configuration from %s, smk %s, hltps %s, lvl1ps %s"%(connection,smk,hltps,lvl1ps))
    elif run_number and connection:
        trigconf = TrigConf.get(run_number)
        try:
            smkey  = trigconf.SMK()
            hltkey = trigconf.HLT_PS(lumi_block)
            l1key  = trigconf.LVL1_PS(lumi_block)
            app_log.info("  FULL PS configuration %s" % trigconf.FULL_HLT_PS())
            if trigconf.FULL_HLT_PS(): 
                hltkey = trigconf.FULL_HLT_PS()
        except:
            app_log.error("BatchHLTApps: cannot get the BD configuration for run %d" % run_number)
            app_log.error(tigconf.str())
            sys.exit(1)
                
        config = { 'type':'DB',                  \
                   'pairs':{                     \
                          'DBSMKey'   :smkey,    \
                          'DBHLTPSKey':hltkey,   \
                          'DBL1PSKey' :l1key,    \
                          },                     \
                   'post_commands':post_commands,\
                   'connection':connection,      \
                   'conditions':conditions,      \
                   'run_number':str(run_number), \
                  }
        app_log.info("  trigger configuration from %s, smk %s, hltps %s, lvl1ps %s"%(connection,smkey,hltkey,l1key))
    else:
         Step().cancel("bad configuration for L2/EF")

    config['modEvent'] = []
    if en_lvl1pre_module:
      config['modEvent'].append(en_lvl1pre_module)
      app_log.info("  LVL1 prescales enabled with the module %s"%en_lvl1pre_module)

    config['basename']           = base_output_name
    config['proc_version']       = proc_session
    config['l2_special_options'] = L2_SpecialOptions

    app_log.info("  special options for L2 = %s"%L2_SpecialOptions)

    theinput.buildSummary(full=True)
    (initialSummary,initialList) = theinput.getEventInfo()
    Step().history.appendSimpleHistory('UNPROCESSED',initialSummary)
    
    app_log.info("initial event list created")
    
    
    app_log.info("lumi block going to be processed: %04d" % lumi_block)
    config['lumi_block'] = lumi_block
    
    ############################        
    # doPreanalysis
    if doPre:
       analyzer = Analyze(myenv,"PRE_ANALYZE") 
       analyzer.analyze(theinput)       
    
    
    #############################        
    # reconstruct CTP information
    if doCTP:
       ctpreco       = CtpReconstruction(myenv) 
       reconstructed = ctpreco.reconstruct(theinput,prefixCTP)
       l2input       = reconstructed
    else:
       # bypass 
       l2input       = theinput         
    
    #############################        
    # replace MUCTPI information
    if doMUCTPI:
       muctpirep     = MuctpiFix(myenv) 
       reconstructed = muctpirep.reconstruct(l2input,prefixMUCTPI)
       l2input       = reconstructed

    #############################        
    # remap LVL1 according to an external module
    if lvl1_remap_module:
       name       = 'REMAP_LVL1_%s' % lvl1_remap_module 
       remap_lvl1 = RemapLVL1(name,lvl1_remap_module,myenv) 
       remapped   = remap_lvl1.remap(l2input,prefixREMAPL1)
       l2input    = remapped

    try:
        ############################
        # do l2 processing
        if doL2:
            l2seed      = L2Process(myenv,config=config)
            l2process   = ProcessSession(l2seed,name="L2_PROCESS_SESSION")
            (goodl2,crashl2array,timeoutl2array)\
                        = l2process.run(l2input,prefixL2,maxevents)
            
            # check whether we have goodl2
            result= Step().getStats()
            history= Step().getHistory()
            result.serialize(result_pickle)   
            history.serialize(history_pickle) 

            # copy crashes to somewhere
            if doHLTDebugWrite and create_dir(crash_dir):
                copy = CastorDataOut(service_class)

                for thisCrashEvent in crashl2array:
                    rsd = thisCrashEvent.files[0].split('RAW')[0]+'RAW'
                    lsd = thisCrashEvent.files[0].split('RAW')[0]+'LOGS'
                    if proc_session != '':
                        rsd = '%s.%s' % (rsd,proc_session)
                        lsd = '%s.%s' % (lsd,proc_session)

                    copy.putFileColl(thisCrashEvent,crash_dir,rsd)
                    copy.putLogColl(thisCrashEvent,crash_dir,lsd)

            # cancel if necessary
            if not goodl2:
                Step().cancel("no good output file from L2")
            
            
            #filter output file if needed
            if len(filters)!=0 and not doEF:
                fil = FilterOutputFile(myenv,filters)
                efinput = fil.filter(goodl2,AthenaMTtoken)
                del fil
            else: 
                efinput=goodl2
        else: 
            # bypass
            efinput=l2input
        
        streaminput = efinput
        input_token = AthenaMTtoken
        app_log.info("input_token after L2=%s",input_token)
    
        ############################        
        # do l3 processing
        if doEF:
            efseed      = EfProcess(myenv,config=config)
            efprocess   = ProcessSession(efseed,name="EF_PROCESS_SESSION")
            (goodef,crashefarray,timeoutefarray)\
                        = efprocess.run(efinput,prefixEF,maxevents)

            # check whether we have goodef      
            result= Step().getStats()
            history= Step().getHistory()
            result.serialize(result_pickle)   
            history.serialize(history_pickle) 

            # copy crashes to somewhere
            if doHLTDebugWrite:
                copy = CastorDataOut(service_class)

                for thisCrashEvent in crashefarray: 
                    rsd = thisCrashEvent.files[0].split('RAW')[0]+'RAW'
                    lsd = thisCrashEvent.files[0].split('RAW')[0]+'LOGS'
                    if proc_session != '':
                        rsd = '%s.%s' % (rsd,proc_session)
                        lsd = '%s.%s' % (lsd,proc_session)

                    copy.putFileColl(thisCrashEvent,crash_dir,rsd)
                    copy.putLogColl(thisCrashEvent,crash_dir,lsd)

            # cancel if necessary       
            if not goodef:
                Step().cancel("no good output file from EF")     

            #filter output file if needed
            if len(filters)!=0:
                fil = FilterOutputFile(myenv,filters)
                streaminput = fil.filter(goodef,AthenaPTtoken)
                del fil
            else: 
                streaminput=goodef

            #streaminput = goodef
            input_token = AthenaPTtoken
        
    except SystemExit:
        streaminput = None
        input_token = None
        if ('%s'%sys.exc_value) != '0':
            sys.exit(sys.exc_value)

    ############################        
    # do Post Analysis 
    if doPost:
       analyzer = Analyze(myenv,"POST_ANALYZE") 
       analyzer.analyze(streaminput)    
    
    
    
    ############################
    # Here goes some preliminary streaming to remove the hlterror
    ############################
    # physics streaming will be done here.
    streamlist = []
    if doStreaming and streaminput:
        prod_step_for_physics = 'Reproc.RAW'
        prod_step_for_errors  = 'Reproc.RAW'
        if file_stream.find('debug')!=-1: prod_step_for_errors = 'Error.RAW'
        streamer = Streamer( '.', True, 30, file_prefix, \
                             prod_step_for_physics,prod_step_for_errors, \
                             '_'+Step().getJobId(), proc_session,stream_out, \
                             True)
        streamlist += streamer.streamFile(streaminput)
    
    
    # produce the recovery datasets for the debug stream
    if file_stream.find('debug')!=-1:
        builder = BuildRecoveryFiles(file_prefix, 'Reproc.RAW',\
                                 '_'+Step().getJobId(), proc_session)
        streamlist += builder.produceOutputDatasets(l2input,input_token)
    

    if doDummyStreaming and input_token:
       # Simply chnage the name of the output file
       streamer  = DummyStreamer()
       streamout = streamer.streamFile(input_token,'Reproc')
       if file_stream.find('debug')!=-1:
         streamtype = "unstreamed"
         streamname = "unknown"
       else:
         streamtype = ""
         streamname = file_stream
       streamlist.append( (streamout,streamtype,streamname) )
        



    #####################################
    # send LOGS HIST and Recovered data
    
    #####################################
    if doRecording:
       copy = CastorDataOut(service_class)
       
       lgds = 'Reproc.LOGS'
       htds = 'HIST'
       if proc_session!='':
           lgds = '%s.%s' % (lgds,proc_session) 
           htds = '%s.%s' % (htds,proc_session)
       copy.putDataset(lgds,output_dir)
       copy.putDataset(htds,histo_dir)
          
       for fileset,streamtype,streamname in streamlist:
           if streamtype=='':   subdir = streamname
           else:                subdir = "%s_%s" % (streamtype,streamname)
           if proc_session!='': subdir = '%s.%s' % (subdir,proc_session)
           copy.putFileColl(fileset,output_dir,subdir)
    
    
    ##########################################
    # finalize and send the PICKLE summaries

    ##########################################
    result= Step().getStats()
    result.setGood()
    history= Step().getHistory()
    result.serialize(result_pickle)
    history.serialize(history_pickle) 

    
    #PICKLE have to be recorded as a last step
    if doRecording:
       copy = CastorDataOut(service_class)
       pkds = 'PICKLE'
       if proc_session!='': pkds = '%s.%s' %(pkds,proc_session)
       copy.putDataset(pkds,output_dir) 
    

    # clean up after yourself 
    cleanup()


if __name__ == "__main__":
    p = OptionParser(usage="usage: %prog [options] <files to process>", version="1.1")

    p.add_option( "--doCTP",
                  action  = 'store_true',
                  default = "False",
                  dest    = "doctp",
                  help    = "Force the RoIb data to be extracted from the CTP"\
                            " fragment and attached to the event data")

    p.add_option( "--doMUCTPI",
                  action  = 'store_true',
                  default = "False",
                  dest    = "domuctpi",
                  help    = "Fix the Muctpi data")

    p.add_option( "--doCopyIn",
                  action  = 'store_true',
                  default = "False",
                  dest    = "docopyin",
                  help    = "Force the copying of the input file into the run dir")

    p.add_option( "--doStreaming",
                  action  = 'store_true',
                  default = "False",
                  dest    = "dostreaming",
                  help    = "Stream the events after the trigger reprocessing")

    p.add_option( "--doDummyStreaming",
                  action  = 'store_true',
                  default = "False",
                  dest    = "dodummystreaming",
                  help    = "Just rename the events out of AthenaPT as they would come from an unknow stream")

    p.add_option( "--doRecording",
                  action  = 'store_true',
                  default = "False",
                  dest    = "dorecording",
                  help    = "Record the output files on CASTOR")

    p.add_option( "--doL2",
                  action  = 'store_true',
                  default = "False",
                  dest    = "dol2",
                  help    = "Reprocess the LVL2 with AthenaMT")

    p.add_option( "--doNotTruncateL2R",
                  action  = 'store_true',
                  default = "False",
                  dest    = "donottruncate",
                  help    = "raise the limit of the L2 result size to 1200000 words")

    p.add_option( "--doEF",
                  action  = 'store_true',
                  default = "False",
                  dest    = "doef",
                  help    = "Reprocess the EF with AthenaPT")

    p.add_option( "--doPre",
                  action  = 'store_true',
                  default = "False",
                  dest    = "dopre",
                  help    = "Reprocess the LVL2 with AthenaMT")

    p.add_option( "--doPost",
                  action  = 'store_true',
                  default = "False",
                  dest    = "dopost",
                  help    = "Reprocess the EF with AthenaPT")

    p.add_option( "--doHLTDebugWrite",
                  action  = 'store_true',
                  default = "False",
                  dest    = "dohltdebugwrite",
                  help    = "Set the DEBUG output level for the trigger reprocessing")

    p.add_option( "--file_stream",
                  default = "",
                  type    = "string",
                  dest    = "filestream",
                  metavar = "STR",
                  help    = "Set the input file stream name")

    p.add_option( "--output_dir",
                  default = "",
                  type    = "string",
                  dest    = "outputdir",
                  metavar = "STR",
                  help    = "Set the directory name for copying the output files")
    
    p.add_option( "--crash_dir",
                  default = "",
                  type    = "string",
                  dest    = "crashdir",
                  metavar = "STR",
                  help    = "Set the directory name for copying the events that crashed the trigger")

    p.add_option( "--histo_dir",
                  default = "",
                  type    = "string",
                  dest    = "histodir",
                  metavar = "STR",
                  help    = "Set the directory name for copying the output histograms")

    p.add_option( "--service_class",
                  default = "",
                  type    = "string",
                  dest    = "svcclass",
                  metavar = "STR",
                  help    = "Set the service class for CASTOR IO")
                  
    p.add_option( "--file_prefix",
                  default = "",
                  type    = "string",
                  dest    = "fileprefix",
                  metavar = "STR",
                  help    = "Set the directory name for copying the output files")

    p.add_option( "--job_id",
                  default = "",
                  type    = "string",
                  dest    = "jobid",
                  metavar = "STR",
                  help    = "Set the jobId that identifys the processing")

    p.add_option( "--max_events",
                  default = -1,
                  type    = "int",
                  dest    = "maxevents",
                  metavar = "NUM",
                  help    = "Set the maximum events to be reprocessed per job")

    p.add_option( "--run_number",
                  default = 0,
                  type    = "int",
                  dest    = "runnumber",
                  metavar = "NUM",
                  help    = "Set the run number to which the events belong")

    p.add_option( "--jobOptions",
                  default = "",
                  type    = "string",
                  dest    = "joboptions",
                  metavar = "FILE",
                  help    = "Set the jobOptions to be used for the reprocessing")

    p.add_option( "--options",
                  default = "",
                  type    = "string",
                  dest    = "options",
                  metavar = "STR",
                  help    = "Set the options for the jobOptions")

    p.add_option( "--smk",
                  default = "",
                  type    = "string",
                  dest    = "smk",
                  metavar = "SMK",
                  help    = "Set the super master key for the database configuration")

    p.add_option( "--lvl1ps",
                  default = "",
                  type    = "string",
                  dest    = "lvl1ps",
                  metavar = "L1PS",
                  help    = "Set the prescales sets for the LVL1")

    p.add_option( "--hltps",
                  default = "",
                  type    = "string",
                  dest    = "hltps",
                  metavar = "HLTPS",
                  help    = "Set the prescales sets for the HLT")

    p.add_option( "--post_commands",
                  default = "",
                  type    = "string",
                  dest    = "post_commands",
                  metavar = "STR",
                  help    = "Postcommands to change configuration on top of DB configuration")

    p.add_option( "--conditions",
                  action  = "append",
                  type    = "string",
                  dest    = "conditions",
                  metavar = "STR",
                  help    = "List of new \'folder tag\' to be used for the run" )

    p.add_option( "--filters",
                  action  = "append",
                  type    = "string",
                  dest    = "filters",
                  metavar = "STR",
                  help    = "List of filters to be applied on the stream tag of the accept event" )

    p.add_option( "--stream_out",
                  action  = "append",
                  type    = "string",
                  dest    = "stream_out",
                  metavar = "STR",
                  help    = "Specified which event tags have to be streamed out by Streamer" )

    p.add_option( "--connection",
                  default = "",
                  type    = "string",
                  dest    = "connection",
                  metavar = "STR",
                  help    = "Set the connection to the configuration database")

    p.add_option( "--proc_session",
                  default = "",
                  type    = "string",
                  dest    = "proc_session",
                  metavar = "STR",
                  help    = "Set the processing version")

    p.add_option( "--doLVL1Remapping",
                  default = "",
                  type    = "string",
                  dest    = "lvl1_remap",
                  metavar = "MOD",
                  help    = "Set the module to be used for LVL1 remapping")

    p.add_option( "--enLVL1prescales",
                  default = "",
                  type    = "string",
                  dest    = "en_lvl1_pre",
                  metavar = "MOD",
                  help    = "Set the module name to be used for enabling the LVL1 prescales")

    p.add_option( "-v",
                  "--verbose",
                  action  = 'store_true',
                  default = False,
                  dest    = "verbose",
                  help    = "enable the printout verbosity" )

    (opt, args) = p.parse_args()


    if len(args) != 1:
      p.error("incorrect number of arguments")

    input_params = {}
    
    if opt.doctp==True:            input_params['doCTP']             = True
    if opt.domuctpi==True:         input_params['doMUCTPI']          = True
    if opt.docopyin==True:         input_params['doCopyIn']          = True
    if opt.dostreaming==True:      input_params['doStreaming']       = True
    if opt.dodummystreaming==True: input_params['doDummyStreaming']  = True
    if opt.dorecording==True:      input_params['doRecording']       = True
    if opt.dol2==True:             input_params['doL2']              = True
    if opt.doef==True:             input_params['doEF']              = True
    if opt.dopre==True:            input_params['doPre']             = True
    if opt.dopost==True:           input_params['doPost']            = True
    if opt.dohltdebugwrite==True:  input_params['doHLTDebugWrite']   = True
    if opt.donottruncate==True:    input_params['doNotTruncateL2R']  = True
    
    if opt.lvl1_remap!="":    input_params['doLVL1Remapping'] = opt.lvl1_remap
    if opt.en_lvl1_pre!="":   input_params['enLVL1prescales'] = opt.en_lvl1_pre

    if opt.filestream!="":    input_params['file_stream']   = opt.filestream
    if opt.outputdir!="":     input_params['output_dir']    = opt.outputdir
    if opt.crashdir!="":      input_params['crash_dir']     = opt.crashdir
    if opt.histodir!="":      input_params['histo_dir']     = opt.histodir
    if opt.svcclass!="":      input_params['service_class'] = opt.svcclass
    if opt.fileprefix!="":    input_params['file_prefix']   = opt.fileprefix
    if opt.jobid!="":         input_params['job_id']        = str(opt.jobid)
    if opt.maxevents!=-1:     input_params['max_events']    = opt.maxevents
    if opt.runnumber!=0:      input_params['run_number']    = opt.runnumber
    if opt.joboptions!="":    input_params['jobOptions']    = opt.joboptions
    if opt.options!="":       input_params['options']       = opt.options
    if opt.smk!="":           input_params['smk']           = opt.smk
    if opt.lvl1ps!="":        input_params['lvl1ps']        = opt.lvl1ps
    if opt.hltps!="":         input_params['hltps']         = opt.hltps
    if opt.post_commands!="": input_params['post_commands'] = opt.post_commands
    if opt.connection!="":    input_params['connection']    = opt.connection
    if opt.conditions!=None:  input_params['conditions']    = opt.conditions
    if opt.filters!=None:     input_params['filters']       = opt.filters
    if opt.stream_out!=None:  input_params['stream_out']    = opt.stream_out
    if opt.proc_session!="":  input_params['proc_session']  = opt.proc_session

    
    input_params['files'] = args[0]

    if opt.verbose: print "\nInput Parameters to BatchHLTApps;"
    if opt.verbose: pprint.pprint (input_params)
    if opt.verbose: printenv()
    if opt.verbose: 
        app_log.info(' --- Start running application --- ')
        app_log.info(' ')
    
    run(input_params)
