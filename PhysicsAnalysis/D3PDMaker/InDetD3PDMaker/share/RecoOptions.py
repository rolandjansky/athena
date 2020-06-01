
import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False

import AthenaCommon.SystemOfUnits as Units

dotruth = False
dorereco = False
dowritesd = False

if 'DoReconstruction' in dir():
  dorereco = DoReconstruction
if 'DoTruth' in dir():
  dotruth = DoTruth

# --- specify input type
if not 'readESD' in dir():
  readESD= True
readAOD = not readESD
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD = dowritesd and readESD
doWriteAOD = False

#--------------------------------------------------------------
# control algorithms to be rerun
#--------------------------------------------------------------
# --- run InDetRecStatistics (only possible if readESD = True)
doInDetRecStatistics = False and readESD
# --- refit the EXISTING tracks in ESD (only possible if readESD = True)
doRefitTracks = False and readESD
# --- redo the pattern reco and the tracking (do not use that in conjunction with doRefitTracks above)
redoPatternRecoAndTracking = dorereco and not doRefitTracks and readESD
# --- redo primary vertexing (will be set to true later automatically if you redid the tracking and want to redo the TrackParticle creation)
reDoPrimaryVertexing = True and dorereco
# --- redo particle creation (recommended after revertexing on ESD, otherwise trackparticles are inconsistent)
#reDoParticleCreation = True and readESD and reDoPrimaryVertexing and dorereco
reDoParticleCreation = redoPatternRecoAndTracking
# --- redo conversion finding
reDoConversions = redoPatternRecoAndTracking
# --- redo V0 finding
reDoV0Finder = redoPatternRecoAndTracking

## Uncomment to do vertexing without rerunning tracking
#reDoPrimaryVertexing = True
#------------------------------------------------------

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
# Set Detector setup
#--------------------------------------------------------------
from RecExConfig.RecFlags import rec
rec.Commissioning=True
rec.doMonitoring=False

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Print()

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth            = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool") and dotruth
InDetFlags.preProcessing      = redoPatternRecoAndTracking
InDetFlags.doPRDFormation        = redoPatternRecoAndTracking  # those two will be (later) automatically false if
InDetFlags.doSpacePointFormation = redoPatternRecoAndTracking  # preProcessing is false
InDetFlags.doNewTracking      = redoPatternRecoAndTracking
InDetFlags.doLowPt            = True
InDetFlags.doCTBTracking      = False
InDetFlags.doBackTracking     = redoPatternRecoAndTracking
InDetFlags.doTRTStandalone    = redoPatternRecoAndTracking
InDetFlags.doTrtSegments      = redoPatternRecoAndTracking
#ao do run separate standalone tracklets
InDetFlags.doTrackSegmentsPixel = redoPatternRecoAndTracking
InDetFlags.doTrackSegmentsSCT   = redoPatternRecoAndTracking
InDetFlags.doTrackSegmentsTRT   = redoPatternRecoAndTracking
#
InDetFlags.postProcessing     = reDoPrimaryVertexing or reDoParticleCreation or reDoConversions or doInDetRecStatistics or reDoV0Finder
InDetFlags.doSlimming         = False
InDetFlags.loadTools          = True
InDetFlags.doVertexFinding    = reDoPrimaryVertexing
InDetFlags.doParticleCreation = reDoParticleCreation
InDetFlags.doConversions      = reDoConversions
InDetFlags.doSecVertexFinder  = False
InDetFlags.doV0Finder         = reDoV0Finder
InDetFlags.doTrkNtuple        = False
InDetFlags.doPixelTrkNtuple   = False
InDetFlags.doSctTrkNtuple     = False
InDetFlags.doTrtTrkNtuple     = False
InDetFlags.doSctClusterNtuple   = False
InDetFlags.doVtxNtuple        = False
InDetFlags.doConvVtxNtuple    = False
InDetFlags.doV0VtxNtuple      = False
InDetFlags.doRefit            = doRefitTracks
InDetFlags.doLowBetaFinder    = False
InDetFlags.doPrintConfigurables = True
InDetFlags.doStatistics   = doInDetRecStatistics

# print InDetFlags

#primary vertex settings 
InDetFlags.primaryVertexSetup = 'DefaultFullFinding'
InDetFlags.useBeamConstraint = False

InDetFlags.primaryVertexCutSetup = "StartUp"
#InDetPriVxFinderTool.chi2CutMethod = 2
#print InDetPriVxFinderTool

#--------------------------------------------------------------
# load master InDet joboptions file
#--------------------------------------------------------------
  
include("InDetRecExample/InDetRec_all.py")

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if doWriteESD:
  StreamESD.OutputFile = "/tmp/pbehera/InDetRecESD_new.root"
    
if doWriteAOD:
  StreamAOD.OutputFile = "/tmp/pbehera/InDetRecAOD_new.root"
 
