#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################
##
## Solve trf for Tier-0: 
##
##  - input parameter: file containing a pickled dictionary consisting of the key/value pairs
##     1) 'inputTFiles': python list, input TFiles 
##         ['datasetname#filename1', 'datasetname#filename2', ...] 
##     2) 'inputAlignmentConstants':  string, input alignment constants, can be empty
##         'alignmentconstantsname#filename.root' 
##
##     3) 'outputAlignmentConstants': string, output alignment constants
##         'name#AlignmentConstants.root'
##     4) 'outputAlignmentConstants': string, output alignment constants, db file
##         'name#AlignmentConstants.db'
##	   5) 'outputAlignFiles': string, output alignment files
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
  print   "##             ATLAS Tier-0 Alignment Processing                   ##"
  print   "##################################################################\n"

  # extract parameters from pickle file
  print "Using pickled file ", picklefile, " for input parameters"
  f = open(picklefile, 'r')
  parmap = pickle.load(f)
  f.close()

  print "\nFull Tier-0 run options:\n"
  pprint.pprint(parmap)

  inputfilelist = parmap.get('inputTFiles', [])
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
    inputTFiles = []
    inputRAWFileList = []
    for val in parmap['inputTFiles']:
      #inputRAWFileList.append(val.split('#')[1])
      inputTFiles.append(val['pfn'])
    for val in parmap['inputRAWFiles']:
      #inputRAWFileList.append(val.split('#')[1])
      inputRAWFileList.append(val['pfn'])
    runnr = int(inputRAWFileList[0].split('.')[-8])
    print "runnr = ", runnr 
    #runnr = int(inputTFiles[0].split('.')[-8])
    
    # input alignment constants
    iteration = int(parmap['iteration']) 
    try:
      inputAlignmentConstants = (parmap['inputAlignmentConstants']).split('#')[1]
    except:
      inputAlignmentConstants = ""
    if iteration >= 1:
	    inputAlignmentConstants = "/afs/cern.ch/user/a/atlidali/w0/calibLoop/data14_cosmics/tmpconstants/%d_AlignmentConstants_Iter%d.root" % (runnr,iteration-1)
    # output file
    ouputAlignmentConstantsds = (parmap['outputAlignmentConstants']).split('#')[0]
    ouputAlignmentConstants = (parmap['outputAlignmentConstants']).split('#')[1]
  
    
    # output db file
    ouputAlignmentConstantsdbds = (parmap['outputAlignmentConstantsdb']).split('#')[0]
    ouputAlignmentConstantsdb = (parmap['outputAlignmentConstantsdb']).split('#')[1]
    
    
    outputAlignFilesds = (parmap['outputAlignFiles']).split('#')[0]
    outputAlignFiles = (parmap['outputAlignFiles']).split('#')[1]
    
    '''
    # get beamspot tag
    from InDetBeamSpotExample import COOLUtils
    beamspottag = COOLUtils.resolveCurrentBeamSpotFolder()'''
    # assemble jobOptions fragment
    (s,o) = commands.getstatusoutput('rm -f myJobOptions.py')
    jOFile = open('myJobOptions.py', 'w')
    if iteration <= 1:
    	cont = '''
###############################################################
#
#  Custom configuration jobOptions file
#
###############################################################

inputFiles		= %s
inputTFiles 		= %s
inputAlignmentConstants = "%s"
outputPoolFile 		= "%s"
numberOfEvents		= 1 #Has to be one to get the reconstruction options


##-------- Alignment Configuration --------------------

if len(inputAlignmentConstants) !=0:
	readConstantsFromPool	=	True
	inputPoolFiles	=	[inputAlignmentConstants]
else:
	readConstantsFromPool	=	False

WriteTFile = True
runAccumulate	=	False
runSolving	=	True

##-------- Extra Configuration --------------------
#from InDetBeamSpotExample import COOLUtils
#beamSpotTag = COOLUtils.resolveCurrentBeamSpotFolder()
#beamSpotTag = "IndetBeampos-ES1-UPD2-03"

#print beamSpotTag

doReadBS	=	False
doBSConstraint	=       False
doPVConstraint	=	False
doTrkNtuple	=	False
doMonitoring	=	False

Cosmics 	=	True
realData	=	True
useTRT		=	True
BField		=	True
dataSource	=	'data' 
projectName	= 	'data14_cos'
PtCut		=	100

#Alignment Method is GX2
runLocal	=	False
solveLocal	=	False
solvingOption	=	1

## solving option
##   0 - No global solving
##   1 - Lapack
##   2 - ***REMOVED***
##   6 - ROOT
##   7 - CLHEP


ModCut	=	0

# Temp configuration
ModCut = 0
detectorDescription = "ATLAS-R2-2015-02-00-00"
globalTag         = "CONDBR2-ES1PA-2014-01"
#siAlignmentTag = "InDetAlign_PeriodD2012_d0z0p_constrained" # changed 11-Oct-12
siAlignmentTag = ""
#trtAlignmentTag   = "TRTAlign_PeriodD2012_d0z0p_constrained" # changed 11-Oct-12
#trtAlignmentTagL3 = "TRTCalibDX_EoverP_189598_189845v1"



## Pixel Alignment Levels 
pixelAlignmentLevel	=	11
pixelAlignmentLevelBarrel	=	-1
pixelAlignmentLevelEndcaps	=	-1

## Pixel alignment DoFs
alignPixel	  = True
pixelAlignBarrel  = True
pixelAlignEndcaps = True
## alignment parameters Barrel
pixelAlignBarrelX = True
pixelAlignBarrelY = True
pixelAlignBarrelZ = True
pixelAlignBarrelRotX = False
pixelAlignBarrelRotY = True
pixelAlignBarrelRotZ = True
## alignment parameters Endcap
pixelAlignEndcapX = True
pixelAlignEndcapY = True
pixelAlignEndcapZ = False
pixelAlignEndcapRotX = False
pixelAlignEndcapRotY = False
pixelAlignEndcapRotZ = True


## SCT Alignment Levels 
sctAlignmentLevel		=	1
sctAlignmentLevelBarrel		=	-1
sctAlignmentLevelEndcaps	=	-1

## SCT alignment DoFs
alignSCT	=	True
sctAlignBarrel  = True
sctAlignEndcaps = False

## alignment parameters Barrel
sctAlignBarrelX = True
sctAlignBarrelY = True
sctAlignBarrelZ = False
sctAlignBarrelRotX = False
sctAlignBarrelRotY = False
sctAlignBarrelRotZ = True
## alignment parameters Endcap
sctAlignEndcapX = True
sctAlignEndcapY = True
sctAlignEndcapZ = True
sctAlignEndcapRotX = True
sctAlignEndcapRotY = True
sctAlignEndcapRotZ = True


alignTRT	=	False
trtAlignmentLevel	=	1

trtAlignmentLevel = 1
trtAlignBarrel  = True
trtAlignEndcaps = True
## alignment parameters Barrel
trtAlignBarrelX = True
trtAlignBarrelY = True
trtAlignBarrelZ = False
trtAlignBarrelRotX = True
trtAlignBarrelRotY = True
trtAlignBarrelRotZ = True
## alignment parameters Endcap
trtAlignEndcapX = True
trtAlignEndcapY = True
trtAlignEndcapZ = False
trtAlignEndcapRotX = True
trtAlignEndcapRotY = True
trtAlignEndcapRotZ = True


##-------- Load Reconstruction --------------------
import os

##-------- Load Reconstruction --------------------
include("InDetAlignExample/jobOption_RecExCommon.py") 
include("InDetAlignExample/jobOption_ConditionsOverrider.py") 

##-------- Load Alignment --------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py") 

###############################################################
  	''' % (str(inputRAWFileList), str(inputTFiles), inputAlignmentConstants, ouputAlignmentConstants)#,beamspottag)
    else:
    	cont = '''
###############################################################
#
#  Custom configuration jobOptions file
#
###############################################################

inputFiles		= %s
inputTFiles 		= %s
inputAlignmentConstants = "%s"
outputPoolFile 		= "%s"
numberOfEvents		= 1 #Has to be one to get the reconstruction options


##-------- Alignment Configuration --------------------

if len(inputAlignmentConstants) !=0:
	readConstantsFromPool	=	True
	inputPoolFiles	=	[inputAlignmentConstants]
else:
	readConstantsFromPool	=	False

WriteTFile = True
runAccumulate	=	False
runSolving	=	True

##-------- Extra Configuration --------------------
#from InDetBeamSpotExample import COOLUtils
#beamSpotTag = COOLUtils.resolveCurrentBeamSpotFolder()
#beamSpotTag = "IndetBeampos-ES1-UPD2-03"

#print beamSpotTag

doReadBS	=	False
doBSConstraint	=       False
doPVConstraint	=	False
doTrkNtuple	=	False
doMonitoring	=	False

Cosmics 	=	True
realData	=	True
useTRT		=	True
BField		=	True
dataSource	=	'data' 
projectName	= 	'data14_cos'
PtCut		=	100

#Alignment Method is GX2
runLocal	=	False
solveLocal	=	False
solvingOption	=	1

## solving option
##   0 - No global solving
##   1 - Lapack
##   2 - ***REMOVED***
##   6 - ROOT
##   7 - CLHEP


ModCut	=	0

# Temp configuration
ModCut = 0
detectorDescription = "ATLAS-R2-2015-02-00-00"
globalTag         = "CONDBR2-ES1PA-2014-01"
#siAlignmentTag = "InDetAlign_PeriodD2012_d0z0p_constrained" # changed 11-Oct-12
siAlignmentTag = ""
#trtAlignmentTag   = "TRTAlign_PeriodD2012_d0z0p_constrained" # changed 11-Oct-12
#trtAlignmentTagL3 = "TRTCalibDX_EoverP_189598_189845v1"



## Pixel Alignment Levels 
pixelAlignmentLevel	=	2
pixelAlignmentLevelBarrel	=	-1
pixelAlignmentLevelEndcaps	=	-1

## Pixel alignment DoFs
alignPixel	  = True
pixelAlignBarrel  = True
pixelAlignEndcaps = True
## alignment parameters Barrel
pixelAlignBarrelX = True
pixelAlignBarrelY = True
pixelAlignBarrelZ = True
pixelAlignBarrelRotX = False
pixelAlignBarrelRotY = True
pixelAlignBarrelRotZ = True
## alignment parameters Endcap
pixelAlignEndcapX = True
pixelAlignEndcapY = True
pixelAlignEndcapZ = False
pixelAlignEndcapRotX = False
pixelAlignEndcapRotY = False
pixelAlignEndcapRotZ = True


## SCT Alignment Levels 
sctAlignmentLevel		=	2
sctAlignmentLevelBarrel		=	-1
sctAlignmentLevelEndcaps	=	1

## SCT alignment DoFs
alignSCT	=	True
sctAlignBarrel  = True
sctAlignEndcaps = True

## alignment parameters Barrel
sctAlignBarrelX = True
sctAlignBarrelY = False
sctAlignBarrelZ = False
sctAlignBarrelRotX = False
sctAlignBarrelRotY = True
sctAlignBarrelRotZ = True
## alignment parameters Endcap
sctAlignEndcapX = True
sctAlignEndcapY = True
sctAlignEndcapZ = False
sctAlignEndcapRotX = False
sctAlignEndcapRotY = False
sctAlignEndcapRotZ = True


alignTRT	=	False
trtAlignmentLevel	=	1

trtAlignmentLevel = 1
trtAlignBarrel  = True
trtAlignEndcaps = True
## alignment parameters Barrel
trtAlignBarrelX = True
trtAlignBarrelY = True
trtAlignBarrelZ = False
trtAlignBarrelRotX = True
trtAlignBarrelRotY = True
trtAlignBarrelRotZ = True
## alignment parameters Endcap
trtAlignEndcapX = True
trtAlignEndcapY = True
trtAlignEndcapZ = False
trtAlignEndcapRotX = True
trtAlignEndcapRotY = True
trtAlignEndcapRotZ = True

##-------- Load Reconstruction --------------------
import os

##-------- Load Reconstruction --------------------
include("InDetAlignExample/jobOption_RecExCommon.py") 
include("InDetAlignExample/jobOption_ConditionsOverrider.py") 

##-------- Load Alignment --------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py") 

###############################################################
  	''' % (str(inputRAWFileList), str(inputTFiles), inputAlignmentConstants, ouputAlignmentConstants)#,beamspottag)
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

    # Move alignment constants db file
    os.system("mv mycool.db %s\n" % ouputAlignmentConstantsdb)
    if iteration>=0:
        os.system("cp %s /afs/cern.ch/user/a/atlidali/w0/calibLoop/data14_cosmics/tmpconstants/%d_AlignmentConstants_Iter%d.root" %(ouputAlignmentConstants,runnr,iteration))
        dataset = "data14_cosmics.%08d.calibration_IDTracks.daq.RAW" % runnr
        if os.path.isfile("/afs/cern.ch/atlas/project/tzero/id/run/idTOM.msg"):
          os.system("echo \"call pdb updateDatasetPState %s aligned Iter%d\" >> /afs/cern.ch/atlas/project/tzero/id/run/idTOM.msg" % (dataset,iteration))
        else:
          os.system("echo \"call pdb updateDatasetPState %s aligned Iter%d\" > /afs/cern.ch/atlas/project/tzero/id/run/idTOM.msg" % (dataset,iteration))
    
    os.system("tar -cf %s *.txt *.root %s %s" % (outputAlignFiles,ouputAlignmentConstantsdb,ouputAlignmentConstants)) 
        #os.system("./makeRootFile.py alignlogfile.txt %s\n" % outputCorrectionsTree)
    # assemble report pickle file
    outfiles = []
    infiles = []
    nevts = 0
    acronym = 'ALLOK'
    txt = 'trf finished OK'  

    # get info for report gpickle file
    if retcode == 0 :
      # one could determine the number of processed events here, e.g. by grep'ping in the logfile
      constantsmap = getFileMap(ouputAlignmentConstants, ouputAlignmentConstantsds, nevts=nevts)
      outfiles.append(constantsmap)
      constantsdbmap = getFileMap(ouputAlignmentConstantsdb, ouputAlignmentConstantsdbds, nevts=nevts)
      outfiles.append(constantsdbmap)
      outputAlignFilesmap = getFileMap(outputAlignFiles, outputAlignFilesds, nevts=nevts)
      outfiles.append(outputAlignFilesmap)
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

  # move TFile
  os.system("mv AlignmentTFile.root TotalAlignmentTFile.root")

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
