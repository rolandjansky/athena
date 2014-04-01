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
doReadBS        = Myoption_ReadBStream
# --- do auditors ?
doAuditors      = False
	
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
DetDescrVersion = My_DetectorDescription

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------
import AthenaCommon.SystemOfUnits as Units
from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.ConditionsTag = My_ConditionsTag

globalflags.DataSource = My_DataSource
# --- BS default input uses this

if 'doReadBS' in dir() and doReadBS:
    globalflags.InputFormat   = 'bytestream'
    #globalflags.ConditionsTag = 'OFLCOND-FDR-02-01-00'
    if globalflags.DataSource == 'geant4':
        globalflags.ConditionsTag = My_ConditionsTag
    else:
        globalflags.ConditionsTag = My_ConditionsTag
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
# --- default is zero luminosity
jobproperties.Beam.numberOfCollisions = 2.0 # default in RecExCommon for 900 GeV
jobproperties.Beam.energy             = 900.*Units.GeV
jobproperties.Beam.beamType           = 'collisions'

from AthenaCommon.BFieldFlags import jobproperties
#jobproperties.BField.solenoidOn= BField_solenoidOn

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
	
from RecExConfig.RecFlags import rec
rec.Commissioning=False
rec.doMonitoring=False

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

if not doTRT:
  DetFlags.TRT_setOff()
  DetFlags.detdescr.TRT_setOn()
  
# --- switch off DCS
#DetFlags.dcs.pixel_setOff()
#if globalflags.DataSource == 'data':
#  DetFlags.dcs.SCT_setOff()
#DetFlags.dcs.TRT_setOff()

# --- printout
DetFlags.Print()


	
#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------
	
# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.InputFormat() == 'pool' and globalflags.DataSource == 'geant4')
InDetFlags.doCosmics = False

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking  = False
#InDetFlags.doLowPt        = False
#InDetFlags.doBeamGas      = True
#InDetFlags.doBeamHalo     = True
#InDetFlags.doxKalman      = False
#InDetFlags.doiPatRec      = False
#InDetFlags.doBackTracking = False
#InDetFlags.doTRTStandalone = False
#InDetFlags.doSingleSpBackTracking = True
	
# --- Turn off track slimming
InDetFlags.doSlimming = False
	
# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doTrackSegmentsPixel = False
#InDetFlags.doTrackSegmentsSCT   = False
#InDetFlags.doTrackSegmentsTRT   = False

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'
	
# --- deactivate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False
	
# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon = True

# --- activate creation of standard plots
#InDetFlags.doStandardPlots  = True
	
# --- produce various ntuples (all in one root file)
InDetFlags.doTrkNtuple      = False
InDetFlags.doPixelTrkNtuple = False
InDetFlags.doSctTrkNtuple   = False
InDetFlags.doTrtTrkNtuple   = False
InDetFlags.doPixelClusterNtuple = False
InDetFlags.doSctClusterNtuple   = False
InDetFlags.doTrtDriftCircleNtuple = False
InDetFlags.doVtxNtuple      = False
InDetFlags.doConvVtxNtuple  = False
InDetFlags.doV0VtxNtuple    = False

InDetFlags.primaryVertexCutSetup = "StartUp"
InDetFlags.useHVForSctDCS=True
	
# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------
include("InDetRecExample/InDetRec_all.py")
	
#-------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------
	
# Number of events to be processed (default is 10)
theApp.EvtMax = theApp.EvtMax
ServiceMgr.EventSelector.SkipEvents = SkipEvents
#ServiceMgr.StoreGateSvc.Dump = True
	
if not doReadBS:
    ServiceMgr.PoolSvc.AttemptCatalogPatch=True
	
    # --- Minbias (DetDescrVersion = "ATLAS-GEO-08-00-01")
    ServiceMgr.EventSelector.InputCollections = InDetAlignExampleFlags.Files
    #ServiceMgr.EventSelector.InputCollections = ["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/mc09_900GeV.105001.pythia_minbias.digit.RDO.e466_s604_s582_d224_tid097817/RDO.097817._000001.pool.root.1"]
	
    # --- t tbar channel 5200 (DetDescrVersion = "ATLAS-DC3-07")
    # ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d38/ID_ValidationData/testIdeal_07.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000201_tid002760._00002.pool.root.1"]
    # --- TP split version (local) - (DetDescrVersion = "ATLAS-DC3-07")
    #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d89/InDetRecRDO.root"]
    # --- Minbias (DetDescrVersion = "ATLAS-CSC-02-00-00")
    #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d158/CSC.005001.pythia_minbias.RDO.pool.root"]
    # --- t tbar channel 5200 (DetDescrVersion = "ATLAS-CSC-02-00-00")
    #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d158/CSC.005200.T1_McAtNlo_Jimmy.RDO.pool.root" ]
    #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00001.pool.root.1",
    #                                             "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00002.pool.root.1",
    #                                             "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00003.pool.root.1",   
    #                                             "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00004.pool.root.1",
    #                                             "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00005.pool.root.1",
    #                                             "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00006.pool.root.1",
    #                                             "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00007.pool.root.1",
    #                                             "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00008.pool.root.1",
    #                                             "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._00009.pool.root.1",
    #                                             "castor:/castor/cern.ch/user/i/ivivarel/900GeVminbias/RDO.024944._000010.pool.root.1"]
    #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d89/InDetRecRDO.root"]
    # --- gamma 20 (ATLAS-CSC-01-00-00)
    #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/datafiles/egamma/DC3.007040.singlepart_gamma_Et20/digit/120031/ideal0_mc12.007040.singlepart_gamma_Et20.digit.RDO.v12003108_tid005022._00001.pool.root"]
    #ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/datafiles/egamma/DC3.007040.singlepart_gamma_Et20/digit/120031/ideal0_mc12.007040.singlepart_gamma_Et20.digit.RDO.v12003108_tid005022._00002.pool.root"]
    # --- black hole event
    #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/user/j/jgoncalo/valid12003108/misal1_mc12.006640.CharybdisJimmy.digit.RDO.v12003108_tid006066._00038.pool.root.4"]
    # --- event with low lumi pile up (4.6 min bias, 2*10^33), should be ATLAS-CSC-01-02-00
    #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/user/k/kgrybel/pileup/CSC/005850.pyt_WH120_munubb/digits/misal1_csc11.005850.WH120bb_pythia.digit.RDOS.v12003103_lumi02._00001.pool.root"]
    
if doReadBS:
    # Collisions Data!
    ServiceMgr.ByteStreamInputSvc.FullFileName = InDetAlignExampleFlags.Files
    #ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/afs/cern.ch/user/g/gencomm/w0/Collisions/MergeBS.run140541.LArAndMBTSInTime._0001.data"]
    # input used for FDR2b (run 8): low lumi (2.3 min bias, 10^33)/pile-up/shifted vertex run 8
    # this file needs (as set by default): DetDescrVersion = "ATLAS-CSC-02-00-00" globalflags.ConditionsTag="OFLCOND-FDR-02-01-00", jobproperties.Beam.numberOfCollisions = 2.3
    #ServiceMgr.ByteStreamInputSvc.FullFileName = [ "/afs/cern.ch/atlas/maxidisk/d158/daq.fdr08_run2.0052300.physics.Express.LB0001._sfo02._0001.data" ]
    # if you create the input BS file yourself with InDetWriteBS jobO the output will be this file
    # and you have to set the detdescr and cond tag to what you used ...
    #ServiceMgr.ByteStreamInputSvc.FullFileName = [ "daq.csc13.0000000.Single.Stream.LB0000.Athena._0001.data" ]

    
