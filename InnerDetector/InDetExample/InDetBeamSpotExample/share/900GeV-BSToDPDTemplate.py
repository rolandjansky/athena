# $Id: 900GeV-BSToDPDTemplate.py 279935 2010-02-16 23:18:33Z beringer $
#
# Top-level job options file to produce minimal AOD files suitable for beamspot
# determination using a JobRunner from RAW or RDO files. This template is based
# on InDetRecExample/jobOptions_900gev.py.
#
# WARNING: This particular version is customized for the initial collsision from
#          run 140541 (solenoid and pixels off, SCT bias voltage at most 50V)
#
# Written by Juerg Beringer in November 2009.
#
printfunc ("InDetBeamSpotExample INFO Using $Id: 900GeV-BSToDPDTemplate.py 279935 2010-02-16 23:18:33Z beringer $")


# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
# See also fragments included below for further jobConfig parameters
if not 'jobConfig' in dir():                         jobConfig = {}
if not 'outputlevel' in jobConfig:                   jobConfig['outputlevel'] = WARNING
if not 'outputfileprefix' in jobConfig:              jobConfig['outputfileprefix'] = ''
if not 'outputfile' in jobConfig:                    jobConfig['outputfile'] = jobConfig['outputfileprefix']+'dpd.root'
if not 'globalmonfile' in jobConfig:                 jobConfig['globalmonfile'] = jobConfig['outputfileprefix']+'globalmonitoring.root'
if not 'DataSource' in jobConfig:                    jobConfig['DataSource'] = 'data'   # data or geant4
if not 'bytestream' in jobConfig:                    jobConfig['bytestream'] = True
if not 'DetDescrVersion' in jobConfig:               jobConfig['DetDescrVersion'] = 'ATLAS-CSC-02-00-00'
if not 'primaryVertexSetup' in jobConfig:            jobConfig['primaryVertexSetup'] = 'DefaultFullFinding'
if not 'primaryVertexCutSetup' in jobConfig:         jobConfig['primaryVertexCutSetup'] = 'StartUp'
if not 'doTruth' in jobConfig:                       jobConfig['doTruth'] = False
if not 'doPrintIndetConfig' in jobConfig:            jobConfig['doPrintIndetConfig'] = False
if not 'doMonitoring' in jobConfig:                  jobConfig['doMonitoring'] = True
if not 'doMonitoringGlobal' in jobConfig:            jobConfig['doMonitoringGlobal'] = True
if not 'hasMag' in jobConfig:                        jobConfig['hasMag'] = True
if not 'hasSCT' in jobConfig:                        jobConfig['hasSCT'] = True
if not 'hasPix' in jobConfig:                        jobConfig['hasPix'] = True
# Used only if present:
# - GlobalTag


# General flags (used by InDetRecExample/InDetRec_all.py)
OutputLevel     = jobConfig['outputlevel']
doJiveXML       = False
doVP1           = False
doWriteESD      = False
doWriteAOD      = True
doReadBS        = jobConfig['bytestream']
doAuditors      = False
doEdmMonitor    = False
doNameAuditor   = False


# Auto configuration (probably not needed)
include("InDetBeamSpotExample/AutoConfFragment.py")

# DetDescrVersion and globalflags
DetDescrVersion = jobConfig['DetDescrVersion']
import AthenaCommon.SystemOfUnits as Units

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource  = jobConfig['DataSource']
if jobConfig['bytestream']:
    globalflags.InputFormat   = 'bytestream'
    if 'GlobalTag' in jobConfig:
        globalflags.ConditionsTag = jobConfig['GlobalTag']
else:
    globalflags.InputFormat = 'pool'
globalflags.DetDescrVersion = DetDescrVersion
if jobConfig['doPrintIndetConfig']:
    globalflags.print_JobProperties()

if len(globalflags.ConditionsTag())!=0:
   from IOVDbSvc.CondDB import conddb
   conddb.setGlobalTag(globalflags.ConditionsTag())


# Jobproperties
from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.numberOfCollisions = 2.0 # default in RecExCommon for 900 GeV
#jobproperties.Beam.numberOfCollisions = 0.0 # default in RecExCommon for 900 GeV
jobproperties.Beam.energy             = 900.*Units.GeV
jobproperties.Beam.beamType           = 'collisions'


# RecExConfig flags
from RecExConfig.RecFlags import rec
rec.Commissioning=True
rec.doMonitoring=False

# Configure magnetic field, if necessary
if not jobConfig['hasMag']:
    from AthenaCommon.BFieldFlags import jobproperties
    jobproperties.BField.solenoidOn = False

# Detector flags
from AthenaCommon.DetFlags import DetFlags 
DetFlags.ID_setOn()
#DetFlags.makeRIO.pixel_setOff()
#DetFlags.makeRIO.SCT_setOff()
#DetFlags.makeRIO.TRT_setOff()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# ---- switch parts of ID off/on as follows (always use both lines)
if not jobConfig['hasPix']:
    DetFlags.pixel_setOff()
    DetFlags.detdescr.pixel_setOn()
