#!/afs/cern.ch/sw/lcg/external/Python/2.5.4/slc4_ia32_gcc34/bin/python
# =====================================================================
# Setup L1 alignment
# Main script to run the NewInDetIterator
# run with ./RunIterator.py
# =====================================================================
import os
import sys


# ===============================
# Number of Iterations
# ===============================
FirstIteration = 0 
Iterations     = 4 

# ===============================
# Basic running options
# ===============================

# Run mode: local or batch
runMode = 'batch'
#runMode = 'local'
# Batch queue
#QUEUE = '1nh'
QUEUE 	       = 'atlasb1'

# Prefix to identify your jobs
preName        = 'L1Alignment'

HOME 	       = os.environ['HOME']
# Alignment Output
OutputPath     = "/afs/cern.ch/user/s/spedraza/work/alignment/repro2010/test/lorentzA/endcapL2"

CMTDIR 	       = HOME+"/athena/cmthome/"
# extra AtlasSetup options 
ASetupOptions  = "single"  

# OutputLevel (DEBUG, INFO, VERBOSE)
OutputLevel    = 'DEBUG'


##########################################################
#		Datasets to use
##########################################################

from InDetAlignExample.NewInDet_SetupData import setupData
DataToRun = []


Data1 = setupData('2010Test') # Name for the dataset, the output subpaths will have this name
#Data1.setDataType("MC10_Singlemuons") # Type of data: IDTracks, MinBias, IDCosmics or MC09_Multimuons by now
Data1.setDataType("IDTracks") # Type of data: IDTracks, MinBias, IDCosmics or MC09_Multimuons by now
Data1.setRuns([159224]) # Run number, you can provide a list of runs
Data1.setCPUs([300,250,250,300,150,150]) # Number of CPUs per iteration to process this data
Data1.setEvents([-1,-1,-1,-1,3000000,3000000]) # Number of total events to process
#Data1.setLimitFiles(20) # Limit the number of files to use (useful when runing in local a subset of data)
Data1.CreateFileList() # Creates the file list
## Data1.setCustomFileList("filelist") # Use your own filelist, WARNING: comment the CreateFileList line
Data1.setGlobalTag("COMCOND-BLKPA-006-10") # Use custom Global Tag, if not given it will use the data default one
Data1.setDetDescrVersion("ATLAS-GEO-20-19-01") # Use custom DetDescrVersion Tag, if not given it will use the data default one
DataToRun.append(Data1) # always add this line to process the defined data
### Duplicate the previous lines to have as many datasets that you want

from InDetAlignExample.NewInDet_IteratorClasses import HandleRunOptions
if len(sys.argv) > 1 or 'rtt' in os.environ["USER"]:
	DataToRun = [HandleRunOptions()]
	
for data in DataToRun:
	data.Print()

# Alignment Options
# =============================
# Global chi^2 alignment
# Alignment method: GX2, LX2
AlignmentMethod = "GX2"

# Alignment poolFile to be read on the first iteration, if empty, constants from database will be read
inputAlignmentPoolFile = ""

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

extraOptions["doMonitoring"] = True
extraOptions["doTrkNtuple"] = False

extraOptions["PtCut"]               = 5000 #Pt in MeV, comment the line to use default value (10000)
extraOptions["lorentzAngleTag"]     ="PIXELLorentzAngleScale-Data-001"
extraOptions["siAlignmentTag"]      = "InDetAlign_Repro2012_d0z0p_constrained"
extraOptions["trtAlignmentTag"]     = "TRTAlign_Repro2012_d0z0p_constrained"
extraOptions["trtAlignmentTagL3"]   = "TRTCalibDX_EoverP_189598_189845v1"

#extraOptions["beamSpotTag"] = "IndetBeampos-ES1-UPD2-04"
#extraOptions["particleNumber"] = 0
extraOptions["ModCut"] = 0
extraOptions["softModeCut"]= 0.

