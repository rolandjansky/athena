
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
Iterations     = 2

# ===============================
# Basic running options
# ===============================

# Run mode: local or batch
runMode = 'batch'
#runMode = 'local'
# Batch queue
QUEUE = '1nh'
#QUEUE = 'atlasb1'

# Prefix to identify your jobs
preName        = 'IDAling_'

#suffix to the folder name
folderSuffix = ""

HOME           = os.environ['HOME']
# Alignment Output
OutputPath     = os.environ['PWD']+"/."

CMTDIR         = HOME+"/athena/cmthome/"
# extra AtlasSetup options
ASetupOptions  = "single"
#ASetupOptions  = "nightlies"


# OutputLevel (DEBUG, INFO, VERBOSE)
OutputLevel    = 'INFO'

# list of extra options for NewInDetAlignAlgSetup
extraOptions = {}


##########################################################
#             Inline Options Definition                  #
##########################################################

def optParsing():
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("--ClusteringAlgo",dest="ClusteringAlgo", help="Which Clustering algorithm to use: NN (Default), Analog, Digital. NOT FULLY WORKING",default="NN")
    parser.add_option("--errorScalingTag",dest="errorScalingTag", help="Name of the error scaling tag or ES file to use",default="")
    parser.add_option("--eventType", dest="eventType", help="Which type of events: collisions / cosmics", default="collisions")
    parser.add_option("--HeavyIons", dest="HeavyIons", help="Set it to true if you want to reconstruct HeavyIons",action="store_true",default=False)
    parser.add_option("--inputList", dest="inputList", help="Input txt File with the list of the files you want to run on", default="")
    parser.add_option("--inputDir", dest="inputDir", help="Directory containing the input files - NOT YET IMPLEMENTED", default="")
    parser.add_option("--inputBowingDb", dest="inputBowingDb", help="In the case you want to use a specific IBL bowing  write here the absolute path to the cool file", default="")
    parser.add_option("--inputConstantsFile", dest="inputConstantsFile", help="In the case you want to use a specific set of initial constants write here the absolute path to the pool file", default="")
    parser.add_option("--isData",dest="isData", help="Set itif you want to run on real data (Default: True)",action="store_true", default=True)
    parser.add_option("--LBrangeFirst", dest="LBrangeFirst", help=" Setting this the LumiBlock selection is set to true and one sets the lower LB to be accepted", default=0)
    parser.add_option("--LBrangeLast", dest="LBrangeLast", help=" Setting this the LumiBlock selection is set to true and one sets the upper LB to be accepted", default=4096)
    parser.add_option("--nCpus",dest="nCpus", help="Number of CPUs to be used", default = "1")
    parser.add_option("--nEvents",dest="nEvents", help="Maximum number of events to use", default = "-1")
    parser.add_option("--nIter",dest="nIter", help="Number of iterations to be executed", default = "")
    parser.add_option("--prefix", dest="prefix", help="prefix to the jobs name", default="")
    parser.add_option("--preIBLgeometry",dest="preIBLgeometry", help="Set it when you want to run on Run1 geometry (Default: False)",action="store_true",default=False)
    parser.add_option("--ptmin",dest="userPtMin", help="Minimum pt of tracks to enter the align track selection *** in MeV ***", default = "2000")
    parser.add_option("--queue",dest="userQueue", help="name of the LSF queue were jobs will be submitted", default="")
    parser.add_option("--suffix", dest="suffix", help="suffix to the IterN folder name --> IterN_suffix", default="")
    
    
    (config, sys.argv[1:]) = parser.parse_args(sys.argv[1:])

    return config
    

##########################################################
#               Option Initialisation                    #
##########################################################
config = optParsing()
inputList=config.inputList
inputDir =config.inputDir
eventType=config.eventType
HeavyIons=config.HeavyIons
ConstantsFile=config.inputConstantsFile
BowingDb=config.inputBowingDb
nEventsMax=config.nEvents
preIBLgeometry=config.preIBLgeometry
isData=config.isData
ClusteringAlgo=config.ClusteringAlgo
inputErrorScalingTag=config.errorScalingTag
nCpus=config.nCpus
userPtMin = config.userPtMin
userPrefix = config.prefix
userSuffix = config.suffix
userLBselection = (config.LBrangeFirst > 0) or (config.LBrangeLast < 4096)
userLBrangeFirst = int(config.LBrangeFirst)
userLBrangeLast = int(config.LBrangeLast)
userQueue = config.userQueue
userNIter = config.nIter


# split the possible collections
inputList = inputList.split()
eventType = eventType.split()
nCpus = nCpus.split()
nEventsMax = nEventsMax.split()
userPtMin = userPtMin.split()

