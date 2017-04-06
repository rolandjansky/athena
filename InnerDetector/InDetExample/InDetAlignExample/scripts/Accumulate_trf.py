#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
##
## Accumulate trf for Tier-0: 
##
##  - input parameter: file containing a pickled dictionary consisting of the key/value pairs
##     1) 'inputRAWData': python list 
##         ['datasetname#filename1', 'datasetname#filename2', ...] (input dataset + file names) 
##     2) 'inputAlignmentConstants':  string, input alignment constants, can be empty
##         'alignmentconstantsname#filename.root' 
##
##     3) 'outputTFile': string, output TFile containing the matrix, vector...
##         'name#AlignmentTFile.root'
##     4) 'outputMonitoringFile': string, output root file with plots.
##         'name#monitoring.root'
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

  inputfilelist = parmap.get('inputRAWData', [])
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
    for val in parmap['inputRAWData']:
      infname = val.split('#')[1]
      inflist.append(infname)
    
    # alignment constants:
    
    try:
      inputAlignmentConstants = (parmap['inputAlignmentConstants']).split('#')[1]
    except:
      inputAlignmentConstants = ""
  
	# output TFile                                                                                           
    outdsname = (parmap['outputTFile']).split('#')[0]
    outfile = (parmap['outputTFile']).split('#')[1]
    outputTFile = (parmap['outputTFile']).split('#')[1]

    
    # output monitoring file
    outputMonitoringFile = (parmap['outputMonitoringFile']).split('#')[1]
    # assemble jobOptions fragment
    (s,o) = commands.getstatusoutput('rm -f myJobOptions.py')
    jOFile = open('myJobOptions.py', 'w')
    cont = '''
###############################################################
#
#  Custom configuration jobOptions file
#
###############################################################

inputFiles = %s
inputAlignmentConstants = "%s"
OutputTFileName = "%s"

PathBinName = OutputTFileName[:OutputTFileName.rfind('/')+1]
TFileName = OutputTFileName[OutputTFileName.rfind('/'):]

##-------- Alignment Configuration --------------------
WriteTFile = True
runLocal	=	False
sctAlignEndcaps	=	True
ModCut	=	6
sctAlignmentLevelEndcaps	=	-1
pixelAlignmentLevel	=	1
pixelAlignBarrel	=	True
if len(inputAlignmentConstants)!=0:
	readConstantsFromPool	=	True
	inputPoolFiles	=	[inputAlignmentConstants]
else:
	readConstantsFromPool	=	False
runAccumulate	=	True
alignSCT	=	True
pixelAlignmentLevelBarrel	=	-1
alignPixel	=	True
sctAlignmentLevel	=	1
pixelAlignEndcaps	=	True
solvingOption	=	1
pixelAlignmentLevelEndcaps	=	-1
trtAlignmentLevel	=	1
sctAlignmentLevelBarrel	=	-1
alignTRT	=	True
runSolving	=	False
sctAlignBarrel	=	True
solveLocal	=	False

##-------- Loading the Alignment Levels --------------------
include("InDetAlignExample/NewInDetAlignLevels.py") 

##-------- Reconstruction Configuration --------------------
errorScalingTag	=	"TrkErrorScaling_7T_2010_03"
numberOfEvents	=	100


detectorDescription	=	"ATLAS-GEO-16-00-00"
globalTag	=	"COMCOND-REPP-007-05"

##-------- Extra Configuration --------------------
ModCut	=	7
beamSpotTag	=	"IndetBeampos-ES1-UPD2"
doReadBS	=	True
doBSConstraint	=	True
useOldPreProcessor	=	False
doPVConstraint	=	False
doTrkNtuple	=	False
doMonitoring	=	True

##-------- End of custom options -------------

##-------- Load Reconstruction --------------------
include("InDetAlignExample/loadInDetRec_new.py") 
svcMgr.PoolSvc.AttemptCatalogPatch = True	
##-------- Load Alignment --------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py") 

###############################################################
  ''' % (str(inflist), inputAlignmentConstants, outputTFile)

    jOFile.write(cont)
    jOFile.close()

    # run athena
    cmd = "python -u `which athena.py` myJobOptions.py"
	
    print "\nRun command:"
    print cmd
    print "\nLogfile:"
    print "------------------------------------------------------------------"
    retcode = os.system(cmd)
    print "------------------------------------------------------------------"
    dt = int(time.time() - t0)

    print "\n## athena.py finished with retcode = %s" % retcode
    print   "## ... elapsed time: ", dt, " sec"

    # move Monitoring file
    os.system("mv monitoring.root %s" % outputMonitoringFile)
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
      print "ERROR: athena.py execution problem!"
      acronym = 'TRF_ATHENA_EXE'
      txt = "athena.py execution problem"

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
