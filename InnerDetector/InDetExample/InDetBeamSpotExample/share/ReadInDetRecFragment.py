# Job option fragment for JobRunner templates to setup InDet
# environment following InDetRecExample/ReadInDet_jobOptions.py
# (and InDetRecExample/jobOptions.py).
#
# NOTE: Be sure to include this template before InDetBeamSpotExample/JobSetupFragment.py,
#       otherwise you may not get IOVDbSvc connected to the correct database!
#
# Written by Juerg Beringer in November 2009.
#
printfunc ("InDetBeamSpotExample INFO Using ReadInDetRecFragment.py atlas/athena")


# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
if not 'jobConfig' in dir():                         jobConfig = {}
if not 'outputlevel' in jobConfig:                   jobConfig['outputlevel'] = WARNING
if not 'outputfileprefix' in jobConfig:              jobConfig['outputfileprefix'] = ''
if not 'outputfile' in jobConfig:                    jobConfig['outputfile'] = jobConfig['outputfileprefix']+'dpd.root'
if not 'globalmonfile' in jobConfig:                 jobConfig['globalmonfile'] = jobConfig['outputfileprefix']+'globalmonitoring.root'
if not 'DataSource' in jobConfig:                    jobConfig['DataSource'] = 'data'   # data or geant4
if not 'DetDescrVersion' in jobConfig:               jobConfig['DetDescrVersion'] = 'ATLAS-R2-2015-03-01-00'
if not 'hasMag' in jobConfig:                        jobConfig['hasMag'] = True
if not 'hasSCT' in jobConfig:                        jobConfig['hasSCT'] = True
if not 'hasSCTStandby' in jobConfig:                 jobConfig['hasSCTStandby'] = False
if not 'hasPix' in jobConfig:                        jobConfig['hasPix'] = True
if not 'doPatternRecoAndTracking' in jobConfig:      jobConfig['doPatternRecoAndTracking'] = False
if not 'doPrimaryVertexing' in jobConfig:            jobConfig['doPrimaryVertexing'] = False
if not 'doSplitVertexing' in jobConfig:              jobConfig['doSplitVertexing'] = False
if not 'doVertexSplitting' in jobConfig:             jobConfig['doVertexSplitting'] = False
if not 'doForceWriteDPD' in jobConfig:               jobConfig['doForceWriteDPD'] = False
if not 'doMonitoringGlobal' in jobConfig:            jobConfig['doMonitoringGlobal'] = False
if not 'doLowPt' in jobConfig:                       jobConfig['doLowPt'] = True
if not 'doRobustReco' in jobConfig:                  jobConfig['doRobustReco'] = False
if not 'doTruth' in jobConfig:                       jobConfig['doTruth'] = False
if not 'doPrintIndetConfig' in jobConfig:            jobConfig['doPrintIndetConfig'] = False
if not 'jobpostprocsteps' in jobConfig:              jobConfig['jobpostprocsteps'] = ''

# WARNING: Be very careful if you ever change the default for commissioning, since it may
#          break beam spot reconstruction! Setting rec.commissioning=True seems to not only
#          define cuts but to also include lots of stuff, which is normally not desired
#          when reading from AOD or ESD. Hence the default for commissioning should be False.
if not 'commissioning' in jobConfig:                 jobConfig['commissioning'] = False

# Needed for vertex splitting
if not 'doVertexStandalone' in jobConfig:            jobConfig['doVertexStandalone'] = False
if jobConfig['doVertexSplitting'] :                  jobConfig['doVertexStandalone'] = True


# Used only if present:
# - primaryVertexSetup        (e.g. AdaptiveMultiFinding,DefaultFastFinding, ...)
# - primaryVertexCutSetup     (e.g. StartUp, ...)
# - doPrimaryVertex3DFinding  (True)

# Temporary (for version tag patch)
if not 'griduser' in jobConfig:                      jobConfig['griduser'] = ''


