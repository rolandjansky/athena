#!/usr/bin/env python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#########################################################################
##
## Tier-0 combined transformation for DQM histogram merging 
## and web display creation
##
##  - to be run in two modes: 
##    1. in regular (15min?) intervals, merges the new results with 
##       the statistics accumulated so far (cached on DQM AFS area)
##    2. at the end of a run, to obtain the final statistics/results
##       (replaces all temporary histograms/results) 
##
##  - input parameter: file containing a json dictionary consisting of the key/value pairs
##     1) 'inputHistFiles': python list 
##         ['datasetname#filename1', 'datasetname#filename2', ...] (input HIST_TMP dataset + file names)
##        or list of file dictionaries
##         [{'lfn':'fname1', 'checksum':'cks1', 'dsn':'dsn1', 'size':sz1, 'guid':'guid1', 'events':nevts1, ...}, 
##          {'lfn':'fname2', 'checksum':'cks2', 'dsn':'dsn2', 'size':sz2, 'guid':'guid2', 'events':nevts2, ...}, ...]
##     2) 'outputHistFile': string 'datasetname#filename'
##        (HIST output dataset name + file)
##     optional parameters:
##     3) 'incrementalMode': string ('True'/'False', default: 'False')
##        ('True': do incremental update of DQM webpages on top of existing statistics;
##         'False': create final DQM webpages, replace temporary ones)
##     4) 'postProcessing': string ('True'/'False', default: 'True')
##        ('False': run histogram merging and DQ assessment only;
##         'True': run additional post-processing step (fitting, etc.))
##     5) 'procNumber': int (number of processing pass, e.g. 1,2, ...)
##     6) 'runNumber': int  
##     7) 'streamName': string (e.g., physics_IDCosmic, physics_Express, ...)
##     8) 'projectTag': string (e.g., data10_7TeV, TrigDisplay)
##     9) 'allowCOOLUpload': string ('True'/'False', default: 'True')
##        ('True': allow upload of defects to database;
##         'False': do not upload defects to database)
##     10) 'doWebDisplay': string ('True'/'False', default: 'True')
##        ('True': run the web display;
##         'False': do not run the web display)
##     11) 'filepaths': dictionary; keys are 'basename' (top level directory
##                      of han configurations; 'Collisions', 'Cosmics', and
##                      'HeavyIons', which are themselves dicts with keys
##                      'minutes10', 'minutes30', 'run' with values the han
##                      configuration file paths relative to basename
##     12) 'productionMode': string ('True'/'False', default: 'True')
##         ('True': run as if on Tier-0. 'False': Assume testing.)
##
## (C) N. Boelaert, L. Goossens, A. Nairz, P. Onyisi, S. Schaetzel, M. Wilson 
##     (April 2008 - July 2010)
##    Modified for dumping resource log in case of problems
##    S. Kama (March 2011)
##    Transformed into accepting argJSON as input 
##    and reporting new format of jobReport.json
##    J. Guenther (February 2017)
#########################################################################

import sys, string, commands, os.path, os, json, time, pprint, xmlrpclib, traceback
#sami
import hashlib

#########################################################################

# Utility function

def getSubFileMap(fname, nevts=0) :
    if os.path.isfile(fname) :
        sz = os.path.getsize(fname)
        map = { 'name': fname,
                'file_size' : sz,
                'nentries' : nevts,
              }
    else : 
        map = {}
    return map

def publish_success_to_mq(run, ptag, stream, incr, ami, procpass, hcfg, isprod):
  import stomp, json, os, ssl
  from DataQualityUtils import stompconfig
  dest='/topic/atlas.dqm.progress'
  conn=stomp.Connection([('atlas-mb.cern.ch', 61013)], **stompconfig.config())
  conn.start()
  conn.connect(wait=True)

  body = {
    'run': run,
    'project_tag': ptag,
    'stream': stream,
    'ami': ami,
    'pass': procpass,
    'hcfg': hcfg,
    }
  headers = {
    'MsgClass':'DQ', 
    'MsgType': (('' if isprod else 'Development') +
                ('WebDisplayRunComplete' if not incr else 'WebDisplayIncremental')),
    'type':'textMessage', 
    'persistent': 'true',
    'destination': dest,
    }
  conn.send(message=json.dumps(body), destination=dest,headers=headers,ack='auto')
  conn.disconnect()

#########################################################################

