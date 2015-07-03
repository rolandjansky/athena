#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# --- read BS 
doReadBS =  True
doCopyRDO = False

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if not doReadBS:
  athenaCommonFlags.FilesInput = ["/afs/cern.ch/work/m/mdiamond/public/digiOUT/OUTdigi1247_2.pool.root"]
  # top no pileup (default)
#  athenaCommonFlags.FilesInput = ["root://eosatlas///eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc12_valid/ac/91/RDO.01385812._000001.pool.root.1"]
  # H-gamgam, 20 pileup
  # athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlasgroupdisk/perf-egamma/dq2/mc11_valid/RDO/e850_s1310_s1300_d576/mc11_valid.106384.PythiaH120gamgam.digit.RDO.e850_s1310_s1300_d576_tid512888_00/RDO.512888._000001.pool.root.1"]
  # H-gamgam, 0 pileup (filename ?)
  # athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlasgroupdisk/perf-egamma/dq2/mc11_valid/RDO/e850_s1310_s1300_d578/mc11_valid.106384.PythiaH120gamgam.digit.RDO.e850_s1310_s1300_d578_tid512902_00/RDO.512902._000001.pool.root.1"]
  # electrons 5 GeV
  # athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105128.singlepart_e_pt5.digit.RDO.e605_s933_s946_d369/RDO.219312._000024.pool.root.2"]
  # pions 1 GeV
  # athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105130.singlepart_singlepi_pt1.digit.RDO.e605_s933_s946_d369/RDO.197118._000005.pool.root.1"]

if doReadBS:
  #athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/data10_7TeV.00154817.physics_MinBias.merge.RAW/data10_7TeV.00154817.physics_MinBias.merge.RAW._lb0100._0001.1"]
  athenaCommonFlags.FilesInput = ["/afs/cern.ch/work/m/mdiamond/public/data15_cos.00253010.physics_Main.merge.RAW._lb0878._SFO-ALL._0001.1"]
  #athenaCommonFlags.FilesInput = ["/tmp/mdiamond/data15_cos.00251873.physics_IDCosmic.merge.RAW._lb0002._SFO-ALL._0001.1", "/tmp/mdiamond/data15_cos.00251873.physics_IDCosmic.merge.RAW._lb0003._SFO-ALL._0001.1", "/tmp/mdiamond/data15_cos.00251873.physics_IDCosmic.merge.RAW._lb0004._SFO-ALL._0001.1", "/tmp/mdiamond/data15_cos.00251873.physics_IDCosmic.merge.RAW._lb0005._SFO-ALL._0001.1"]

#from AthenaCommon.GlobalFlags import jobproperties
#jobproperties.Global.ConditionsTag='OFLCOND-SDR-BS14T-IBL-CORR-06-02'
#from IOVDbSvc.CondDB import conddb
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v19s0_ATLAS-R2-2015-02')
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v15_ATLAS-IBL3D25-04-00-01')
#conddb.setGlobalTag(globalflags.ConditionsTag())
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v15_ATLAS-IBL3D25-04-00-01')
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v19s0_ATLAS-R2-2015-02')


  
import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetRecExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

#from AthenaCommon.GlobalFlags import jobproperties
#jobproperties.Global.ConditionsTag='OFLCOND-SDR-BS14T-IBL-CORR-06-02'
#from IOVDbSvc.CondDB import conddb
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v15_ATLAS-IBL3D25-04-00-01')
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v19s0_ATLAS-R2-2015-02')
#conddb.setGlobalTag(globalflags.ConditionsTag())
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v19s0_ATLAS-R2-2015-02')
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterial','AtlasLayerMat_v15_ATLAS-IBL3D25-04-00-01')



if doReadBS:
  from AthenaCommon.GlobalFlags import globalflags
  if len(globalflags.ConditionsTag())!=0:
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())
    conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v19s0_ATLAS-R2-2015-02')

#include("RecExCond/AllDet_detDescr.py")

import MagFieldServices.SetupField     

theApp.EvtMax = -1
#theApp.EvtMax = 20

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
doWriteESD      = True
doWriteAOD      = False
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
# Additional Detector Setup
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec
rec.Commissioning=False

# --- for heavy ion running
#rec.doHeavyIon = True

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
#DetFlags.makeRIO.pixel_setOff()
DetFlags.makeRIO.SCT_setOff()
DetFlags.makeRIO.TRT_setOff()

# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# ---- switch parts of ID off/on as follows (always use both lines)
#DetFlags.pixel_setOff()
#DetFlags.detdescr.pixel_setOn()
DetFlags.SCT_setOff()
DetFlags.detdescr.SCT_setOn()
DetFlags.TRT_setOff()
DetFlags.detdescr.TRT_setOn()

# --- switch off DCS
#DetFlags.dcs.pixel_setOff()
DetFlags.dcs.SCT_setOff()
DetFlags.dcs.TRT_setOff()

# --- printout
#DetFlags.Print()

# --- output level
#OutputLevel          = DEBUG

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
#InDetFlags.doTruth       = (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')
InDetFlags.doTruth = False
InDetFlags.doDBM = True

# --- special setup for Beate
# InDetFlags.doVtxLumi = True 

# --- enable xKalman and iPatRec
# InDetFlags.doxKalman              = True
# InDetFlags.doiPatRec              = True

# --- uncomment to change the default of one of the following options:
#InDetFlags.doNewTracking          = False
InDetFlags.doLowPt                = False
#InDetFlags.doBeamGas              = True
#InDetFlags.doBeamHalo             = True
#InDetFlags.doBackTracking         = False
#InDetFlags.doTRTStandalone        = False
#InDetFlags.doLowBetaFinder        = False
InDetFlags.doTRTPhaseCalculation = False

#InDetFlags.cutLevel.set_Value_and_Lock                             (10)

InDetFlags.cutLevel       = 1 # test for TRTonly in PU
#InDetFlags.priVtxCutLevel = 2 # test for PU

# --- enable brem recovery
# InDetFlags.doBremRecovery   = True

# --- calo seeding does not work standalone 
# InDetFlags.doCaloSeededBrem = False

# --- Turn off track slimming
InDetFlags.doSlimming = False

InDetFlags.doIBL = True
#InDetFlags.doHighPileup = True
#InDetFlags.doSLHC = True

# --- possibility to run tracking on subdetectors separately (and independent from each other)
#InDetFlags.doTrackSegmentsPixel = True
#InDetFlags.doTrackSegmentsSCT   = True
#InDetFlags.doTrackSegmentsTRT   = True

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'

# --- activate monitorings
InDetFlags.doMonitoringGlobal    = False
InDetFlags.doMonitoringPrimaryVertexingEnhanced = False
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

# --- activate (memory/cpu) monitoring
#InDetFlags.doPerfMon        = True
# --- activate creation of standard plots
InDetFlags.doStandardPlots  = True
# --- activate physics validation monitoring
InDetFlags.doPhysValMon  = False
# --- active storegate delection
#InDetFlags.doSGDeletion  = True

# --- produce various ntuples (all in one root file)
InDetFlags.doTrkNtuple      = False
InDetFlags.doPixelTrkNtuple = False
#InDetFlags.doSctTrkNtuple   = True
#InDetFlags.doTrtTrkNtuple   = True
InDetFlags.doPixelClusterNtuple = False
#InDetFlags.doSctClusterNtuple   = True
#InDetFlags.doTrtDriftCircleNtuple = True
#InDetFlags.doVtxNtuple      = True
#InDetFlags.doConvVtxNtuple  = True
#InDetFlags.doV0VtxNtuple    = True

#InDetFlags.doMinBias   = True

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables = True

#DBM changes
InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False)
InDetFlags.doNewTracking.set_Value_and_Lock(True)
InDetFlags.useBroadPixClusterErrors.set_Value_and_Lock(True)
InDetFlags.selectSCTIntimeHits = False
InDetFlags.doForwardTracks = False
InDetFlags.doVertexFinding = True
InDetFlags.doSCT_PRDFormation = False
InDetFlags.doTRT_PRDFormation = False
InDetFlags.doHolesOnTrack = False
InDetFlags.doTIDE_Ambi = False
InDetFlags.useBeamConstraint = False
InDetFlags.doCaloSeededBrem = False
InDetFlags.doVertexFindingForMonitoring = False
InDetFlags.doSplitVertexFindingForMonitoring = False
InDetFlags.doParticleCreation = False
#InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True);
#InDetFlags.doFatras = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")
include("RecExCommon/BSRead_config.py")
include("IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py")
include("PixelCabling/SelectPixelMap.py")

# --- dump MC truth into logfile
#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC(McEventKey = "TruthEvent")
