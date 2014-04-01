#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
##
## MergeMon trf for Tier-0: 
##
##  - input parameter: file containing a pickled dictionary consisting of the key/value pairs
##     1) 'inputMonitoringFiles': python list 
##         ['datasetname#filename1.root', 'datasetname#filename2.root', ...] 
##     2) 'outputMonitoringMergedFile': string, output merged monitoring
##         'name#TotalMonitoring.root'
##
##  - assembles custom jobOptions file, runs athena
##
##########################################################################

import sys, string, commands, os.path, os, pickle, time, pprint, xmlrpclib

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

#########################################################################

        
def runAthena(picklefile) :

  t0 = time.time()
  
  print "\n##################################################################"
  print   "##             ATLAS Tier-0 Alignment Processing                ##"
  print   "##################################################################\n"

  # extract parameters from pickle file
  print "Using pickled file ", picklefile, " for input parameters"
  f = open(picklefile, 'r')
  parmap = pickle.load(f)
  f.close()

  print "\nFull Tier-0 run options:\n"
  pprint.pprint(parmap)

  inputfilelist = parmap.get('inputMonitoringFiles', [])
  nfiles = len(inputfilelist) 
  if not nfiles :   # problem with job definition or reading pickle file
    dt = int(time.time() - t0) 
    retcode = 1
    acronym = 'TRF_NOINPUT'
    txt = 'empty input file list'  
    outmap = { 'prodsys': { 'trfCode': retcode,
                            'trfAcronym': acronym,  
                            'jobOutputs': [],
                            'jobInputs': [],
                            'nevents': 0,
                            'more': { 'num1': 0, 'num2': dt, 'txt1': txt }
                          }
             }
  
  else : 
    # assemble list of ['filename1', 'filename2', ...]
    inflist = []
    for val in parmap['inputMonitoringFiles']:
      infname = val.split('#')[1]
      inflist.append(infname)
    
   
  
	# output merged Monitoring file                                                                                           
    outdsname = (parmap['outputMonitoringMergedFile']).split('#')[0]
    outfile = (parmap['outputMonitoringMergedFile']).split('#')[1]
    outputmonFile = (parmap['outputMonitoringMergedFile']).split('#')[1]

    
    
    # assemble jobOptions fragment
    (s,o) = commands.getstatusoutput('rm -f filelist')
    fileList = open('filelist.txt', 'w')
    for name in inflist:
    	fileList.write(name+'\n')
    fileList.close()
    
    

    # run athena
    cmd = "DQHistogramMerge.py filelist.txt %s True" % outputmonFile
	
    print "\nRun command:"
    print cmd
    print "\nLogfile:"
    print "------------------------------------------------------------------"
    retcode = os.system(cmd)
    print "------------------------------------------------------------------"
    dt = int(time.time() - t0)

    print "\n## DQHistogramMerge.py finished with retcode = %s" % retcode
    print   "## ... elapsed time: ", dt, " sec"

    # assemble report pickle file
    outfiles = []
    infiles = []
    nevts = 0
    acronym = 'ALLOK'
    txt = 'trf finished OK'  

    # get info for report gpickle file
    if retcode == 0 :
      # one could determine the number of processed events here, e.g. by grep'ping in the logfile
      fmap = getFileMap(outfile, outdsname, nevts=nevts)
      outfiles = [fmap]
    if retcode != 0 :
      print "ERROR: DQHistogramMerge execution problem!"
      acronym = 'TRF_PYTHON_EXE'
      txt = "DQHistogramMerge.py execution problem"

    # assemble job report map
    outmap = { 'prodsys': { 'trfCode': retcode,
                            'trfAcronym': acronym,
                            'jobOutputs': outfiles,
                            'jobInputs': infiles,
                            'more': { 'num1': int(nevts), 'num2': int(dt), 'txt1': txt }
                          }
             }
  
  # pickle report map
  f = open('jobReport.gpickle', 'w')
  pickle.dump(outmap, f)
  f.close()

  print "\n##################################################################"
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
    print "     1) 'inputFiles': python list "
    print "          ['datasetname#filename1','datasetname#filename2',...] (input dataset + file names) "
    print "     2) 'outputFile': string 'datasetname#filename' "
    print "        (merged output dataset name + file) "
    sys.exit(-1)
  
  else :
    picklefile = sys.argv[1][len('--argdict='):]
    runAthena(picklefile)