#--------------------------------------------------------------
# control input/output
#--------------------------------------------------------------
readESD = jobConfig['doPatternRecoAndTracking'] or jobConfig['doPrimaryVertexing']
readAOD = not readESD
doWriteESD = False and readESD
doWriteAOD = False # jobConfig['doPatternRecoAndTracking'] or jobConfig['doPrimaryVertexing'] or jobConfig['doForceWriteDPD']


printfunc (jobConfig['inputfiles'])
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = jobConfig['inputfiles']

#--------------------------------------------------------------
# control algorithms to be rerun
#--------------------------------------------------------------
# --- run InDetRecStatistics (only possible if readESD = True)
doInDetRecStatistics = False and readESD
# --- refit the EXISTING tracks in ESD (only possible if readESD = True)
doRefitTracks = False and readESD
# --- redo the pattern reco and the tracking (do not use that in conjunction with doRefitTracks above)
redoPatternRecoAndTracking = jobConfig['doPatternRecoAndTracking'] and not doRefitTracks and readESD
# --- redo primary vertexing (will be set to true later automatically if you redid the tracking and want to redo the TrackParticle creation)
reDoPrimaryVertexing = False #jobConfig['doPrimaryVertexing'] or redoPatternRecoAndTracking or jobConfig['doVertexStandalone']
# --- redo particle creation (recommended after revertexing on ESD, otherwise trackparticles are inconsistent)
reDoParticleCreation = False #(redoPatternRecoAndTracking or reDoPrimaryVertexing) and not jobConfig['doVertexStandalone']
# --- redo conversion finding
reDoConversions = False
# --- redo V0 finding
reDoV0Finder = False

if jobConfig['commissioning']:
  from RecExConfig.RecFlags import rec
  rec.Commissioning = True
  rec.doMonitoring = False

#--------------------------------------------------------------
# Control - standard options (as in jobOptions.py)
#--------------------------------------------------------------
# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = jobConfig['outputlevel']
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- do auditors ?
doAuditors      = True

doEdmMonitor    = False
doNameAuditor   = False

# safety section ... redoing tracking/vertexing is a tricky business to stay consistent ...
if redoPatternRecoAndTracking and reDoParticleCreation:
  reDoPrimaryVertexing = True

if not (readESD or readAOD):
  printfunc ("You have to turn on reading of ESD or AOD! That's the purpose of this jobO!")
if readESD and readAOD:
  printfunc ("I can either read ESD or AOD but not both at the same time! Turn one or the other off!")
if readESD and reDoPrimaryVertexing and not reDoParticleCreation:
  printfunc ("INFO! You are running on ESD, redoing the vertexing but not recreating the TrackParticles!")
  printfunc ("INFO! To avoid inconsistencies do not use the old track particles in conjunction with the new vertex!")
  if doWriteESD or doWriteAOD:
    printfunc ("INFO! To avoid inconsistencies the old track particle (truth) container will not be in the new ESD/AOD!")
if readAOD and reDoPrimaryVertexing:
  printfunc ("INFO! You are running on AOD, and redoing the vertexing. At the moment new track particles cannot be made from old ones.")
  printfunc ("INFO! To avoid inconsistencies do not use the old track particles in conjunction with the new vertex!")
  if doWriteAOD:
    printfunc ("INFO! To avoid inconsistencies the old track particle (truth) container will not be in the new AOD!")
if doRefitTracks and (reDoPrimaryVertexing or reDoParticleCreation):
  printfunc ("INFO! You are refitting tracks and also revertex and/or recreate track particles")
  printfunc ("INFO! The input for that will be the refitted tracks!")


#--------------------------------------------------------------
# detector description version
#--------------------------------------------------------------
DetDescrVersion = jobConfig['DetDescrVersion']

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource  = jobConfig['DataSource']
globalflags.InputFormat = 'pool'
# --- set geometry version
globalflags.DetDescrVersion = DetDescrVersion
# --- printout
if jobConfig['doPrintIndetConfig']:
  globalflags.print_JobProperties()

