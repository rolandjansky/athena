#--------------------------------------------------------------
# Template jobOptions: IBL 
#  - Tracking 
#--------------------------------------------------------------

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if not athenaCommonFlags.PoolRDOInput.isDefault():
  athenaCommonFlags.FilesInput = athenaCommonFlags.PoolRDOInput()
elif not athenaCommonFlags.PoolESDInput.isDefault():
  athenaCommonFlags.FilesInput = athenaCommonFlags.PoolESDInput()  

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetIBLExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

from AthenaCommon.GlobalFlags import globalflags
if len(globalflags.ConditionsTag())!=0:
  from IOVDbSvc.CondDB import conddb
  conddb.setGlobalTag(globalflags.ConditionsTag())

theApp.EvtMax = athenaCommonFlags.EvtMax()

#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

from RecExConfig.RecFlags import rec

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = rec.doWriteESD()
doWriteAOD      = rec.doWriteAOD()
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
# --- and switch on/off all the rest
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()

#DetFlags.makeRIO.pixel_setOff()
#DetFlags.makeRIO.SCT_setOff()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags

# --- geometry
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
from InDetRecExample.InDetKeys import InDetKeys
InDetFlags.doTruth = (globalflags.InputFormat() == 'pool')

if (hasattr(InDetFlags,"doPixelClusterSplitting")) :
  InDetFlags.doPixelClusterSplitting=False

# --- do track validation Ntuple
if hasattr(runArgs,"outputNTUP_TRKVALIDFile"):
  InDetFlags.doTrkNtuple.set_Value_and_Lock(True) 
  InDetKeys.trkValidationNtupleName.set_Value_and_Lock(runArgs.outputNTUP_TRKVALIDFile) 

# --- do tracking D3PD
if hasattr(runArgs,"outputDESDM_TRACKFile"):
  InDetFlags.doTrkD3PD.set_Value_and_Lock(True)
  InDetKeys.trkD3PDFileName.set_Value_and_Lock(runArgs.outputDESDM_TRACKFile)

# --- uncomment to change the default of one of the following options:
#OutputLevel          = VERBOSE

# --- Set output names such that they work with Reco_trf.py
if athenaCommonFlags.PoolESDOutput(): InDetKeys.OutputESDFileName = athenaCommonFlags.PoolESDOutput()
if athenaCommonFlags.PoolAODOutput(): InDetKeys.OutputAODFileName = athenaCommonFlags.PoolAODOutput()

#--------------------------------------------------------------
# configure IBL reco
#--------------------------------------------------------------

include("InDetIBL_Example/InDetIBLRecoPreInclude.py")

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")

#--------------------------------------------------------------
# configure IBL reco
#--------------------------------------------------------------

include("InDetIBL_Example/InDetIBLRecoPostInclude.py")
