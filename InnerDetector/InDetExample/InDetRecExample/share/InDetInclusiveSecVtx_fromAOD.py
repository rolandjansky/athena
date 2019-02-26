#--------------------------------------------------------------
# control input
#--------------------------------------------------------------
# --- specify input type
if not 'readAOD' in dir():
  readAOD = True
readESD = not readAOD

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
  
if readESD:
  athenaCommonFlags.FilesInput = [ "ESD.pool.root" ]

elif readAOD:
  athenaCommonFlags.FilesInput = [ "/afs/cern.ch/work/m/myexley/public/mc15_13TeV.402716.PythiaRhad_AUET2BCTEQ6L1_gen_gluino_p1_1200_qq_1070_1ns.merge.AOD.e4732_s2821_s2183_r8788_r8535/AOD.09897076._000001.pool.root.1" ]

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

theApp.EvtMax  = 1000
#theApp.EvtMax  = -1

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
reDoPrimaryVertexing = False
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
  print "You have to turn on reading of ESD or AOD! That's the purpose of this jobO!"
if readESD and readAOD:
  print "I can either read ESD or AOD but not both at the same time! Turn one or the other off!"
if readESD and reDoPrimaryVertexing and not reDoParticleCreation:
  print "INFO! You are running on ESD, redoing the vertexing but not recreating the TrackParticles!"
  print "INFO! To avoid inconsistencies do not use the old track particles in conjunction with the new vertex!"
  if doWriteESD or doWriteAOD:
    print "INFO! To avoid inconsistencies the old track particle (truth) container will not be in the new ESD/AOD!"
if readAOD and reDoPrimaryVertexing:
  print "INFO! You are running on AOD, and redoing the vertexing. At the moment new track particles cannot be made from old ones."
  print "INFO! To avoid inconsistencies do not use the old track particles in conjunction with the new vertex!"
  if doWriteAOD:
    print "INFO! To avoid inconsistencies the old track particle (truth) container will not be in the new AOD!"
if doRefitTracks and (reDoPrimaryVertexing or reDoParticleCreation):
  print "INFO! You are refitting tracks and also revertex and/or recreate track particles"
  print "INFO! The input for that will be the refitted tracks!"

#--------------------------------------------------------------
# Additional Detector setup
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
rec.doTrigger=False
rec.Commissioning=False
rec.doCalo=False

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# ---- switch parts of ID off/on as follows (always use both lines)
#DetFlags.pixel_setOff()
#DetFlags.detdescr.pixel_setOn()
#DetFlags.SCT_setOff()
#DetFlags.detdescr.SCT_setOn()
#DetFlags.TRT_setOff()
#DetFlags.detdescr.TRT_setOn()

# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load Reconstruction configuration for tools only
#--------------------------------------------------------------
#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------
import MagFieldServices.SetupField

from AthenaCommon.GlobalFlags import globalflags

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doPrintConfigurables = True
InDetFlags.doVertexFinding    = False
InDetFlags.postProcessing     = True
InDetFlags.doTruth            = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")
InDetFlags.preProcessing      = False
InDetFlags.doPRDFormation        = False                       # those two will be (later) automatically false if
InDetFlags.doSpacePointFormation = False  # preProcessing is false
InDetFlags.doNewTracking      = False
InDetFlags.doiPatRec          = False
InDetFlags.doxKalman          = False
InDetFlags.doLowPt            = False
InDetFlags.doBackTracking     = False
InDetFlags.doTRTStandalone    = False
InDetFlags.doTrtSegments      = False
InDetFlags.doTrackSegmentsPixel = False
InDetFlags.doTrackSegmentsSCT = False
InDetFlags.doTrackSegmentsTRT = False
InDetFlags.doSlimming         = False
InDetFlags.loadTools          = True
InDetFlags.doParticleCreation = False
InDetFlags.doConversions      = False
InDetFlags.doSecVertexFinder  = False
InDetFlags.doV0Finder         = False
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
#if InDetFlags.doVertexFinding():
#  InDetKeys.xAODVertexContainer = "RefitPrimaryVertices" 

if readESD and not redoPatternRecoAndTracking:
  InDetKeys.UnslimmedTracks              = 'Tracks'
  InDetKeys.UnslimmedTracksTruth         = 'TrackTruthCollection'

# Set container names
if doWriteESD:
  InDetKeys.OutputESDFileName = "InDetRecESD_new.root"

if doWriteAOD:
  InDetKeys.OutputAODFileName = "output.root"  
#  InDetKeys.OutputAODFileName = "/tmp/shanly/InDetRecAOD_new.root"  

print "Printing InDetKeys"
InDetKeys.lockAllExceptAlias()
InDetKeys.print_JobProperties()

#--------------------------------------------------------------
# enable statistics for reading ESD testing
#--------------------------------------------------------------

InDetFlags.doStatistics   = doInDetRecStatistics
TrackCollectionKeys        = [InDetKeys.Tracks()]
TrackCollectionTruthKeys   = [InDetKeys.TracksTruth()]

# Uncomment to use medical image seeding
# InDetFlags.primaryVertexSetup = "MedImgMultiFinding"
  
#--------------------------------------------------------------
# load master joboptions file  InclusiveSecVtx 
#--------------------------------------------------------------
  
include("InDetRecExample/InDetRec_all.py")
include("../athena/InnerDetector/InDetExample/InDetRecExample/share/InDetInclusiveSecVtx.py")
#include("InDetInclusiveSecVtx.py")

StreamAOD.ItemList+=['xAOD::VertexContainer#' + 'InclusiveSecVtx']
StreamAOD.ItemList+=['xAOD::VertexAuxContainer#' + 'InclusiveSecVtx' + 'Aux.-vxTrackAtVertex.-nrobbed']

# Set to True if you want to write out all input data ("pass-through" mode)
if doWriteESD:
  StreamESD.TakeItemsFromInput = False

if doWriteAOD:
  StreamAOD.TakeItemsFromInput = True
