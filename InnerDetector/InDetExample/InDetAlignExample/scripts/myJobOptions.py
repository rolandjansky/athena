# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


###############################################################
#
#  Custom configuration jobOptions file
#
###############################################################

inputFiles=['/castor/cern.ch/grid/atlas/atlasscratchdisk/data10_7TeV/RAW/other/data10_7TeV.00165591.calibration_IDTracks.daq.RAW/data10_7TeV.00165591.calibration_IDTracks.daq.RAW._lb0000._SFO-10._0001.data', '/castor/cern.ch/grid/atlas/atlasscratchdisk/data10_7TeV/RAW/other/data10_7TeV.00165591.calibration_IDTracks.daq.RAW/data10_7TeV.00165591.calibration_IDTracks.daq.RAW._lb0000._SFO-10._0002.data']
inputTFiles = ['/tmp/lacuesta/matrixTFile1.root', '/tmp/lacuesta/matrixTFile2.root']
inputalignmentConstants = ""
outputPoolFile = "outputConstants.root"

PathBinName = ""


##-------- Alignment Configuration --------------------
WriteTFile = True
sctAlignmentLevelEndcaps	=	-1
pixelAlignmentLevel	=	1
sctAlignmentLevel	=	1
pixelAlignmentLevelBarrel	=	-1
pixelAlignmentLevelEndcaps	=	-1
sctAlignmentLevelBarrel	=	-1
alignSCT	=	True
if len(inputalignmentConstants)!=0:
	readConstantsFromPool	=	True
	inputPoolFiles	=	[inputalignmentConstants]
else:
	readConstantsFromPool	=	False
	
solveLocal	=	False
sctAlignBarrel	=	True
runAccumulate	=	False
runLocal	=	False
runSolving	=	True
pixelAlignEndcaps	=	True
alignPixel	=	True
trtAlignmentLevel	=	1
alignTRT	=	True
#inputPoolFiles	=	['']
readConstantsFromPool	=	False
pixelAlignBarrel	=	True
sctAlignEndcaps	=	True


##-------- Loading the Alignment Levels --------------------
include("InDetAlignExample/NewInDetAlignLevels.py") 

##-------- Reconstruction Configuration --------------------
numberOfEvents	=	1
inputFiles	=	inputFiles
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

##-------- Load Alignment --------------------
include("InDetAlignExample/NewInDetAlignAlgSetup.py") 

###############################################################
  