# check that inputList and eventype have the same number of elements
if (len(inputList) != len(eventType)):
    print " <RunIterator> ERROR no input list. Please provide a set of files with data to be processed"
    exit()

#prefix for the job's name
if (len(userPrefix) > 0):
    preName = userPrefix

if (len(userSuffix) > 0):
    folderSuffix = "_"+userSuffix

# LSF queue
if (len(userQueue)>0):
    QUEUE = userQueue

# Number of iterations
if (len(userNIter)>0):
    Iterations     = int(userNIter)

##########################################################
#               Print input parameters                   #
##########################################################

print " \n <RunIterator> ----- User input in command line --- "
if (len(folderSuffix)>0): print " <RunIterator> folder suffix = ", folderSuffix
print " <RunIterator> inputList = ",inputList
print " <RunIterator> nCpus = ",nCpus
print " <RunIterator> nEventsMax = ",nEventsMax
if (len(ConstantsFile)>0): print " <RunIterator> ConstantsFile = ",ConstantsFile
if (len(BowingDb)>0): print " <RunIterator> BowingDb = ", BowingDb
if (len(inputErrorScalingTag)>0): print " <RunIterator> inputErrorScalingTag = ",inputErrorScalingTag
if (len(userPtMin)>0): print " <RunIterator> userPtMin = ",userPtMin, " MeV"
print " <RunIterator> eventType = ",eventType
print " <RunIterator> iterating on ",len(eventType),"samples "
if (len(userNIter)>0): print " <RunIterator> #Iterations = ",Iterations
if userLBselection:
    print " <RunIterator> LB range = ",userLBrangeFirst, " --> ", userLBrangeLast

##########################################################
#       Datasets to use
##########################################################

from InDetAlignExample.NewInDet_SetupData import setupData
DataToRun = []

for i_sample in range(len(inputList)):
    print " "
    print " ==== configuring sample #",i_sample
    Data1 = setupData(eventType[i_sample])
    #Need To be changed this one
    myDataType = "Data_15" + eventType[i_sample]
    #Data1.setDataType("MinBias_Data_15") # Type of data: IDTracks, MinBias, IDCosmics or MC09_Multimuons by now
    Data1.setDataType(myDataType) # Type of data: IDTracks, MinBias, IDCosmics or MC09_Multimuons by now
    
    # number of CPUS to be used in each iteration to process this data type
    thisNCpus = int(nCpus[0]) 
    if (len(nCpus) >= i_sample+1):
        thisNCpus = int(nCpus[i_sample])
    Data1.setCPUs([thisNCpus,thisNCpus,thisNCpus,thisNCpus,thisNCpus,thisNCpus,thisNCpus,thisNCpus,thisNCpus,thisNCpus])
    
    # number of events to be processed 
    thisNEvents = int(nEventsMax[0])
    if (len(nEventsMax) >= i_sample+1):
         thisNEvents = int(nEventsMax[i_sample])
    Data1.setEvents([thisNEvents,thisNEvents, thisNEvents, thisNEvents, thisNEvents, thisNEvents, thisNEvents, thisNEvents, thisNEvents, thisNEvents])
    Data1.setLimitFiles(5000) # Limit the number of files to use (useful when runing in local a subset of data)
    #Data1.setRuns([201280]) # Run number, you can provide a list of runs
    #Data1.CreateFileList() # Creates the file list
    
    # figure out the mimimum pt
    thisPtMin = userPtMin[0]
    if (len(userPtMin) >= i_sample+1):
         thisPtMin = userPtMin[i_sample]

    #     
    if isData:
        Data1.setByteStream(True)
    else:
        Data1.setByteStream(False)
        
    Data1.setCustomFileList(inputList[i_sample])

    # Use custom Global Tag, if not given it will use the data default one (that is empty ;) )
    if (eventType[i_sample] == "collisions"):
        # default
        myTag = "ATLAS-R2-2015-03-01-00"
        print " ==> default tag   myTag=",myTag            
        Data1.setGlobalTag("OFLCOND-RUN12-SDR-28")
        Data1.setDetDescrVersion(myTag)
        if isData:
            print " ==> In collisions and isData True :)   myTag=",myTag
            myTag = "ATLAS-R2-2015-03-01-00"
            Data1.setGlobalTag("CONDBR2-BLKPA-2015-14")
            Data1.setDetDescrVersion(myTag)
            if HeavyIons:
                myTag = "ATLAS-R2-2015-03-01-00"
                print " ==> In collisions and HeavyIons :)   myTag=",myTag
                Data1.setGlobalTag("COMCOND-BLKPA-RUN1-06")
                Data1.setDetDescrVersion(myTag)
            if preIBLgeometry and HeavyIons:
                print " ==> In preIBLgeometry and HeavyIons :) "
                Data1.setDetDescrVersion("ATLAS-R1-2011-02-00-00")

    
    if (eventType[i_sample] == "cosmics"):
        if isData:
            print " ==> In cosmics and isData True :) "
            Data1.setGlobalTag("CONDBR2-BLKPA-2015-14")
            Data1.setDetDescrVersion("ATLAS-R2-2015-03-01-00")
        else:
            print " ==> In cosmics and else :) "
            Data1.setGlobalTag("OFLCOND-RUN12-SDR-14")
            Data1.setDetDescrVersion("ATLAS-R2-2015-03-01-00")

    extraOptions["Cosmics"] = False # default is False
    
    Data1.setCosmic(False)
    Data1.setDigitalClustering(False)
    Data1.setPtMin(thisPtMin)
    if "cosmics" in eventType[i_sample] or "Cosmics" in eventType[i_sample]:
        Data1.setProjectName("data15_cos")
        Data1.setCosmic(True)
        Data1.setDigitalClustering(True)
    if HeavyIons:
        Data1.setProjectName("data11_hi")
        

    DataToRun.append(Data1) # always add this line to process the defined data
    print " == configuration sample ",i_sample," completed"
