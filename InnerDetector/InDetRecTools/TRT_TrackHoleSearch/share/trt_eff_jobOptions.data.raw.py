#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

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
doReadBS        = True
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

#--------------------------------------------------------------
# detector description version: both RDO and BS default use this
#--------------------------------------------------------------
DetDescrVersion = "ATLAS-GEO-18-01-01"

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------
import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'data'
# --- BS default input uses this
if 'doReadBS' in dir() and doReadBS:
  globalflags.InputFormat   = 'bytestream'
  globalflags.ConditionsTag = 'COMCOND-BLKPST-005-01'
else:
  globalflags.InputFormat = 'pool'

# --- set geometry version
globalflags.DetDescrVersion = DetDescrVersion
# --- printout
globalflags.print_JobProperties()

if len(globalflags.ConditionsTag())!=0:
   from IOVDbSvc.CondDB import conddb
   conddb.setGlobalTag(globalflags.ConditionsTag()) 
    
from AthenaCommon.BeamFlags import jobproperties
# --- in Tier0 reco these flags are set from outside
#Let number of collisions default to 0.0 since input file doesn't include pileup 
#jobproperties.Beam.numberOfCollisions = 10.0 # arbitrary higher number for 7+ TeV, for 900 GeV it is 2.
jobproperties.Beam.energy             = 7.*Units.TeV
jobproperties.Beam.beamType           = 'collisions'
if 'doReadBS' in dir() and doReadBS:
#  jobproperties.Beam.numberOfCollisions = 6.9
  jobproperties.Beam.bunchSpacing       = 75

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
rec.Commissioning=False

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
#DetFlags.makeRIO.pixel_setOff()
#DetFlags.makeRIO.SCT_setOff()
#DetFlags.makeRIO.TRT_setOff()

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

# --- switch off DCS
#DetFlags.dcs.pixel_setOff()
#DetFlags.dcs.SCT_setOff()
#DetFlags.dcs.TRT_setOff()

# --- printout
DetFlags.Print()

# --- output level
#OutputLevel          = DEBUG

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking          = False
#InDetFlags.doLowPt                = True
#InDetFlags.doBeamGas              = True
#InDetFlags.doBeamHalo             = True
#InDetFlags.doBackTracking         = False
#InDetFlags.doSingleSpBackTracking = True
#InDetFlags.doTRTStandalone        = False
#InDetFlags.doLowBetaFinder        = False

# --- Turn off track slimming
#InDetFlags.doSlimming = False

# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doTrackSegmentsPixel = True
#InDetFlags.doTrackSegmentsSCT   = True
#InDetFlags.doTrackSegmentsTRT   = True

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- activate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon        = True
# --- activate creation of standard plots
#InDetFlags.doPhysValMon  = True
# --- active storegate delection
#InDetFlags.doSGDeletion  = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkNtuple      = True
#InDetFlags.doPixelTrkNtuple = True
#InDetFlags.doSctTrkNtuple   = True
#InDetFlags.doTrtTrkNtuple   = True
#InDetFlags.doSctClusterNtuple   = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True

# --- produce various ntuples (all in one root file)
#InDetFlags.doTrkD3PD = True
#InDetFlags.doPixelTrkD3PD = True
#InDetFlags.doSctTrkD3PD = True
#InDetFlags.doTrtTrkD3PD = True
#InDetFlags.doVtxD3PD = True
#InDetFlags.doConvVtxD3PD = True  # NOT implemented yet
#InDetFlags.doV0VtxD3PD = True    # NOT implemented yet

#InDetFlags.doMinBias   = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------
from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.TRT_BuildStrawLayers = True

include("InDetRecExample/InDetRec_all.py")

# --- dump MC truth into logfile
#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC(McEventKey = "TruthEvent")

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = 5
#ServiceMgr.EventSelector.SkipEvents = 2
#ServiceMgr.StoreGateSvc.Dump = True

