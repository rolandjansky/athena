# $Id: BSToDPDTemplate.py 323441 2010-09-24 03:45:38Z beringer $
#
# Top-level job options file to produce minimal AOD files suitable for beamspot
# determination using a JobRunner from RAW or RDO files. This template is based
# on InDetRecExample/jobOptions.py.
#
# Written by Juerg Beringer in November 2009.
#
printfunc ("InDetBeamSpotExample INFO Using $Id: BSToDPDTemplate.py 323441 2010-09-24 03:45:38Z beringer $")


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
if not 'commissioning' in jobConfig:                 jobConfig['commissioning'] = True
# if not 'primaryVertexSetup' in jobConfig:            jobConfig['primaryVertexSetup'] = 'DefaultFullFinding'
# if not 'primaryVertexCutSetup' in jobConfig:         jobConfig['primaryVertexCutSetup'] = 'StartUp'
# if not 'doPrimaryVertex3DFinding' in jobConfig:      jobConfig['doPrimaryVertex3DFinding'] = True
if not 'doRobustReco' in jobConfig:                  jobConfig['doRobustReco'] = False
if not 'doLowPt' in jobConfig:                       jobConfig['doLowPt'] = False
if not 'doTruth' in jobConfig:                       jobConfig['doTruth'] = False
if not 'doPrintIndetConfig' in jobConfig:            jobConfig['doPrintIndetConfig'] = False
if not 'doMonitoring' in jobConfig:                  jobConfig['doMonitoring'] = True
if not 'doMonitoringGlobal' in jobConfig:            jobConfig['doMonitoringGlobal'] = True
if not 'hasMag' in jobConfig:                        jobConfig['hasMag'] = True
if not 'hasSCT' in jobConfig:                        jobConfig['hasSCT'] = True
if not 'hasSCTStandby' in jobConfig:                 jobConfig['hasSCTStandby'] = False
if not 'hasPix' in jobConfig:                        jobConfig['hasPix'] = True
if not 'jobpostprocsteps' in jobConfig:              jobConfig['jobpostprocsteps'] = ''
# Used only if present (if desired, comment out above):
# - GlobalTag
# - primaryVertexSetup        (e.g. DefaultFullFinding,AdaptiveMultiFinding,DefaultFastFinding, ...)
# - primaryVertexCutSetup     (e.g. StartUp, ...)
# - doPrimaryVertex3DFinding  (True)

# Temporary (for version tag patch)
if not 'griduser' in jobConfig:                      jobConfig['griduser'] = ''


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
# --- in Tier0 reco these flags are set from outside
#jobproperties.Beam.numberOfCollisions = 2.0 # arbitrary higher number for 7+ TeV, for 900 GeV it i#s 2 (or 0?)
#jobproperties.Beam.energy             = 7.*Units.TeV
#jobproperties.Beam.energy             = 900.*Units.TeV
#jobproperties.Beam.beamType           = 'collisions'
# jobproperties.Beam.bunchSpacing       = 75


# RecExConfig flags
from RecExConfig.RecFlags import rec
rec.Commissioning = jobConfig['commissioning']
rec.doMonitoring = False


# Configure magnetic field, if necessary
if not jobConfig['hasMag']:
    from AthenaCommon.BFieldFlags import jobproperties
    jobproperties.BField.solenoidOn = False


# Detector flags
from AthenaCommon.DetFlags import DetFlags 
from InDetRecExample.InDetJobProperties import InDetFlags
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
#DetFlags.TRT_setOff()
#DetFlags.detdescr.TRT_setOn()
# --- switch off DCS
#DetFlags.dcs.pixel_setOff()
#DetFlags.dcs.SCT_setOff()
#DetFlags.dcs.TRT_setOff()
if jobConfig['doPrintIndetConfig']:
    DetFlags.Print()


# InDetFlags
if jobConfig['doRobustReco']:
    try:
        InDetFlags.doRobustReco = True
    except:
        printfunc ('ERROR: Unable to set InDetFlags.doRobustReco = True')

InDetFlags.doTruth       = jobConfig['doTruth']

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking  = False
InDetFlags.doLowPt        = jobConfig['doLowPt']
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

# Beam constraints always disabled for beam spot work
InDetFlags.useBeamConstraint = False