# end of loop on data types

### Duplicate the previous lines to have as many datasets that you want

    
for data in DataToRun:
    data.Print()

# Lumiblock selection
if userLBselection:
    extraOptions["LumiblockSelection"] = True
    extraOptions["LumiblockList"]      = range(userLBrangeFirst,userLBrangeLast+1,1)
    
# Alignment Options
# =============================
# Global chi^2 alignment
# Alignment method: GX2, LX2
AlignmentMethod = "GX2"

# Alignment poolFile to be read on the first iteration, if empty, constants from database will be read
#inputAlignmentPoolFile = "/afs/cern.ch/user/m/mdanning/AlignmentTestArea/IBL_4br_30Jan/run_misalign/MisalignmentSet1.pool.root"
inputAlignmentPoolFile = ""
inputBowingCoolFile    = ""
readConstantsFromPool = False
readBowingFromCool    = False

if ConstantsFile != "":
    inputAlignmentPoolFile=ConstantsFile
    readConstantsFromPool = True

if BowingDb != "":
    inputBowingCoolFile = BowingDb
    readBowingFromCool  = True





# split files to subjobs to have exactly equal number of events (True)
# or just approximately based on the size of the input files (False)
doDetailedSplitting = False

# Option to use TFiles instead bin files
# if False it will use the standard bin files
useTFiles = True


# These options will be written to the jobOptions
# have a look into NewInDetAlignAlgSetup.py and NewInDetAlignGeometrySetup.py
# for all supported options

extraOptions["realData"] = isData
if isData: 
   extraOptions["dataSource"]  = 'data' 
   #extraOptions["projectName"] = 'data15_cos' 
   #extraOptions["projectName"] = 'data15_comm' 
   extraOptions["doReadBS"]=True
else: 
   extraOptions["dataSource"]  = 'geant4'  
   #extraOptions["projectName"] = 'MC_Cosmic_BOFF' 
   extraOptions["doReadBS"]=False

extraOptions["doMonitoring"] = True
#extraOptions["doTrkNtuple"] = False
extraOptions["BField"]  = True
extraOptions["BField_AlgSetup"] = extraOptions["BField"]  


#Heavy Ions reconstruction
extraOptions["HeavyIons"] = HeavyIons



#
#extraOptions["siAlignmentTag"] = "InDetAlign_R2_Nominal"
#extraOptions["siAlignmentTag"] = "InDetAlign_RUN1-L3IBLm15-CORRECT"
#extraOptions["siAlignmentTag"] = "InDetAlign_R2_Initial_fixed"
#extraOptions["trtAlignmentTag"] = "TRTAlign_nominal"

#25 NS
extraOptions["siAlignmentTag"] = "InDetAlign-RUN2-25NS"
extraOptions["trtAlignmentTag"] = "TRTAlign-RUN2-25NS"

#extraOptions["beamSpotTag"] = "IndetBeampos-ES1-UPD2"
#extraOptions["particleNumber"] = 0
extraOptions["ModCut"] = 0
extraOptions["softModeCut"]= 0.

#--- Pixel and IBL alignment
#extraOptions["AlignIBLbutNotPixel"]= True
#extraOptions["AlignPixelbutNotIBL"]= True

#extraOptions["LumiblockSelection"] = True
#extraOptions["LumiblockList"]      = range(100,400,1)

#
#---- IBL stave bowing alignment
extraOptions["applyLBibldistTweak"] = False
extraOptions["writeIBLDistDB"] = True


#extraOptions["doBSConstraint"] = False
#extraOptions["doPVConstraint"] = False

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

