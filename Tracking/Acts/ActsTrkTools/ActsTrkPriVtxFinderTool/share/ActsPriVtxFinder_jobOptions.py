from AthenaCommon.AppMgr import ServiceMgr
#--------------------------------------------------------------
# control input
#--------------------------------------------------------------
# --- specify input type
#if not 'readAOD' in dir():
readAOD = True
readESD = not readAOD

# Use Acts AMVF
doAMVF = True
useActs = True
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
nEvents = 30

# Set your input AOD file here
myfile = ""

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if readESD:
  athenaCommonFlags.FilesInput = [ "ESD.pool.root" ]
elif readAOD:
  athenaCommonFlags.FilesInput = [ myfile ]

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())

theApp.EvtMax  = nEvents

#--------------------------------------------------------------
# control output (here so RecExCommon via auto-config doesn't delete the global flags)
#--------------------------------------------------------------
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD = False and readESD
doWriteAOD = True

#--------------------------------------------------------------
# control algorithms to be rerun
#--------------------------------------------------------------
# --- run InDetRecStatistics (only possible if readESD = True)
doInDetRecStatistics = True and readESD
# --- refit the EXISTING tracks in ESD (only possible if readESD = True)
doRefitTracks = False and readESD
# --- redo the pattern reco and the tracking (do not use that in conjunction with doRefitTracks above)
redoPatternRecoAndTracking = False and not doRefitTracks and readESD
# --- redo primary vertexing (will be set to true later automatically if you redid the tracking and want to redo the TrackParticle creation)
reDoPrimaryVertexing = True
# --- redo particle creation (recommended after revertexing on ESD, otherwise trackparticles are inconsistent)
reDoParticleCreation = False and readESD and reDoPrimaryVertexing
# --- redo conversion finding
reDoConversions = False
# --- redo V0 finding
reDoV0Finder = False
 
#--------------------------------------------------------------
# Control - standard options (as in jobOptions.py)
#--------------------------------------------------------------
# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- do auditors ?
doAuditors      = True

import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False

# safety section ... redoing tracking/vertexing is a tricky business to stay consistent ...
if redoPatternRecoAndTracking and reDoParticleCreation:
  reDoPrimaryVertexing = True

if not (readESD or readAOD):
  print( "You have to turn on reading of ESD or AOD! That's the purpose of this jobO!")
if readESD and readAOD:
  print( "I can either read ESD or AOD but not both at the same time! Turn one or the other off!")
if readESD and reDoPrimaryVertexing and not reDoParticleCreation:
  print( "INFO! You are running on ESD, redoing the vertexing but not recreating the TrackParticles!")
  print( "INFO! To avoid inconsistencies do not use the old track particles in conjunction with the new vertex!")
  if doWriteESD or doWriteAOD:
    print( "INFO! To avoid inconsistencies the old track particle (truth) container will not be in the new ESD/AOD!")
if readAOD and reDoPrimaryVertexing:
  print( "INFO! You are running on AOD, and redoing the vertexing. At the moment new track particles cannot be made from old ones.")
  print( "INFO! To avoid inconsistencies do not use the old track particles in conjunction with the new vertex!")
  if doWriteAOD:
    print( "INFO! To avoid inconsistencies the old track particle (truth) container will not be in the new AOD!")
if doRefitTracks and (reDoPrimaryVertexing or reDoParticleCreation):
  print( "INFO! You are refitting tracks and also revertex and/or recreate track particles")
  print( "INFO! The input for that will be the refitted tracks!")

#--------------------------------------------------------------
# Additional Detector setup
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
rec.Commissioning=False

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# --- print(out)
DetFlags.Print()