def genmd5sum(filename):
  md5summer=hashlib.md5()
  if os.path.isfile(filename):
    try:
      infil=open(filename,'rb')
      while True:
        fs=infil.read(8192)
        if not fs:
          break
        md5summer.update(fs)
    finally:
        infil.close()
  print "md5 sum of the \"%s\" is %s"%(filename,md5summer.hexdigest())
  return
      
def dq_combined_trf(jsonfile, outmap):

  print "\n##################################################################"
  print   "## STEP 1: creating file with list of root files ..."
  print   "##################################################################\n"
  
  nfiles=0

  try:
    # extract parameters from json file
    print "Using json file ", jsonfile, " for input parameters"
    f = open(jsonfile, 'r')
    parmap = json.load(f)
    f.close()

    print "\nFull Tier-0 run options:\n"
    pprint.pprint(parmap)

    inputfilelist = parmap.get('inputHistFiles', [])
    nfiles = len(inputfilelist) 
    histMergeCompressionLevel=parmap.get('histMergeCompressionLevel', 1)
    histMergeDebugLevel=parmap.get('histMergeDebugLevel', 0)
  except: 
    outmap['exitCode'] = 101
    outmap['exitAcronym'] = 'TRF_NOINPUT'
    outmap['exitMsg'] = 'Trouble reading json input dict.'
    traceback.print_exc()
    return
  
  if not nfiles :   # problem with job definition or reading json file
    outmap['exitCode'] = 102
    outmap['exitAcronym'] = 'TRF_NOINPUT'
    outmap['exitMsg'] = 'Empty input file list.'
    return

  histtmpflist = []
  nevts = 0

  try:
    if isinstance(inputfilelist[0], unicode) :  
      histtmpdsname = (inputfilelist[0]).split('#')[0]
      for val in inputfilelist :
        histtmpflist.append(val.split('#')[1])
    
    elif isinstance(inputfilelist[0], dict) :
      histtmpdsname = inputfilelist[0]['dsn']
      for fdict in inputfilelist :
        histtmpflist.append(fdict['lfn'])
        nevt = fdict.get('events', 0)
        if nevt is None:
          nevt=0
          print "WARNING Can't get number of events from input json file"
        nevts+=nevt
  
    f = open('hist_merge.list', 'w')
    txtstr = ""
    for hf in histtmpflist : 
      txtstr += "%s\n" % hf
    f.write(txtstr)
    f.close()

    cmd = "cat hist_merge.list"
    (s,o) = commands.getstatusoutput(cmd)
    print "\nContents of file hist_merge.list:\n"
    print o
  except:
    outmap['exitCode'] = 103
    outmap['exitAcronym'] = 'TRF_INPUTINFO'
    outmap['exitMsg'] = 'ERROR: crash in assembling input file list (STEP 1)'
    traceback.print_exc()
    return

  try:
    print "\n##################################################################"
    print   "## STEP 2: determining job parameters..."
    print   "##################################################################\n"

    # output file
    histdsname = (parmap['outputHistFile']).split('#')[0]
    histfile = (parmap['outputHistFile']).split('#')[1]
    amitag = histfile.split('.')[5]


    # incremental mode on/off
    incr = parmap.get('incrementalMode', 'False') 
    
    # post-processing on/off
    postproc = parmap.get('postProcessing', 'True')

    # database uploading on/off
    allowCOOLUpload = parmap.get('allowCOOLUpload', 'True')

    # do web display
    doWebDisplay = parmap.get('doWebDisplay', 'True')

    # production mode
    productionMode = parmap.get('productionMode', 'True')
    if productionMode != 'True' and incr == 'True':
      print("Production mode is not True, turning off incremental mode")
      incr = 'False'
    
    # get file paths, put into environment vars
    filepaths = parmap.get('filepaths', None)
    if filepaths and isinstance(filepaths, dict):
      if 'basename' not in filepaths:
        print("Improperly formed 'filepaths' (no 'basename')")
      else:
        for evtclass in ('Collisions', 'Cosmics', 'HeavyIons'):
          if evtclass not in filepaths:
            print("Improperly formed 'filepaths' (no '%s')" % evtclass)
          else:
            clinfo = filepaths[evtclass]
            for timeclass in ('run', 'minutes10', 'minutes30'):
              if timeclass not in clinfo:
                print("Improperly formed 'filepaths[%s]' (no '%s')" % (evtclass, timeclass))
              else:
                dqcenvvar = 'DQC_HCFG_%s_%s' % (evtclass.upper(), timeclass.upper())
                fpath = os.path.join(filepaths['basename'], clinfo[timeclass])
                print("Setting %s = %s" % (dqcenvvar, fpath))
                os.environ[dqcenvvar] = fpath

    # extract info from dataset name
    # AMI project name
    # override if tag has been specified in parmap
    try :
      dqproject = histdsname.split('.')[0]
    except :
      dqproject = 'data_test'
    dqproject = parmap.get('projectTag', dqproject)
    
    # run number
    if parmap.has_key('runNumber') : 
      runnr = parmap['runNumber']
    else :
      try :
        runnr = int(histdsname.split('.')[1])
      except :
        runnr = 1234567890

    # stream name  
    if parmap.has_key('streamName') : 
      stream = parmap['streamName']
    else :
      try :
        stream = histdsname.split('.')[2]
      except :
        stream = 'test_dummy'
    
    # processing pass number  
    MAX_XMLRPC_TRIES = 5 
    if parmap.has_key('procNumber') : 
      procnumber = parmap['procNumber']
    else :
      n_xmlrpc_tries = 1
      while n_xmlrpc_tries <= MAX_XMLRPC_TRIES :
        procnumber = 99
        try :
          xmlrpcserver = xmlrpclib.ServerProxy('http://atlasdqm.cern.ch:8888')
          procnumber = xmlrpcserver.get_next_proc_pass(runnr, stream, 'tier0')
          break 
        except :
          print 'Web service connection failed, attempt', n_xmlrpc_tries, 'of', MAX_XMLRPC_TRIES
          n_xmlrpc_tries += 1
          if n_xmlrpc_tries <= MAX_XMLRPC_TRIES:
            time.sleep(20*2**n_xmlrpc_tries)

    print "Job parameters:\n"
    print "  Run number:      ", runnr
    print "  Stream name:     ", stream
    print "  Processing pass: ", procnumber
    print "  Incremental mode:", incr
    print "  Post-processing: ", postproc
    print "  COOL uploads:    ", allowCOOLUpload
    print "  Production mode: ", productionMode

  except:
    outmap['exitCode'] = 104
    outmap['exitAcronym'] = 'TRF_JOBPARS'
    outmap['exitMsg'] = 'Error in determining job parameters (STEP 2).'
    traceback.print_exc()
    return

  try:
    print "\n##################################################################"
    print   "## STEP 3: running histogram merging procedure ..."
    print   "##################################################################\n"

    # environment setting
    os.environ['DQPRODUCTION'] = '1' if productionMode == 'True' else '0'
    print "Setting env variable DQPRODUCTION to %s\n" % os.environ['DQPRODUCTION']
    os.environ['DQ_STREAM'] = stream
    print "Setting env variable DQ_STREAM to %s\n" % os.environ['DQ_STREAM']
    os.environ['COOLUPLOADS'] = '1' if allowCOOLUpload == 'True' and productionMode == 'True' else '0'
    print "Setting env variable COOLUPLOADS to %s\n" % os.environ['COOLUPLOADS']

    if postproc == 'True' :
      if incr == 'True':
        cmd = "python -u `which DQHistogramMerge.py` hist_merge.list %s 1 1 %d %d " % (histfile,histMergeCompressionLevel,histMergeDebugLevel)
      else:        
        cmd = "python -u `which DQHistogramMerge.py` hist_merge.list %s 1 0 %d %d"  % (histfile,histMergeCompressionLevel,histMergeDebugLevel)
    else :  
      cmd = "python -u `which DQHistogramMerge.py` hist_merge.list %s 0 0 %d %d"    % (histfile,histMergeCompressionLevel,histMergeDebugLevel)
    
    print "Histogram merging command:\n"
    print cmd
    print "\n##################################################################\n"
    
    print "## ... logfile from DQHistogramMerge.py: "
    print "--------------------------------------------------------------------------------"
    tstart = time.time()
    # execute command
    retcode1 = os.system(cmd)
    print "--------------------------------------------------------------------------------"
    t1 = time.time()
    dt1 = int(t1 - tstart)
    
    print "\n## DQHistogramMerge.py finished with retcode = %s" % retcode1
    print   "## ... elapsed time: ", dt1, " sec"

    if retcode1 != 0 :
      outmap['exitCode'] = retcode1
      outmap['exitAcronym'] = 'TRF_DQMHISTMERGE_EXE'
      outmap['exitMsg'] = 'ERROR: DQHistogramMerge.py execution problem! (STEP 3).'
      print "ERROR: DQHistogramMerge.py execution problem!"
      retcode = retcode1
      txt = 'DQHistogramMerge.py execution problem'
      try:
        try:
          infilelist=open('hist_merge.list','r')
          for infname in infilelist:
            genmd5sum(infname.rstrip(os.linesep))
        finally:
            infilelist.close()
        genmd5sum(histfile)
        DQResFile="DQResourceUtilization.txt"
        if os.path.exists(DQResFile):
          print "dumping resource utilization log"
          with open(DQResFile) as resfile:
            for resline in resfile:
              print resline,
      except:
        outmap['exitMsg'] = 'ERROR: DQHistogramMerge.py execution problem + problem dumping DQResourceUtilization! (STEP 3).'
        traceback.print_exc()
        print "ERROR: DQHistogramMerge.py execution problem + problem dumping DQResourceUtilization!"
      return

    if postproc == 'True' and incr == 'False':
      print "\n##################################################################"
      print "## STEP 3b: copying postprocessing output to AFS ..."
      print "##################################################################\n"

      cmd = "python -u `which DQFileMove.py` %s %s_%s_%s" % (dqproject, runnr, stream, procnumber)

      print "File move command:\n"
      print cmd
      print "\n##################################################################\n"

      print "## ... logfile from DQFileMove.py: "
      print "--------------------------------------------------------------------------------"
      # execute command
      retcode1b = os.system(cmd)
      print "--------------------------------------------------------------------------------"
      t1b = time.time()
      dt1b = int(t1b - t1)
      t1 = t1b

      print "\n## DQFileMove.py finished with retcode = %s" % retcode1b
      print   "## ... elapsed time: ", dt1b, " sec"
  except:
      outmap['exitCode'] = 105
      outmap['exitAcronym'] = 'TRF_DQMHISTMERGE_EXE'
      outmap['exitMsg'] = 'ERROR: Failure in histogram merging or copying postprocessing output to AFS (STEP 3/3b).'
      traceback.print_exc()
      return

  try:
    retcode2 = 0
    dt2 = 0
    if doWebDisplay == 'True':
      print "\n##################################################################"
      print   "## STEP 4: running web-display creation procedure ..."
      print   "##################################################################\n"

      cmd = "python -u `which DQWebDisplay.py` %s %s %s %s stream=%s" % (histfile, dqproject, procnumber, incr, stream)

      print "Web display creation command:\n"
      print cmd
      print "\n##################################################################\n"

      print "## ... logfile from DQWebDisplay.py: "
      print "--------------------------------------------------------------------------------"
      # execute command
      retcode2 = os.system(cmd)
      print 'DO NOT REPORT "Error in TH1: cannot merge histograms" ERRORS! THESE ARE IRRELEVANT!'
      print "--------------------------------------------------------------------------------"
      t2 = time.time()
      dt2 = int(t2 - t1)

      print "\n## DQWebDisplay.py finished with retcode = %s" % retcode2
      print   "## ... elapsed time: ", dt2, " sec"
      if not (retcode2 >> 8) in (0, 5) :
        print "ERROR: DQWebDisplay.py execution problem!"
        outmap['exitCode'] = retcode2
        outmap['exitAcronym'] = 'TRF_DQMDISPLAY_EXE'
        outmap['exitMsg'] = 'ERROR: DQWebDisplay.py execution problem! (STEP 4).'
        try:
          infilelist=open('hist_merge.list','r')
          for infname in infilelist:
            genmd5sum(infname.rstrip(os.linesep))
        finally:
          infilelist.close()
        genmd5sum(histfile)
        return
      if productionMode == 'True': 
          try:
              print 'Publishing to message service'
              publish_success_to_mq(runnr, dqproject, stream, incr=(incr=='True'), ami=amitag, procpass=procnumber, hcfg=filepaths, isprod=(productionMode=='True'))
          except:
              outmap['exitCode'] = 106
              outmap['exitAcronym'] = 'TRF_DQMDISPLAY_EXE'
              outmap['exitMsg'] = 'ERROR: Failure in publishing info to messaging service (STEP 4).'
              traceback.print_exc()
              return
    else:
      print "\n##################################################################"
      print   "## WEB DISPLAY CREATION SKIPPED BY USER REQUEST"
      print   "##################################################################\n"
      print 'Web display off, not publishing to message service'
  except: 
    outmap['exitCode'] = 106
    outmap['exitAcronym'] = 'TRF_DQMDISPLAY_EXE'
    outmap['exitMsg'] = 'ERROR: Failure in web-display creation procedure (STEP 4).'
    print 'ERROR: Failure in web-display creation procedure (STEP 4).'
    traceback.print_exc()
    return
  
  print "\n##################################################################"
  print   "## STEP 5: finishing the job ..."
  print   "##################################################################\n"
        
  # get info for report json file
  try:
    outfiles = [getSubFileMap(histfile, nevts=nevts)]
    # assemble job report map
    outmap['files']['output'][0]['dataset'] = histdsname
    outmap['files']['output'][0]['subFiles'] = outfiles
    outmap['resource']['transform']['processedEvents'] = long(nevts)
    return
  except: 
    outmap['exitCode'] = 107
    outmap['exitAcronym'] = 'TRF_JOBREPORT'
    outmap['exitMsg'] = 'ERROR: in job report creation (STEP 5)'
    print "ERROR: in job report creation (STEP 5) !"
    traceback.print_exc()
    return