from AthenaCommon.BeamFlags import jobproperties
# --- default is zero luminosity
#jobproperties.Beam.numberOfCollisions = 0.0 # default in RecExCommon for 900 GeV
# TODO: Is any of the following needed?
#jobproperties.Beam.energy             = 900.*Units.GeV
#jobproperties.Beam.beamType           = 'collisions'


#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

# Configure magnetic field, if necessary
if not jobConfig['hasMag']:
    from AthenaCommon.BFieldFlags import jobproperties
    jobproperties.BField.solenoidOn = False

from AthenaCommon.DetFlags import DetFlags 
from InDetRecExample.InDetJobProperties import InDetFlags
# --- switch on InnerDetector
DetFlags.ID_setOn()
#DetFlags.ID_setOff()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# ---- switch parts of ID off/on as follows (always use both lines)
if not jobConfig['hasPix']:
    DetFlags.pixel_setOff()
    DetFlags.detdescr.pixel_setOn()

if not jobConfig['hasSCT']:
  if jobConfig['hasSCTStandby']:
    # SPECIAL for 20V bias
    # Initial trick
    #InDetFlags.useDCS = False
    # Preferred according to Pat Ward - This has the effect of making a channel
    # whose status is 'standby' good if the HV is > 19.5V, but one which has tripped
    # (or is <19.5V) will be bad. If you use InDetFlags.useDCS = False
    # then the DCS information is ignored and all channels will be 'good'.
    InDetFlags.useHVForSctDCS.set_Value_and_Lock(True)
  else:
    DetFlags.SCT_setOff()
    DetFlags.detdescr.SCT_setOn()

DetFlags.TRT_setOff()
#DetFlags.detdescr.TRT_setOn()
# --- printout
if jobConfig['doPrintIndetConfig']:
  DetFlags.Print()

#--------------------------------------------------------------
# Load Reconstruction configuration for tools only
#--------------------------------------------------------------
#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
InDetFlags.doTruth            = jobConfig['doTruth']
InDetFlags.preProcessing      = redoPatternRecoAndTracking
InDetFlags.doPRDFormation        = False                       # those two will be (later) automatically false if
InDetFlags.doSpacePointFormation = redoPatternRecoAndTracking  # preProcessing is false
InDetFlags.doNewTracking      = redoPatternRecoAndTracking
InDetFlags.doLowPt            = jobConfig['doLowPt']
if jobConfig['doRobustReco']:
    try:
        InDetFlags.doRobustReco = True
    except:
        printfunc ('ERROR: Unable to set InDetFlags.doRobustReco = True')
InDetFlags.doBackTracking     = redoPatternRecoAndTracking
InDetFlags.doTRTStandalone    = redoPatternRecoAndTracking
InDetFlags.doTrtSegments      = redoPatternRecoAndTracking
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
InDetFlags.doPrintConfigurables = jobConfig['doPrintIndetConfig']

### Key flag  -- this will create the unconstrained beamspot collection on the fly from standard xAOD::TrackParticles
InDetFlags.doVertexFindingForMonitoring = jobConfig['doPrimaryVertexing']


# Standard monitoring
InDetFlags.doMonitoringGlobal    = jobConfig['doMonitoringGlobal']

# Beam constraints are always disabled for beam spot work
InDetFlags.useBeamConstraint = False

# In case we don't want to use the default vertexing settings:
if 'primaryVertexSetup' in jobConfig:
  InDetFlags.primaryVertexSetup = jobConfig['primaryVertexSetup']
if 'primaryVertexCutSetup' in jobConfig:
  InDetFlags.primaryVertexCutSetup = jobConfig['primaryVertexCutSetup']
if 'doPrimaryVertex3DFinding' in jobConfig:
    InDetFlags.doPrimaryVertex3DFinding = jobConfig['doPrimaryVertex3DFinding']

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

