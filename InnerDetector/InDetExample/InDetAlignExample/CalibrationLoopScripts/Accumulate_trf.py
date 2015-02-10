#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### last change by $Vicente Lacuesta$
### $09-03-2011$
### $0.9$

#########################################################################
##
## Accumulate trf for Tier-0: 
##
##  - input parameter: file containing a pickled dictionary consisting of the key/value pairs
##     1) 'inputRAWFiles': python list ['datasetname#filename1','datasetname#filename2',...]
##        (input RAW dataset + file names)
##     2) 'inputAlignmentConstants':  string 'datasetname#alignmentconstants.root'
##        (input alignment constants, can be empty)
##     3) 'alignmentLevel' : integer 
##        (alignment level to run)
##	   4) 'iteration' : integer
##		  number of current iteration
##     5) 'outputTFile': string 'datasetname#AlignmentTFile.root'
##        (output TFile containing the matrix, vector...)
##     6) 'outputMonitoringFile': string 'name#monitoring.root'
##        (output root file with plots)
##  - assembles custom jobOptions file, runs athena
##  - generates an outputTFile and a monitoring.root file
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

        
def accumData(picklefile) :

  t0 = time.time()
  
  print "\n##################################################################"
  print   "##             ATLAS Tier-0 Alignment Processing                ##"
  print   "##################################################################\n"

  # extract parameters from pickle file
  print "Using pickled file ", picklefile, " for input parameters"
  f = open(picklefile, 'r')
  parmap = pickle.load(f)
  f.close()

  print "\nAlignment Processiong run options:\n"
  for key in parmap.keys() :
    print key, '=', parmap[key] 
    print "#####################"
    
 
  inputRAWFileList = []
  for val in parmap['inputRAWFiles']:
      #inputRAWFileList.append(val.split('#')[1])
      inputRAWFileList.append(val['pfn'])
  #alignmentLevel = int(parmap['alignmentLevel']) 
   
  events = int(parmap['events'])
  iteration = int(parmap['iteration'])   
  #runnr = int(inputRAWFileList[0].split('.')[-8])
  runnr = int(inputRAWFileList[0].split('.')[-8]) 
  tfiledsname = (parmap['outputTFile']).split('#')[0]
  tfile = (parmap['outputTFile']).split('#')[1]
        
  mondsname = (parmap['outputMonitoringFile']).split('#')[0]
  monfile = (parmap['outputMonitoringFile']).split('#')[1]

  '''# get beamspot tag
  from InDetBeamSpotExample import COOLUtils
  beamspottag = COOLUtils.resolveCurrentBeamSpotFolder() '''

  if iteration >= 1:
  	inputAlignmentConstants = "/afs/cern.ch/user/a/atlidali/w0/calibLoop/data14_cosmics/tmpconstants/%d_AlignmentConstants_Iter%d.root" % (runnr,iteration-1)
  else:
  	inputAlignmentConstants = ""
  (s,o) = commands.getstatusoutput('rm -f myJobOptions.py')
  jOFile = open('myJobOptions.py', 'w')

  if iteration <=1:
  	cont = '''
###############################################################
#
#  Custom configuration jobOptions file
#
###############################################################

inputFiles		= %s
inputAlignmentConstants = "%s"
numberOfEvents		= %d

##-------- Alignment Configuration --------------------

if len(inputAlignmentConstants) !=0:
	readConstantsFromPool	=	True
	inputPoolFiles	=	[inputAlignmentConstants]
else:
	readConstantsFromPool	=	False

WriteTFile = True
runAccumulate	=	True
runSolving	=	False

##-------- Extra Configuration --------------------
#from InDetBeamSpotExample import COOLUtils
#beamSpotTag = COOLUtils.resolveCurrentBeamSpotFolder()
#beamSpotTag = "IndetBeampos-ES1-UPD2-03"

#print beamSpotTag

doReadBS	=	False
doBSConstraint	=       False
doPVConstraint	=	False
doTrkNtuple	=	False
doMonitoring	=	True

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

##-------- End of custom options -------------


##-------- Loading the Alignment Levels --------------------
include("InDetAlignExample/NewInDetAlignLevels.py") 

##-------- Reconstruction Configuration --------------------


##-------- Load Reconstruction -----------------------------
include("InDetAlignExample/jobOption_RecExCommon.py") 
include("InDetAlignExample/jobOption_ConditionsOverrider.py") 

##-------- Load Alignment ----------------------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py")
###############################################################
  	''' % (str(inputRAWFileList), inputAlignmentConstants,events)

  else:
  	cont = '''
###############################################################
#
#  Custom configuration jobOptions file
#
###############################################################

inputFiles		= %s
inputAlignmentConstants = "%s"
numberOfEvents		= %d

##-------- Alignment Configuration --------------------

if len(inputAlignmentConstants) !=0:
	readConstantsFromPool	=	True
	inputPoolFiles	=	[inputAlignmentConstants]
else:
	readConstantsFromPool	=	False

WriteTFile = True
runAccumulate	=	True
runSolving	=	False

##-------- Extra Configuration --------------------
#from InDetBeamSpotExample import COOLUtils
#beamSpotTag = COOLUtils.resolveCurrentBeamSpotFolder()
#beamSpotTag = "IndetBeampos-ES1-UPD2-03"

#print beamSpotTag

doReadBS	=	False
doBSConstraint	=       False
doPVConstraint	=	False
doTrkNtuple	=	False
doMonitoring	=	True

Cosmics 	=	True
realData	=	True
useTRT		=	True
BField		=	True
dataSource	=	'data' 
projectName	= 	'data14_cos'
PtCut		=	5000

#Alignment Method is GX2
runLocal	=	False
solveLocal	=	False
solvingOption	=	1

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

##-------- End of custom options -------------


##-------- Loading the Alignment Levels --------------------
include("InDetAlignExample/NewInDetAlignLevels.py") 

##-------- Reconstruction Configuration --------------------


##-------- Load Reconstruction -----------------------------
include("InDetAlignExample/jobOption_RecExCommon.py") 
include("InDetAlignExample/jobOption_ConditionsOverrider.py") 

##-------- Load Alignment ----------------------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py")
###############################################################
  	''' % (str(inputRAWFileList), inputAlignmentConstants,events)

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
  os.system("mv monitoring.root %s" % monfile)
  os.system("mv AlignmentTFile.root %s" % tfile)
  # assemble report pickle file

  outfiles = []
  infiles = []
  nevts = 0
  acronym = 'ALLOK'
  txt = 'trf finished OK'  

  # get info for report gpickle file
  if retcode == 0 :
    # one could determine the number of processed events here, e.g. by grep'ping in the logfile

    tfilemap = getFileMap(tfile, tfiledsname)
    monmap = getFileMap(monfile, mondsname)   
    outfiles.append(tfilemap)
    outfiles.append(monmap)
    infiles = inputRAWFileList
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
    print "     1) 'inputRAWFiles': python list ['datasetname#filename1','datasetname#filename2',...] "
    print "        (input RAW dataset + file names) "
    print "     2) 'inputAlignmentConstants':  string 'datasetname#alignmentconstants.root' "
    print "        (input alignment constants, can be empty)"
    print "     3) 'alignmentLevel': integer "
    print "        (alignment level to run)"
    print "     4) 'outputTFile': string 'datasetname#AlignmentTFile.root' "
    print "        (output TFile containing the matrix, vector...) "
    print "     5) 'level': integer "
    print "        (level to run the alignment)"
    sys.exit(-1)
  
  else :
    picklefile = sys.argv[1][len('--argdict='):]
    accumData(picklefile)