def dq_trf_wrapper(jsonfile):
  print "\n##################################################################"
  print   "##              ATLAS Tier-0 Offline DQM Processing             ##"
  print   "##################################################################\n"

  outmap = { 'exitAcronym' : 'OK',
               'exitCode' : 0,
               'exitMsg' : 'trf finished OK',
               'files' : { 'output' : [{ 'dataset' : '',
                                         'subFiles' : [ {},
                                                      ]}
                                    ] },
                'resource' : { 'transform' : { 'processedEvents' : 0L } }
                 }

  # dq_combined_trf will update outmap
  tstart = time.time()
  dq_combined_trf(jsonfile, outmap)
  outmap['resource']['transform']['wallTime'] = int(time.time() - tstart) 
   
  # dump json report map
  f = open('jobReport.json', 'w')
  json.dump(outmap, f)
  f.close()

  # summarize status
  print "\n## ... job finished with retcode : %s" % outmap['exitCode']
  print   "## ... error acronym: ", outmap['exitAcronym']
  print   "## ... job status message: ", outmap['exitMsg']
  print   "## ... elapsed time: ", outmap['resource']['transform']['wallTime'], "sec"
  print   "##"
  print   "##################################################################"
  print   "## End of job."
  print   "##################################################################\n"


########################################
## main()
########################################