#--------------------------------------------------------------
# Load Reconstruction configuration for tools only
#--------------------------------------------------------------
#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------
import MagFieldServices.SetupField

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = "ATLAS-R2-2016-00-01-00"

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth            = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")
InDetFlags.preProcessing      = redoPatternRecoAndTracking
InDetFlags.doPRDFormation        = False                       # those two will be (later) automatically false if
InDetFlags.doSpacePointFormation = redoPatternRecoAndTracking  # preProcessing is false
InDetFlags.doNewTracking      = redoPatternRecoAndTracking
#InDetFlags.doiPatRec          = False
#InDetFlags.doxKalman          = False
InDetFlags.doLowPt            = False
InDetFlags.doBackTracking     = redoPatternRecoAndTracking
InDetFlags.doTRTStandalone    = redoPatternRecoAndTracking
InDetFlags.doTrtSegments      = redoPatternRecoAndTracking
InDetFlags.postProcessing     = reDoPrimaryVertexing or reDoParticleCreation or reDoConversions or doInDetRecStatistics or reDoV0Finder
InDetFlags.doTrackSegmentsPixel = False
InDetFlags.doTrackSegmentsSCT = False
InDetFlags.doTrackSegmentsTRT = False
InDetFlags.doSlimming         = False
InDetFlags.loadTools          = True
InDetFlags.doVertexFinding    = reDoPrimaryVertexing
InDetFlags.doParticleCreation = reDoParticleCreation
InDetFlags.doConversions      = reDoConversions
InDetFlags.doSecVertexFinder  = False
InDetFlags.doV0Finder         = reDoV0Finder
InDetFlags.doSimpleV0Finder   = False
InDetFlags.doTrkNtuple        = False
InDetFlags.doPixelTrkNtuple   = False
InDetFlags.doSctTrkNtuple     = False
InDetFlags.doTrtTrkNtuple     = False
#InDetFlags.doPixelClusterNtuple = False
InDetFlags.doSctClusterNtuple   = False
#InDetFlags.doTrtDriftCircleNtuple = False
InDetFlags.doVtxNtuple        = False
InDetFlags.doConvVtxNtuple    = False
InDetFlags.doV0VtxNtuple      = False
InDetFlags.doRefit            = doRefitTracks
InDetFlags.doLowBetaFinder    = False
InDetFlags.doPrintConfigurables = True

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

from InDetRecExample.InDetKeys import InDetKeys
if InDetFlags.doVertexFinding() and readAOD:
  InDetKeys.Tracks = InDetKeys.TrackParticles()

# uncomment if you don't want to overwrite the original fits (e.g. for comparison)
# this would also require enabling "pass-through" output mode (see bottom of this file)
# or else manually adding the input collection to the output stream
if InDetFlags.doVertexFinding():
  InDetKeys.xAODVertexContainer = "RefitPrimaryVertices" 

if readESD and not redoPatternRecoAndTracking:
  InDetKeys.UnslimmedTracks              = 'Tracks'
  InDetKeys.UnslimmedTracksTruth         = 'TrackTruthCollection'

# Set container names
if doWriteESD:
  InDetKeys.OutputESDFileName = "InDetRecESD_new.root"

toolString = ""
if useActs:
  toolString = "acts"
else:
  toolString = "athena"

if doWriteAOD:
  InDetKeys.OutputAODFileName = "InDetRecAOD_{}_n{}.root".format(toolString,nEvents)  

print( "Printing InDetKeys")
InDetKeys.lockAllExceptAlias()
InDetKeys.print_JobProperties()

#--------------------------------------------------------------
# enable statistics for reading ESD testing
#--------------------------------------------------------------

InDetFlags.doStatistics   = doInDetRecStatistics
TrackCollectionKeys        = [InDetKeys.Tracks()]
TrackCollectionTruthKeys   = [InDetKeys.TracksTruth()]

# Uncomment to use variant vertex seeding and/or finding
if doAMVF:
  InDetFlags.primaryVertexSetup = "GaussAdaptiveMultiFinding"
# Use Acts primary vertexing
InDetFlags.useActsPriVertexing = useActs

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------
  
include("InDetRecExample/InDetRec_all.py")

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["InDetAdaptiveMultiPriVxFinderTool DATAFILE='AMVF_{}_output_nEvent{}.root' OPT='RECREATE'".format(toolString,nEvents)]

# Set to True if you want to write out all input data ("pass-through" mode)
if doWriteESD:
  StreamESD.TakeItemsFromInput = False

if doWriteAOD:
  StreamAOD.TakeItemsFromInput = True

from AthenaCommon.ConfigurationShelve import saveToAscii; saveToAscii("config_vertexing_{}.txt".format(toolString));