if not doReadBS:
  ServiceMgr.PoolSvc.AttemptCatalogPatch=True

  # --- t tbar channel 5200 (DetDescrVersion = "ATLAS-DC3-07")
  # ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d38/ID_ValidationData/testIdeal_07.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000201_tid002760._00002.pool.root.1"]
  # --- TP split version (local) - (DetDescrVersion = "ATLAS-DC3-07")
  #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d89/InDetRecRDO.root"]
  # --- Minbias (DetDescrVersion = "ATLAS-GEO-08-00-01")
  #ServiceMgr.EventSelector.InputCollections = ["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/mc09_900GeV.105001.pythia_minbias.digit.RDO.e466_s604_s582_d224_tid097817/RDO.097817._000001.pool.root.1"]
  # --- t tbar channel 5200 (DetDescrVersion = "ATLAS-CSC-02-00-00")
  ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d158/CSC.005200.T1_McAtNlo_Jimmy.RDO.pool.root" ]
  #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d89/InDetRecRDO.root"]
  # --- gamma 20 (ATLAS-CSC-01-00-00)
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/datafiles/egamma/DC3.007040.singlepart_gamma_Et20/digit/120031/ideal0_mc12.007040.singlepart_gamma_Et20.digit.RDO.v12003108_tid005022._00001.pool.root"]
  #ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/datafiles/egamma/DC3.007040.singlepart_gamma_Et20/digit/120031/ideal0_mc12.007040.singlepart_gamma_Et20.digit.RDO.v12003108_tid005022._00002.pool.root"]
  # --- black hole event
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/user/j/jgoncalo/valid12003108/misal1_mc12.006640.CharybdisJimmy.digit.RDO.v12003108_tid006066._00038.pool.root.4"]
  # --- event with low lumi pile up (4.6 min bias, 2*10^33), should be ATLAS-CSC-01-02-00
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/user/k/kgrybel/pileup/CSC/005850.pyt_WH120_munubb/digits/misal1_csc11.005850.WH120bb_pythia.digit.RDOS.v12003103_lumi02._00001.pool.root"]

if doReadBS:
  # input used for FDR2b (run 8): low lumi (2.3 min bias, 10^33)/pile-up/shifted vertex run 8
  # this file needs (as set by default): DetDescrVersion = "ATLAS-CSC-02-00-00" globalflags.ConditionsTag="OFLCOND-FDR-02-01-00", jobproperties.Beam.numberOfCollisions = 2.3
  ServiceMgr.EventSelector.Input = [ "/tmp/jahreda/data11_7TeV.00191628.physics_ZeroBias.merge.RAW/data11_7TeV.00191628.physics_ZeroBias.merge.RAW._lb0216._SFO-ALL._0001.1"]
  # if you create the input BS file yourself with InDetWriteBS jobO the output will be this file
  # and you have to set the detdescr and cond tag to what you used ...
  #ServiceMgr.EventSelector.Input = [ "daq.csc13.0000000.Single.Stream.LB0000.Athena._0001.data" ]

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator = AtlasExtrapolator()
ToolSvc += theAtlasExtrapolator

from TRT_TrackHoleSearch.TRT_TrackHoleSearchConf import TRTTrackHoleSearchTool
theTRTTrackHoleSearchTool = TRTTrackHoleSearchTool(
        OutputLevel = WARNING,
        extrapolator = theAtlasExtrapolator,
        conditions_svc = InDetTRTConditionsSummaryService, # defined in InDetRec_all.py
        use_conditions_svc = True,
        do_dump_bad_straw_log = False,
        begin_at_first_trt_hit = False, # if not, extrapolate from last Si hit
        end_at_last_trt_hit = False, # if not, continue hole search to the edge of the TRT
        max_trailing_holes = 1, # only used if end_at_last_trt_hit=False
        locR_cut = -1, # 1.4*mm  # negative means no cut
        locR_sigma_cut = -1)

ToolSvc += theTRTTrackHoleSearchTool
from TRT_TrackHoleSearch.TRT_TrackHoleSearchConf import TRTStrawEfficiency

theTRTStrawEfficiency = TRTStrawEfficiency(
        OutputLevel = INFO,
        trt_hole_finder = theTRTTrackHoleSearchTool,
        max_abs_d0 = 10*mm,
        max_abs_z0 = 300*mm,
        min_pT = 2.0*GeV,
        min_pixel_hits = 1,
        min_sct_hits = 2,
        min_trt_hits = 10,
        track_collection = 'CombinedInDetTracks',
        tree_name = 'trt_eff',
        stream_name = 'TRTEffStream',
#        required_trigger = 'L1_J55',
        )
topSequence += theTRTStrawEfficiency




# --- THist Service
if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["TRTEffStream DATAFILE='efficiency.root' OPT='RECREATE'" ]
ServiceMgr.THistSvc.OutputLevel = WARNING