if __name__ == "__main__":

  if (len(sys.argv) != 2) and (not sys.argv[1].startswith('--argJSON=')) :
    print "Input format wrong --- use "
    print "   --argJSON=<json-dictionary containing input info> "
    print "   with key/value pairs: "
    print "     1) 'inputHistFiles': python list "
    print "          ['datasetname#filename1', 'datasetname#filename2',...] (input dataset + file names) "
    print "        or list of file dictionaries "
    print "          [{'lfn':'fname1', 'checksum':'cks1', 'dsn':'dsn1', 'size':sz1, 'guid':'guid1', 'events':nevts1, ...}, " 
    print "           {'lfn':'fname2', 'checksum':'cks2', 'dsn':'dsn2', 'size':sz2, 'guid':'guid2', 'events':nevts2, ...}, ...] "
    print "     2) 'outputHistFile': string 'datasetname#filename' "
    print "        (HIST output dataset name + file) "
    print "     optional parameters: "
    print "     3) 'incrementalMode': string ('True'/'False') "
    print "        ('True': do incremental update of DQM webpages on top of existing statistics; "
    print "         'False': create final DQM webpages, replace temporary ones) "
    print "     4) 'postProcessing': string ('True'/'False', default: 'True') "
    print "        ('False': run histogram merging and DQ assessment only; "
    print "         'True': run additional post-processing step (fitting, etc.)) "
    print "     5) 'procNumber': int (number of processing pass, e.g. 1,2, ...) "
    print "     6) 'runNumber': int "  
    print "     7) 'streamName': string (e.g., physics_IDCosmic, physics_Express, ...) "  
    print "     8) 'projectTag': string (e.g., data10_7TeV, TrigDisplay)"
    print "     9) 'allowCOOLUpload': string ('True'/'False', default: 'True')"
    print "        ('True': allow upload of defects to database; "
    print "         'False': do not upload defects to database)"
    sys.exit(-1)
  
  else :
    jsonfile = sys.argv[1][len('--argJSON='):]
    dq_trf_wrapper(jsonfile)
  
