#CONTROL

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = False
doWriteAOD      = False
# --- read BS - please look at the default input file(s) to know the det descr and cond tag
doReadBS        = False
if globalflags.InputFormat() == 'bytestream':
    doReadBS=True
    
doWriteBS       = False
# --- do auditors ?
doAuditors      = True

# this might be useful to trace s.v. faults
import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
    # --- do EDM monitor (debug mode only)
    doEdmMonitor    = True 
    # --- write out a short message upon entering or leaving each algorithm
    doNameAuditor   = True
else:
    doEdmMonitor    = False
    doNameAuditor   = False

#--------------------------------------------------------------
# data or simulation
#--------------------------------------------------------------

mySimulation=False

print "mySimulation=",mySimulation

from RecExConfig.RecFlags import rec
from RecExConfig.InputFilePeeker import inputFileSummary
if inputFileSummary['evt_type'][0]=='IS_SIMULATION':
   mySimulation=True
 
print " SCT Ntuple will be created from simulated data: ",mySimulation

#--------------------------------------------------------------
# detector description version: both RDO and BS default use this
#--------------------------------------------------------------


#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------
import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults

if mySimulation:
    globalflags.DataSource = 'geant4'
else:
    globalflags.DataSource = 'data'

#--------------------------------------------------------------
# Detector Set Up
#-------------------------------------------------------------

from RecExConfig.RecFlags import rec
rec.Commissioning=False
rec.doESD.set_Value_and_Lock(True)
rec.doInDet.set_Value_and_Lock(True)
rec.doEgamma.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False) ## write out ESDs
rec.doJiveXML.set_Value_and_Lock(False) ## for ATLANTIS
readESD = True
readRDO = False
if globalflags.InputFormat() == 'bytestream':
    readESD = False
elif globalflags.InputFormat() == 'pool' and globalflags.DataSource() == 'geant4':
    redESD = False
    redRDO = True
    
rec.readESD=readESD
rec.readRDO=readRDO


from AthenaCommon.DetFlags import DetFlags 
# --- switch on Detector bits
DetFlags.ID_setOn()
#DetFlags.makeRIO.pixel_setOff()
#DetFlags.makeRIO.SCT_setOff()
#DetFlags.makeRIO.TRT_setOff()

# Swithc of the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

DetFlags.Print()

#--------------------------------------------------------------
# control algorithms to be rerun
#--------------------------------------------------------------
# --- run InDetRecStatistics (only possible if readESD = True)
doInDetRecStatistics = True and rec.doESD
# --- refit the EXISTING tracks in ESD (only possible if readESD = True)
doRefitTracks = readESD #False #False 
# --- redo the pattern reco and the tracking (do not use that in conjunction with doRefitTracks above)
redoPatternRecoAndTracking = True
# --- redo primary vertexing (will be set to true later automatically if you redid the tracking and want to redo the TrackParticle creation)
reDoPrimaryVertexing = True 
# --- redo particle creation (recommended after revertexing on ESD, otherwise trackparticles are inconsistent)
reDoParticleCreation = True 
# --- redo conversion finding
reDoConversions = False
# --- redo V0 finding
reDoV0Finder = False
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

import AthenaPython.ConfigLib as apcl

if globalflags.InputFormat() == 'bytestream':
    cfg = apcl.AutoCfg(name = 'MyAutoConfig', input_files=athenaCommonFlags.BSRDOInput())
    cfg.configure_job()
else:
    cfg = apcl.AutoCfg(name = 'MyAutoConfig', input_files=athenaCommonFlags.FilesInput())
    cfg.configure_job()

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth            = (globalflags.DataSource == 'geant4' and globalflags.InputFormat == "pool")
InDetFlags.preProcessing = redoPatternRecoAndTracking

InDetFlags.doPRDFormation = False  # For SCT_Clusters
if globalflags.InputFormat() == 'bytestream':
    InDetFlags.doPRDFormation = True
elif globalflags.InputFormat() == 'pool' and globalflags.DataSource() == 'geant4':
    InDetFlags.doPRDFormation = True
     

InDetFlags.doSpacePointFormation = redoPatternRecoAndTracking  # preProcessing is false
InDetFlags.doNewTracking     = redoPatternRecoAndTracking

if readESD:
    InDetFlags.doLowPt           = False
    InDetFlags.doCTBTracking      = False
    
InDetFlags.doBackTracking     = redoPatternRecoAndTracking
InDetFlags.doTRTStandalone    = redoPatternRecoAndTracking
InDetFlags.doTrtSegments      = redoPatternRecoAndTracking
InDetFlags.postProcessing     = reDoPrimaryVertexing or reDoParticleCreation or reDoConversions or doInDetRecStatistics or reDoV0Finder
InDetFlags.doSlimming         = False
InDetFlags.loadTools          = True
InDetFlags.doVertexFinding    = reDoPrimaryVertexing
InDetFlags.doParticleCreation = reDoParticleCreation
InDetFlags.doConversions      = reDoConversions
if readESD:
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

#---------------------------------------------------------------
# Load master JO
#---------------------------------------------------------------
include("InDetRecExample/InDetRec_all.py")