# In case we don't want to use the default vertexing settings:
if 'primaryVertexSetup' in jobConfig:
    InDetFlags.primaryVertexSetup = jobConfig['primaryVertexSetup']
if 'primaryVertexCutSetup' in jobConfig:
    InDetFlags.primaryVertexCutSetup = jobConfig['primaryVertexCutSetup']
if 'doPrimaryVertex3DFinding' in jobConfig:
    InDetFlags.doPrimaryVertex3DFinding = jobConfig['doPrimaryVertex3DFinding']

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = jobConfig['doPrintIndetConfig']

include("InDetRecExample/InDetRec_all.py")


# --- dump MC truth into logfile
#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC(McEventKey = "TruthEvent")


# TODO: - Is the trick below still necessary?
#       - Should we use InDetKeys as in ReadInDetFragment.py instead
#       - Or use DPD maker?
if doWriteESD:
    if not jobConfig['griduser']:
        # See comment below on version tag info.
        itemList = StreamESD.ItemList
        StreamESD = AthenaPoolOutputStream ( "StreamESD" , jobConfig['outputfile'] )
        StreamESD.ItemList = itemList
    else:
        StreamESD.OutputFile = jobConfig['outputfile']

if doWriteAOD:
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
# If we do global monitoring, set the histogram file name
#--------------------------------------------------------------
if jobConfig['doMonitoringGlobal']:
    ServiceMgr += CfgMgr.THistSvc()
    # Dangerous - this overrides everything to replace the monitoring stream
    ServiceMgr.THistSvc.Output = [ "GLOBAL DATAFILE='%s' OPT='RECREATE'" % jobConfig['globalmonfile'] ]

    # Default postprocessing step(s)
    jobConfig['jobpostprocsteps'] += ' PlotGlobalMon'


# Special configuration for beamspot work (if there's an InDetDetailedTrackSelectorTool)
if hasattr(ToolSvc,'InDetDetailedTrackSelectorTool'):
  if not jobConfig['hasPix']:
    ToolSvc.InDetDetailedTrackSelectorTool.nHitPix = 0
    ToolSvc.InDetDetailedTrackSelectorTool.nHitSi = 0
  if not jobConfig['hasSCT']:
      ToolSvc.InDetDetailedTrackSelectorTool.nHitSct = 0
      ToolSvc.InDetDetailedTrackSelectorTool.nHitSi = 0      
  if jobConfig['hasSCTStandby']:
    # SCT @ 20V - require SCT hits (even if just at 20V)
    ToolSvc.InDetDetailedTrackSelectorTool.nHitSct = 4
    ToolSvc.InDetDetailedTrackSelectorTool.nHitSi = 0
  #ToolSvc.InDetDetailedTrackSelectorTool.nHitTrt = 10
  #ToolSvc.InDetDetailedTrackSelectorTool.nHitTrt = 0
  if not jobConfig['hasMag']:
    ToolSvc.InDetDetailedTrackSelectorTool.pTMin = 0
  #ToolSvc.InDetDetailedTrackSelectorTool.useTrackQualityInfo = False
  #ToolSvc.InDetDetailedTrackSelectorTool.IPd0Max = 25
  #ToolSvc.InDetDetailedTrackSelectorTool.sigIPd0Max = 5.0
  #ToolSvc.InDetDetailedTrackSelectorTool.d0significanceMax = -1
  #ToolSvc.InDetDetailedTrackSelectorTool.sigIPz0Max = 10.0

  printfunc (ToolSvc.InDetDetailedTrackSelectorTool)


# If InDetPriVxFinderTool is configured, print its configuration
if 'InDetPriVxFinderTool' in dir():
  printfunc (InDetPriVxFinderTool)


# General job setup
include("InDetBeamSpotExample/JobSetupFragment.py")
topSequence.AthEventCounter.Frequency = 10


# Standard beam spot monitoring
if jobConfig['doMonitoring']:
    include("InDetBeamSpotExample/MonitoringFragment.py")

# Default postprocessing step(s)
jobConfig['jobpostprocsteps'] += ' LinkResults'

# Print jobConfig parameters
include("InDetBeamSpotExample/PrintConfFragment.py")