from InDetRecExample.InDetKeys import InDetKeys
if InDetFlags.doVertexFinding() and readAOD:
  InDetKeys.Tracks = InDetKeys.TrackParticles()
  if jobConfig['doPrintIndetConfig']:
    printfunc ("InDetKeys.Tracks = "+InDetKeys.Tracks())

if readESD and not redoPatternRecoAndTracking:
  InDetKeys.UnslimmedTracks              = 'Tracks'
  InDetKeys.UnslimmedTracksTruth         = 'TrackTruthCollection'

# Set outputfile properly, so that we get version tags included
# For the time being, we keep the patch for older releases
try:
  if doWriteESD:
    InDetKeys.OutputESDFileName = jobConfig['outputfile']
  if doWriteAOD:
    InDetKeys.OutputAODFileName = jobConfig['outputfile']
except:
  printfunc ("WARNING: Release doesn't seem to have fix for missing version tags - will try to apply patch")
  needsOutputFilePatch = True
else:
  needsOutputFilePatch = False

InDetKeys.lockAllExceptAlias()
if jobConfig['doPrintIndetConfig']:
  printfunc ("Printing InDetKeys")
  InDetKeys.print_JobProperties()


if jobConfig['doPrimaryVertexing'] : 
  jobConfig['VertexContainer'] = InDetKeys.PrimaryVerticesWithoutBeamConstraint()
  from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
  RefittedVertexStream = MSMgr.NewPoolRootStream( "RefittedVertexStream", "RefittedVertex.pool.root" )
  RefittedVertexStream.AddItem( 'xAOD::VertexContainer#'+InDetKeys.PrimaryVerticesWithoutBeamConstraint() )
  RefittedVertexStream.AddItem( 'xAOD::VertexAuxContainer#'+InDetKeys.PrimaryVerticesWithoutBeamConstraint()+'Aux.-vxTrackAtVertex' )
  RefittedVertexStream.AddItem( 'xAOD::EventInfo#EventInfo' )
  RefittedVertexStream.AddItem( 'xAOD::EventAuxInfo#EventInfoAux.' )


#--------------------------------------------------------------
# enable statistics for reading ESD testing
#--------------------------------------------------------------

InDetFlags.doStatistics   = doInDetRecStatistics
TrackCollectionKeys        = [InDetKeys.Tracks()]
TrackCollectionTruthKeys   = [InDetKeys.TracksTruth()]
  
#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------
  
include("InDetRecExample/InDetRec_all.py")

#--------------------------------------------------------------
# Remaining streams parameters
#--------------------------------------------------------------
if doWriteESD:
  if needsOutputFilePatch:
    if not jobConfig['griduser']:
      # See comment below on version tag info.
      itemList = StreamESD.ItemList
      StreamESD = AthenaPoolOutputStream ( "StreamESD" , jobConfig['outputfile'] )
      StreamESD.ItemList = itemList
    else:
      StreamESD.OutputFile = jobConfig['outputfile']

if doWriteAOD:
  if needsOutputFilePatch:
    if not jobConfig['griduser']:
      # The following is a hack to get version tag info into the DPD. Simply
      # setting StreamAOD.OutputFile to the desired file name does NOT work.
      # At the moment, this does NOT yet work with pathena.
      itemList = StreamAOD.ItemList
      StreamAOD = AthenaPoolOutputStream ( "StreamAOD" , jobConfig['outputfile'] )
      StreamAOD.ItemList = itemList
    else:
      StreamAOD.OutputFile = jobConfig['outputfile']

#--------------------------------------------------------------
# If we do global monitoring, set the histogram file name and
# specify corresponding postprocessing step
#--------------------------------------------------------------
if jobConfig['doMonitoringGlobal']:
    ServiceMgr += CfgMgr.THistSvc()
    # Dangerous - this overrides everything to replace the monitoring stream
    ServiceMgr.THistSvc.Output = [ "GLOBAL DATAFILE='%s' OPT='RECREATE'" % jobConfig['globalmonfile'] ]

    # Default postprocessing step(s)
    jobConfig['jobpostprocsteps'] += ' PlotGlobalMon'
