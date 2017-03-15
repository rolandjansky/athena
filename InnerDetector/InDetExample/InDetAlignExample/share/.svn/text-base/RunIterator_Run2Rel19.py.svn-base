#!/afs/cern.ch/sw/lcg/external/Python/2.5.4/slc4_ia32_gcc34/bin/python
# =====================================================================
# Main script to run the NewInDetIterator
# run with ./RunIterator.py
# =====================================================================
import os
import sys


# ===============================
# Number of Iterations
# ===============================
FirstIteration = 0
Iterations     = 1

# ===============================
# Basic running options
# ===============================

# Run mode: local or batch
runMode = 'batch'
#runMode = 'local'
# Batch queue
#QUEUE = '1nh'
QUEUE          = 'atlasb1'

# Prefix to identify your jobs
preName        = 'm9_test'

HOME           = os.environ['HOME']
# Alignment Output
OutputPath     = os.environ['PWD']+"/."

CMTDIR         = HOME+"/athena/cmthome/"
# extra AtlasSetup options
ASetupOptions  = "single"
#ASetupOptions  = "nightlies"


# OutputLevel (DEBUG, INFO, VERBOSE)
OutputLevel    = 'INFO'


##########################################################
#             Inline Options Definition                  #
##########################################################

def optParsing():
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("--inputList", dest="inputList", help="Input txt file with the list of the files for the first data set you want to process", default="")
    parser.add_option("--inputList2", dest="inputList2", help="Input txt file with the list of the files for the second data set you want to process", default="")
    parser.add_option("--inputDir", dest="inputDir", help="Directory containing the input files - NOT YET IMPLEMENTED", default="")
    parser.add_option("--eventType", dest="eventType", help="Which type of events: collisions / cosmics", default="collisions")
    parser.add_option("--isData",dest="isData", help="Set itif you want to run on real data (Default: False)",action="store_true", default=False)
    parser.add_option("--inputConstantsFile", dest="inputConstantsFile", help="In the case you want to use a specific set of initial constants write here the absolute path to the pool file", default="")
    parser.add_option("--nEvents",dest="nEvents", type=int, help="Maximum number of events to use", default=-1)
    parser.add_option("--nCpus",dest="nCpus", help="Number of CPUs to be used",default=1)
    parser.add_option("--preIBLgeometry",dest="preIBLgeometry", help="Set it  if you want to run on Run1 geometry (Default: False)",action="store_true",default=False)
    parser.add_option("--ClusteringAlgo",dest="ClusteringAlgo", help="Which Clustering algorithm to use: NN (Default), Analog, Digital",default="NN")
    parser.add_option("--errorScalingTag",dest="errorScalingTag", help="Name of the error scaling tag or ES file to use",default="")
    parser.add_option("--ptmin",dest="userPtMin", help="Minimum pt of tracks to enter the align track selection *** in MeV ***", default = 0)
    
    (config, sys.argv[1:]) = parser.parse_args(sys.argv[1:])

    return config
    

##########################################################
#               Option Initialisation                    #
##########################################################
config = optParsing()
inputList=config.inputList
inputList2 = config.inputList2
inputDir =config.inputDir
eventType=config.eventType
ConstantsFile=config.inputConstantsFile
nEventsMax=config.nEvents
preIBLgeometry=config.preIBLgeometry
isData=config.isData
ClusteringAlgo=config.ClusteringAlgo
inputErrorScalingTag=config.errorScalingTag
nCpus=int(config.nCpus)  
userPtMin = float(config.userPtMin)


print " \n <RunIterator> ----- User input in command line --- "
print " <RunIterator> inputList = ",inputList
print " <RunIterator> nCpus = ",nCpus
print " <RunIterator> nEventsMax = ",nEventsMax
print " <RunIterator> inputList2 = ",inputList2
print " <RunIterator> ConstantsFile = ",ConstantsFile
print " <RunIterator> inputErrorScalingTag = ",inputErrorScalingTag
print " <RunIterator> userPtMin = ",userPtMin, " MeV"

##########################################################
#       Datasets to use
##########################################################

from InDetAlignExample.NewInDet_SetupData import setupData
DataToRun = []

