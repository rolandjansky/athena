#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
##  - input parameter: file containing a pickled dictionary consisting of the key/value pairs
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
#########################################################################

import sys, string, commands, os.path, os, pickle, time, pprint, xmlrpclib
#sami
import hashlib

#########################################################################

# Utility function

def getFileMap(fname, dsname, nevts=0) :
  if os.path.isfile(fname) :
    sz = os.path.getsize(fname)
    map = { 'lfn': fname,
            'dataset' : dsname,
            'size' : sz,
            'events' : nevts
          }
  else : 
    map = {}
  return map

def publish_success_to_mq(run, ptag, stream, incr, ami, procpass, hcfg, isprod):
  import stomp, json, os, ssl
  from DataQualityUtils import stompconfig
  dest='/topic/atlas.dqm.progress'
  #conn=stomp.Connection([('atlas-mb.cern.ch', 61023)], use_ssl=True,
  #                      ssl_cert_file=os.environ['X509_USER_PROXY'],
  #                      ssl_version=ssl.PROTOCOL_TLSv1)
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
      
def dq_combined_trf(picklefile):
  
  tstart = time.time()

  print "\n##################################################################"
  print   "##              ATLAS Tier-0 Offline DQM Processing             ##"
  print   "##################################################################\n"

  print "\n##################################################################"
  print   "## STEP 1: creating file with list of root files ..."
  print   "##################################################################\n"

  # extract parameters from pickle file
  print "Using pickled file ", picklefile, " for input parameters"
  f = open(picklefile, 'r')
  parmap = pickle.load(f)
  f.close()

  print "\nFull Tier-0 run options:\n"
  pprint.pprint(parmap)

  inputfilelist = parmap.get('inputHistFiles', [])
  nfiles = len(inputfilelist) 
  histMergeCompressionLevel=parmap.get('histMergeCompressionLevel', 1)
  histMergeDebugLevel=parmap.get('histMergeDebugLevel', 0)
  
  if not nfiles :   # problem with job definition or reading pickle file
    dt = int(time.time() - tstart) 
    retcode = 1
    acronym = 'TRF_NOINPUT'
    txt = 'empty input file list'  
    reportmap = { 'prodsys': { 'trfCode': retcode,
                            'trfAcronym': acronym,  
                            'jobOutputs': [],
                            'jobInputs': [],
                            'nevents': 0,              
                            'more': { 'num1': 0, 'num2': dt, 'txt1': txt }
                          }
             }              
  
  else : 
    histtmpflist = []
    nevts = 0

    if isinstance(inputfilelist[0], str) :  
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
          print "WARNING Can't get number of events from input pickle file"
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
    

    print "\n##################################################################"
    print   "## STEP 3: running histogram merging procedure ..."
    print   "##################################################################\n"

    # environment setting
    os.environ['DQPRODUCTION'] = '1' if productionMode == 'True' else '0'
    os.environ['DQ_STREAM'] = stream
    print "Setting env variable DQPRODUCTION to %s\n" % os.environ['DQPRODUCTION']
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
    # execute command
    retcode1 = os.system(cmd)
    print "--------------------------------------------------------------------------------"
    t1 = time.time()
    dt1 = int(t1 - tstart)
    
    print "\n## DQHistogramMerge.py finished with retcode = %s" % retcode1
    print   "## ... elapsed time: ", dt1, " sec"

    if retcode1 == 0 :
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
      else:
        print "\n##################################################################"
        print   "## WEB DISPLAY CREATION SKIPPED BY USER REQUEST"
        print   "##################################################################\n"
        retcode2 = 0
        dt2 = 0

    print "\n##################################################################"
    print   "## STEP 5: finishing the job ..."
    print   "##################################################################\n"

    # assemble report gpickle file
    outfiles = []
    infiles = []
    
    retcode = 0
    acronym = 'OK'
    txt = 'trf finished OK'  
    
    # get info for report gpickle file
    if retcode1 == 0 :
      dt = dt1
      if (retcode2 >> 8) in (0, 5) :
        # if success, or if unable to acquire cache lock
        histmap = getFileMap(histfile, histdsname, nevts=nevts)
        outfiles = [histmap]
        dt += dt2
        if doWebDisplay == 'True':
          print 'Publishing to message service'
          publish_success_to_mq(runnr, dqproject, stream, incr=(incr=='True'), ami=amitag, procpass=procnumber, hcfg=filepaths, isprod=(productionMode=='True'))
        else:
          print 'Web display off, not publishing to message service'
      else :
        txt = 'DQWebDisplay.py execution problem'  
        print "ERROR: DQWebDisplay.py execution problem!"
        retcode = retcode2
        acronym = 'TRF_DQMDISPLAY_EXE'
        try:
          infilelist=open('hist_merge.list','r')
          for infname in infilelist:
            genmd5sum(infname.rstrip(os.linesep))
        finally:
          infilelist.close()
        genmd5sum(histfile)
    else :
      print "ERROR: DQHistogramMerge.py execution problem!"
      retcode = retcode1
      acronym = 'TRF_DQMHISTMERGE_EXE'
      dt = 0
      txt = 'DQHistogramMerge.py execution problem'
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
                  
    # assemble job report map
    reportmap = { 'prodsys': { 'trfCode': retcode,
                               'trfAcronym': acronym,  
                               'jobOutputs': outfiles,
                               'jobInputs': infiles,
                               'nevents': int(nevts),              
                               'more': { 'num1': int(nevts), 'num2': int(dt), 'txt1': txt }
                             }
                }              

  # pickle report map
  f = open('jobReport.gpickle', 'w')
  pickle.dump(reportmap, f)
  f.close()

  print "\n## ... job finished with retcode : %s" % reportmap['prodsys']['trfCode']
  print   "## ... error acronym: ", reportmap['prodsys']['trfAcronym']
  print   "## ... elapsed time: ", reportmap['prodsys']['more']['num2'], "sec"
  print   "##"
  print   "##################################################################"
  print   "## End of job."
  print   "##################################################################\n"


########################################
## main()
########################################

if __name__ == "__main__":

  if (len(sys.argv) != 2) and (not sys.argv[1].startswith('--argdict=')) :
    print "Input format wrong --- use "
    print "   --argdict=<pickled-dictionary containing input info> "
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
    picklefile = sys.argv[1][len('--argdict='):]
    dq_combined_trf(picklefile)
  