extraOptions["doBSConstraint"] = False 
extraOptions["doPVConstraint"] = False
extraOptions["useOldPreProcessor"] = False
#extraOptions["eoverpmapconstraint"] = ""#"/afs/cern.ch/work/s/spedraza/alignment/D/L1/maps/Data.L1-Iter0-map.Refit1Params.root"  # Full path of the map
#extraOptions["z0mapconstraint"] = "/afs/cern.ch/work/s/spedraza/alignment/D/L1/maps/Data.L1-Iter0-map.Refit1Params.root"
#extraOptions["CosmicGRL"] = "CosmicsGRL.xml"
#extraOptions["CollisionGRL"] = "ColisionGRL.xml"
extraOptions["useTRT"] = True
#extraOptions["readTRT"] = False
#extraOptions["writeTRT"] = False
#extraOptions["TRTCalibTextFile"] = ""
#extraOptions["TRTCalibT0TagCos"] = "TrtCalibT0-AlignmentJune2010-00"
#extraOptions["TRTCalibRtTagCos"] = "TrtCalibRt-AlignmentJune2010-00"
extraOptions["trtAlignBarrelZ"] = False
extraOptions["trtAlignEndcapZ"] = False
## PIXEL
extraOptions["pixelAlignBarrelX"] = False
extraOptions["pixelAlignBarrelY"] = False
extraOptions["pixelAlignBarrelZ"] = False
extraOptions["pixelAlignBarrelRotX"] = False
extraOptions["pixelAlignBarrelRotY"] = False
extraOptions["pixelAlignBarrelRotZ"] = False
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
extraOptions["sctAlignBarrelZ"] = True
extraOptions["sctAlignBarrelRotX"] = True
extraOptions["sctAlignBarrelRotY"] = True
extraOptions["sctAlignBarrelRotZ"] = True
## alignment parameters Endcap
extraOptions["sctAlignEndcapX"] = True
extraOptions["sctlAlignEndcapY"] = True
extraOptions["sctAlignEndcapZ"] = True
extraOptions["sctAlignEndcapRotX"] = True
extraOptions["sctAlignEndcapRotY"] = True
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
PixelAlignmentLevel        = [ 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1] # see InDetAlignGeometryLevel wiki for more info
# alignment level for the Pixel barrel. If set to -1 uses PixelAlignmentLevel, otherwise it overwrites it
PixelAlignmentLevelBarrel  = [-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1] # see InDetAlignGeometryLevel wiki for more info
# alignment level for the Pixel endcaps. If set to -1 uses PixelAlignmentLevel, otherwise it overwrites it
PixelAlignmentLevelEndcaps = [-1,-1 ,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1] # see InDetAlignGeometryLevel wiki for more info

##########
# SCT
# are we running SCT alignment (for barrel and endcaps)
AlignSCT                  =  [True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True]
AlignSCTBarrel            =  [True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True]
AlignSCTEndcaps           =  [True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True]

# alignment level for the full SCT
SCTAlignmentLevel         =  [ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1] # see InDetAlignGeometryLevel wiki for more info
# alignment level for the SCT barrel. If set to -1 uses SCTAlignmentLevel, otherwise it overwrites it
SCTAlignmentLevelBarrel   =  [-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1] # see InDetAlignGeometryLevel wiki for more info
# alignment level for the SCT endcaps. If set to -1 uses SCTAlignmentLevel, otherwise it overwrites it
SCTAlignmentLevelEndcaps  =  [-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1] # see InDetAlignGeometryLevel wiki for more info

##########
# TRT
# are we running TRT alignment
AlignTRT            = [True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True,True]
# alignment level for the full TRT
TRTAlignmentLevel   = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1] # see InDetAlignGeometryLevel wiki for more info

#AlignInDet          = [False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False]

########
#### To setup additional detailed settings for different alignment levels
#### use NewInDetAlignLevels.py
########

# Error Scaling
# =============================
ErrorScaling        = [True,True,True,True,True,True,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False,False]
#errorScalingTag = "IndetTrkErrorScaling_nominal"
#errorScalingTag = "IndetTrkErrorScaling_cscalign00_00"
#errorScalingTag = "IndetTrkErrorScaling_Day1-00"
errorScalingTag = ''

# Alignment Montioring Script to be run, if empty, no monitoring script will be run
# A dedicated monitoring script from InDetAlignmentMonitoring package can be used (with full path)
# Only if extraOptions["doMonitoring"]=True
MonitoringScript = ""

os.system("get_files -jo InDetAlignExample/NewInDetIterator.py >/dev/null")
print os.system("get_files -jo InDetAlignExample/NewInDetIterator.py")
execfile("NewInDetIterator.py")