#################### test to run BON and BOFF data
Data1 = setupData(eventType)
#Need To be changed this one
Data1.setDataType("IDCosmics") # Type of data: IDTracks, MinBias, IDCosmics or MC09_Multimuons by now
#Data1.setRuns([201280]) # Run number, you can provide a list of runs
Data1.setCPUs([nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus]) # Number of CPUs per iteration to process this data
#Data1.setEvents([5000,5000,5000]) # Number of total events to process
Data1.setEvents([nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax])
Data1.setLimitFiles(500) # Limit the number of files to use (useful when runing in local a subset of data)
#Data1.CreateFileList() # Creates the file list
if isData:
    Data1.setByteStream(True)
else:
    Data1.setByteStream(False)

Data1.setCustomFileList(inputList)

# Use custom Global Tag, if not given it will use the data default one (that is empty ;) )
if eventType == "collisions":
    #Data1.setGlobalTag("OFLCOND-MC12-IBL-20-30-25") 
    Data1.setGlobalTag("OFLCOND-RUN12-SDR-14")
else: 
    #Data1.setGlobalTag("OFLCOND-MC12-IBL-20-30-50") #Cosmics
    #Data1.setGlobalTag("CONDBR2-ES1PA-2014-01")
    if isData:
        #Data1.setGlobalTag("CONDBR2-ES1PA-2014-01")
        Data1.setGlobalTag("CONDBR2-ES1PA-2014-03") # move to 03 -> recomended by PF 
        # Data1.setDetDescrVersion("ATLAS-R2-2015-02-00-00")
        Data1.setDetDescrVersion("ATLAS-R2-2015-03-04-00")
    else:
        Data1.setGlobalTag("OFLCOND-RUN12-SDR-14")
        Data1.setDetDescrVersion("ATLAS-R2-2015-01-01-00")
#Data1.setDetDescrVersion("ATLAS-IBL3D25-04-00-01") # Use custom DetDescrVersion Tag, if not given it will use the data default one
#Data1.setDetDescrVersion("ATLAS-R2-2015-01-01-00")  --Bugged: IBL is rotated with respect to the real position
#Data1.setDetDescrVersion("ATLAS-R2-2015-01-01-00")

##############
############## deal with second data set
##############
if (len(inputList2)>1):
    Data2 = setupData("cosmics_boff")
    Data2.setCustomFileList(inputList2)
    # Need To be changed this one
    Data2.setDataType("IDCosmics") # Type of data: IDTracks, MinBias, IDCosmics or MC09_Multimuons by now
    Data2.setCPUs([nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus,nCpus]) # Number of CPUs per iteration to process this data
    Data2.setEvents([nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax,nEventsMax])
    Data2.setLimitFiles(500) # Limit the number of files to use (useful when runing in local a subset of data)
    if isData:
       Data2.setByteStream(True)
    else:
       Data2.setByteStream(False)

    # Use custom Global Tag, if not given it will use the data default one (that is empty ;) )
    if eventType == "collisions":
        Data2.setGlobalTag("OFLCOND-RUN12-SDR-14")
    else: 
        if isData:
            Data2.setGlobalTag("CONDBR2-ES1PA-2014-03") # move to 03 -> recomended by PF 
            Data2.setDetDescrVersion("ATLAS-R2-2015-02-00-00")
        else:
            Data2.setGlobalTag("OFLCOND-RUN12-SDR-14")
            Data2.setDetDescrVersion("ATLAS-R2-2015-01-01-00")
            
            
######################## continue
            
DataToRun.append(Data1) # always add this line to process the defined data
if (len(inputList2)>1): DataToRun.append(Data2) # always add this line to process the defined data


### Duplicate the previous lines to have as many datasets that you want

### I'm disabling the original SYNOPSIS.
### Fast and Dirty solution. Better to modify this part


### from InDetAlignExample.NewInDet_IteratorClasses import HandleRunOptions
### if len(sys.argv) > 1 or 'rtt' in os.environ["USER"]:
### DataToRun = [HandleRunOptions()]
    
for data in DataToRun:
    data.Print()

# Alignment Options
# =============================
# Global chi^2 alignment
# Alignment method: GX2, LX2
AlignmentMethod = "GX2"

# Alignment poolFile to be read on the first iteration, if empty, constants from database will be read
#inputAlignmentPoolFile = "/afs/cern.ch/user/m/mdanning/AlignmentTestArea/IBL_4br_30Jan/run_misalign/MisalignmentSet1.pool.root"
inputAlignmentPoolFile = ""
readConstantsFromPool = False