## TRT dofs
extraOptions["trtAlignBarrelX"] = True
extraOptions["trtAlignBarrelY"] = True
extraOptions["trtAlignBarrelZ"] = False
extraOptions["trtAlignBarrelRotX"] = True
extraOptions["trtAlignBarrelRotY"] = True
extraOptions["trtAlignBarrelRotZ"] = True
# end-caps
extraOptions["trtAlignEndcapX"] = True
extraOptions["trtAlignEndcapY"] = True
extraOptions["trtAlignEndcapZ"] = False
extraOptions["trtAlignEndcapRotX"] = False
extraOptions["trtAlignEndcapRotY"] = False
extraOptions["trtAlignEndcapRotZ"] = True

## PIXEL
extraOptions["pixelAlignBarrelX"] = True
extraOptions["pixelAlignBarrelY"] = True
extraOptions["pixelAlignBarrelZ"] = True
extraOptions["pixelAlignBarrelRotX"] = True
extraOptions["pixelAlignBarrelRotY"] = True
extraOptions["pixelAlignBarrelRotZ"] = True
extraOptions["pixelAlignBarrelBowX"] = True

## end-caps
extraOptions["pixelAlignEndcapX"] = True
extraOptions["pixelAlignEndcapY"] = True
extraOptions["pixelAlignEndcapZ"] = False
extraOptions["pixelAlignEndcapRotX"] = False
extraOptions["pixelAlignEndcapRotY"] = False
extraOptions["pixelAlignEndcapRotZ"] = True

## SCT
extraOptions["sctAlignBarrelX"] = False
extraOptions["sctAlignBarrelY"] = False
extraOptions["sctAlignBarrelZ"] = False
extraOptions["sctAlignBarrelRotX"] = False
extraOptions["sctAlignBarrelRotY"] = False
extraOptions["sctAlignBarrelRotZ"] = False
## end-caps
extraOptions["sctAlignEndcapX"] = True
extraOptions["sctAlignEndcapY"] = True
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
AlignPixel                 = [ True, True, True, True, True, True, True, True, True, True]
AlignPixelBarrel           = [ True, True, True, True, True, True, True, True, True, True]
AlignPixelEndcaps          = [ True, True, True, True, True, True, True, True, True, True]
# alignment level for the full Pixel # see InDetAlignGeometryLevel wiki for more info
PixelAlignmentLevel        = [   11,   11,   11,   11,   11,   11,   11,   11,   11,   11]
# alignment level for the Pixel barrel. If set to -1 uses PixelAlignmentLevel, otherwise it overwrites it
PixelAlignmentLevelBarrel  = [   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1]
# alignment level for the Pixel endcaps. If set to -1 uses PixelAlignmentLevel, otherwise it overwrites it
PixelAlignmentLevelEndcaps = [   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1]

##########
# SCT
# are we running SCT alignment (for barrel and endcaps)
AlignSCT                  =  [ True, True, True, True, True, True, True, True, True, True]
AlignSCTBarrel            =  [ True, True, True, True, True, True, True, True, True, True]
AlignSCTEndcaps           =  [ True, True, True, True, True, True, True, True, True, True]

# alignment level for the full SCT # see InDetAlignGeometryLevel wiki for more info
SCTAlignmentLevel         =  [    1,    1,    1,    1,    1,    1,    1,    1,    1,    1] 
# alignment level for the SCT barrel. If set to -1 uses SCTAlignmentLevel, otherwise it overwrites it
SCTAlignmentLevelBarrel   =  [   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1] 
# alignment level for the SCT endcaps. If set to -1 uses SCTAlignmentLevel, otherwise it overwrites it
SCTAlignmentLevelEndcaps  =  [    1,    1,    1,    1,    1,    1,    1,    1,    1,    1] 

##########
# TRT
# are we running TRT alignment # see InDetAlignGeometryLevel wiki for more info
AlignTRT                  =  [ True, True, True, True, True, True, True, True, True, True]
AlignTRTBarrel            =  [ True, True, True, True, True, True, True, True, True, True]
AlignTRTEndcaps           =  [ True, True, True, True, True, True, True, True, True, True]
# alignment level for the TRT parts
TRTAlignmentLevel         =  [    1,    1,    1,    1,    1,    1,    1,    1,    1,    1]
TRTAlignmentLevelBarrel   =  [   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1]
TRTAlignmentLevelEndcaps  =  [    1,    1,    1,    1,    1,    1,    1,    1,    1,    1]
##########
# XXXX                  = [False,False,False,False,False,False,False,False,False,False]


########
#### To setup additional detailed settings for different alignment levels
#### use NewInDetAlignLevels.py
########

# Error Scaling
# =============================
ErrorScaling        = [ True, True, True, True, True, True, True, True, True, True]
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


