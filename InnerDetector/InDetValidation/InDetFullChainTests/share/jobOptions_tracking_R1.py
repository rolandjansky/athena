#--------------------------------------------------------------
# Template jobOptions: IBL 
#  - Tracking 
#--------------------------------------------------------------

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# --- read BS
doReadBS        = False

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if not doReadBS:
  athenaCommonFlags.FilesInput = ['singleMuon_R1_RDO.pool.root']

import MagFieldServices.SetupField

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetIBLExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

if doReadBS:
  from AthenaCommon.GlobalFlags import globalflags
  if len(globalflags.ConditionsTag())!=0:
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())

theApp.EvtMax = 3

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
doWriteAOD      = True
# --- do auditors ?
doAuditors      = True

if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True 
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False

#--------------------------------------------------------------
# Additional Detector setup
#--------------------------------------------------------------

# # --- select the geometry version to be IBL default 
# from AthenaCommon.GlobalFlags import globalflags
# globalflags.DetDescrVersion = 'ATLAS-IBL-01-00-00'
# 
# 
# from AthenaCommon.BeamFlags import jobproperties
# # --- default is high luminosity for IBL
# jobproperties.Beam.numberOfCollisions = 1.0

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

#DetFlags.makeRIO.pixel_setOff()
#DetFlags.makeRIO.SCT_setOff()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags

# --- geoemetry

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth = (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')

InDetFlags.doTrkNtuple.set_Value_and_Lock(True)
InDetFlags.doTrkD3PD.set_Value_and_Lock(True)
# --- uncomment to change the default of one of the following options:
#OutputLevel          = VERBOSE

# --- Set output names such that they work with Reco_trf.py
from InDetRecExample.InDetKeys import InDetKeys
if athenaCommonFlags.PoolESDOutput(): InDetKeys.OutputESDFileName = athenaCommonFlags.PoolESDOutput()
if athenaCommonFlags.PoolAODOutput(): InDetKeys.OutputAODFileName = athenaCommonFlags.PoolAODOutput()
if InDetFlags.doTrkNtuple():          InDetKeys.trkValidationNtupleName = 'myTrkValidation_singleMuon_IBL.root'
if InDetFlags.doTrkD3PD():            InDetKeys.trkD3PDFileName.set_Value_and_Lock('InDetTrackD3PD.pool.root')


#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include ("RecExCommon/RecExCommon_topOptions.py")