if ConstantsFile != "":
    inputAlignmentPoolFile=ConstantsFile
    readConstantsFromPool = True

# split files to subjobs to have exactly equal number of events (True)
# or just approximately based on the size of the input files (False)
doDetailedSplitting = False

# Option to use TFiles instead bin files
# if False it will use the standard bin files
useTFiles = True


# These options will be written to the jobOptions
# have a look into NewInDetAlignAlgSetup.py and NewInDetAlignGeometrySetup.py
# for all supported options

extraOptions = {}

extraOptions["Cosmics"] = False
if "cosmics" in eventType or "Cosmics" in eventType:
    extraOptions["Cosmics"] = True

extraOptions["realData"] = isData
if isData: 
   extraOptions["dataSource"]  = 'data' 
   extraOptions["projectName"] = 'data15_cos' 
   extraOptions["doReadBS"]=True
else: 
   extraOptions["dataSource"]  = 'geant4'  
   extraOptions["projectName"] = 'MC_Cosmic_BOFF' 
   extraOptions["doReadBS"]=False

extraOptions["doMonitoring"] = True
extraOptions["doTrkNtuple"] = False
extraOptions["BField"]  = False
extraOptions["BField_AlgSetup"] = extraOptions["BField"]  
extraOptions["PtCut"]               = userPtMin #Pt in MeV, comment the line to use default value (10000)
#extraOptions["siAlignmentTag"] = "InDetAlign_R2_Nominal"
extraOptions["siAlignmentTag"] = ""
#extraOptions["trtAlignmentTag"] = "TRTAlign_nominal"
#extraOptions["beamSpotTag"] = "IndetBeampos-ES1-UPD2"
#extraOptions["particleNumber"] = 0
extraOptions["ModCut"] = 0
extraOptions["softModeCut"]= 0.

#extraOptions["doBSConstraint"] = False
#extraOptions["doPVConstraint"] = False
#extraOptions["useOldPreProcessor"] = False
#extraOptions["eoverpmapconstraint"] = ""  # Full path of the map
#extraOptions["CosmicGRL"] = "CosmicsGRL.xml"
#extraOptions["CollisionGRL"] = "ColisionGRL.xml"
extraOptions["useTRT"] = True
extraOptions["readTRT"] = True
extraOptions["writeTRT"] = True
#extraOptions["TRTCalibTextFile"] = ""
if isData:
    extraOptions["TRTCalibT0TagCos"] = "" # "TrtCalibT0-AlignmentJune2010-00"
    extraOptions["TRTCalibRtTagCos"] = "" # "TrtCalibRt-AlignmentJune2010-00"
else:
    extraOptions["TRTCalibT0TagCos"] = "TrtCalibT0-MCCosmics_00-00" 
    extraOptions["TRTCalibRtTagCos"] = "TrtCalibRt-MCCosmics_00-00" 

## I'm fixing SCT to test IBL alignment
extraOptions["trtAlignBarrelZ"] = False
extraOptions["trtAlignEndcapZ"] = False
## PIXEL
extraOptions["pixelAlignBarrelX"] = True
extraOptions["pixelAlignBarrelY"] = True
extraOptions["pixelAlignBarrelZ"] = True
extraOptions["pixelAlignBarrelRotX"] = True
extraOptions["pixelAlignBarrelRotY"] = True
extraOptions["pixelAlignBarrelRotZ"] = True
## alignment parameters Endcap
extraOptions["pixelAlignEndcapX"] = True
extraOptions["pixelAlignEndcapY"] = True
extraOptions["pixelAlignEndcapZ"] = False
extraOptions["pixelAlignEndcapRotX"] = False
extraOptions["pixelAlignEndcapRotY"] = False
extraOptions["pixelAlignEndcapRotZ"] = True

## SCT
extraOptions["sctAlignBarrelX"] = True
extraOptions["sctAlignBarrelY"] = True
extraOptions["sctAlignBarrelZ"] = False
extraOptions["sctAlignBarrelRotX"] = True
extraOptions["sctAlignBarrelRotY"] = True
extraOptions["sctAlignBarrelRotZ"] = True
## alignment parameters Endcap
extraOptions["sctAlignEndcapX"] = True
extraOptions["sctlAlignEndcapY"] = True
extraOptions["sctAlignEndcapZ"] = False
extraOptions["sctAlignEndcapRotX"] = False
extraOptions["sctAlignEndcapRotY"] = False
extraOptions["sctAlignEndcapRotZ"] = True
             