# Leave SCT in, even if it is "off" - it may still be at 20V and get hits
#DetFlags.SCT_setOff()
#DetFlags.detdescr.SCT_setOn()
#DetFlags.TRT_setOff()
#DetFlags.detdescr.TRT_setOn()
# --- switch off DCS
#DetFlags.dcs.pixel_setOff()
#DetFlags.dcs.SCT_setOff()
#DetFlags.dcs.TRT_setOff()
if jobConfig['doPrintIndetConfig']:
    DetFlags.Print()


# InDetFlags
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = jobConfig['doTruth']

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking  = False
#InDetFlags.doLowPt        = True
#InDetFlags.doBeamGas      = True
#InDetFlags.doBeamHalo     = True
#InDetFlags.doBackTracking = False
#InDetFlags.doTRTStandalone = False
#InDetFlags.doSingleSpBackTracking = True

# --- Turn off track slimming
#InDetFlags.doSlimming = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doTrackSegmentsPixel = True
#InDetFlags.doTrackSegmentsSCT   = True
#InDetFlags.doTrackSegmentsTRT   = True

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- deactivate monitorings
InDetFlags.doMonitoringGlobal    = jobConfig['doMonitoringGlobal']
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# --- activate creation of standard plots
#InDetFlags.doPhysValMon  = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkNtuple      = True
#InDetFlags.doPixelTrkNtuple = True
#InDetFlags.doSctTrkNtuple   = True
#InDetFlags.doTrtTrkNtuple   = True
#InDetFlags.doSctClusterNtuple   = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True

InDetFlags.primaryVertexSetup    = jobConfig['primaryVertexSetup']
InDetFlags.primaryVertexCutSetup = jobConfig['primaryVertexCutSetup']

if not jobConfig['hasSCT']:
  # SPECIAL for 20V bias
  # Initial trick
  #InDetFlags.useDCS = False
  # Preferred according to Pat Ward - This has the effect of making a channel
  # whose status is 'standby' good if the HV is > 19.5V, but one which has tripped
  # (or is <19.5V) will be bad. If you use InDetFlags.useDCS = False
  # then the DCS information is ignored and all channels will be 'good'.
  InDetFlags.useHVForSctDCS.set_Value_and_Lock(True)

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = jobConfig['doPrintIndetConfig']

include("InDetRecExample/InDetRec_all.py")

if doWriteESD:
    StreamESD.OutputFile = jobConfig['outputfile']

if doWriteAOD:
    StreamAOD.OutputFile = jobConfig['outputfile']


#--------------------------------------------------------------
# If we do global monitoring, set the histogram file name
#--------------------------------------------------------------
if jobConfig['doMonitoringGlobal']:
    ServiceMgr += CfgMgr.THistSvc()
    # Dangerous - this overrides everything to replace the monitoring stream
    ServiceMgr.THistSvc.Output = [ "GLOBAL DATAFILE='%s' OPT='RECREATE'" % jobConfig['globalmonfile'] ]


# Special configuration for beamspot work
if jobConfig['hasPix']:
    ToolSvc.InDetDetailedTrackSelectorTool.nHitPix = 1
else:
    ToolSvc.InDetDetailedTrackSelectorTool.nHitPix = 0
ToolSvc.InDetDetailedTrackSelectorTool.nHitSct = 4
#ToolSvc.InDetDetailedTrackSelectorTool.nHitSi = 0
if not jobConfig['hasMag']:
    ToolSvc.InDetDetailedTrackSelectorTool.pTMin = 0
#ToolSvc.InDetDetailedTrackSelectorTool.useTrackQualityInfo = False
#ToolSvc.InDetDetailedTrackSelectorTool.IPd0Max = 50
#ToolSvc.InDetDetailedTrackSelectorTool.sigIPd0Max = 5.0
#ToolSvc.InDetDetailedTrackSelectorTool.d0significanceMax = -1
#ToolSvc.InDetDetailedTrackSelectorTool.sigIPz0Max = 10.0

if 'InDetPriVxFinderTool' in dir() and hasattr(InDetPriVxFinderTool,'enableMultipleVertices'):
    InDetPriVxFinderTool.enableMultipleVertices = 0

printfunc (ToolSvc.InDetDetailedTrackSelectorTool)


# General job setup
include("InDetBeamSpotExample/JobSetupFragment.py")
topSequence.AthEventCounter.Frequency = 10


# Standard beam spot monitoring
if jobConfig['doMonitoring']:
    include("InDetBeamSpotExample/MonitoringFragment.py")


# Print jobConfig parameters
include("InDetBeamSpotExample/PrintConfFragment.py")