# do the solving? (Set to False only to do quick reconstruction tests)
doSolve = True

# Setup of alignment geometry
# =============================
##########
# Pixel
# are we running Pixel alignment (for barrel and endcaps)
AlignPixel                 = [True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True]
AlignPixelBarrel           = [True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True]
AlignPixelEndcaps          = [True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True]

# alignment level for the full Pixel
PixelAlignmentLevel        = [  11, 11, 11, 11, 11, 11, 11,  2,  2,  2,  2,  2,  2, 11, 11, 2, 2, 2, 2] # see InDetAlignGeometryLevel wiki for more info
# alignment level for the Pixel barrel. If set to -1 uses PixelAlignmentLevel, otherwise it overwrites it
PixelAlignmentLevelBarrel  = [  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, 2, 2, 2] # see InDetAlignGeometryLevel wiki for more info
# alignment level for the Pixel endcaps. If set to -1 uses PixelAlignmentLevel, otherwise it overwrites it
PixelAlignmentLevelEndcaps = [  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, 2, 2, 2] # see InDetAlignGeometryLevel wiki for more info

##########
# SCT
# are we running SCT alignment (for barrel and endcaps)
AlignSCT                  =  [ True, True, True, True, True, True, True, True, True, True, True, True, True,True,True,True,True,True,True,True,True,True,True]
AlignSCTBarrel            =  [ True, True, True, True, True, True, True, True, True, True, True, True, True,True,True,True,True,True,True,True,True,True,True]
AlignSCTEndcaps           =  [False,False,False,False,False,False, True, True, True, True, True, True, True,True,True,True,True,True,True,True,True,True,True]

# alignment level for the full SCT
SCTAlignmentLevel         =  [    1,    1,    1,    1,    1,    1,    1,  2,  2,  2,  2,  2,  2,  2, 2, 2, 2, 2, 2] # see InDetAlignGeometryLevel wiki for more info
# alignment level for the SCT barrel. If set to -1 uses SCTAlignmentLevel, otherwise it overwrites it
SCTAlignmentLevelBarrel   =  [   -1,   -1,   -1,   -1,   -1,   -1,   -1, -1, -1, -1, -1, -1, -1, -1, -1, 2, 2, 2, 2] # see InDetAlignGeometryLevel wiki for more info
# alignment level for the SCT endcaps. If set to -1 uses SCTAlignmentLevel, otherwise it overwrites it
SCTAlignmentLevelEndcaps  =  [   -1,   -1,   -1,   -1,   -1,   -1,   -1,  1,  1,  1,  1,  1,  1,  1, 1, 1, 2, 2, 2] # see InDetAlignGeometryLevel wiki for more info

##########
# TRT
# are we running TRT alignment
AlignTRT            = [False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,True,True,True,True,True,True,True,True]
# alignment level for the full TRT
TRTAlignmentLevel   = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2] # see InDetAlignGeometryLevel wiki for more info

#AlignInDet          = [False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False]

########
#### To setup additional detailed settings for different alignment levels
#### use NewInDetAlignLevels.py
########

# Error Scaling
# =============================
ErrorScaling        = [True,True,True,True,True,True,True,True,True,True,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False]
#errorScalingTag = "IndetTrkErrorScaling_nominal"
#errorScalingTag = "IndetTrkErrorScaling_cscalign00_00"
#errorScalingTag = "IndetTrkErrorScaling_Day1-00"
errorScalingTag = inputErrorScalingTag


# Alignment Montioring Script to be run, if empty, no monitoring script will be run
# A dedicated monitoring script from InDetAlignmentMonitoring package can be used (with full path)
# Only if extraOptions["doMonitoring"]=True
MonitoringScript = ""

os.system("get_files -jo InDetAlignExample/NewInDetIterator_Rel19.py >/dev/null")
print os.system("get_files -jo InDetAlignExample/NewInDetIterator_Rel19.py")
execfile("NewInDetIterator_Rel19.py")
