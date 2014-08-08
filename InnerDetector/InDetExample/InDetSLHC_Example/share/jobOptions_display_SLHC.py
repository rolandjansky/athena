#--------------------------------------------------------------
# Template jobOptions: SLHC 
#  - Display
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

# MC or data - affects which conditions database instance is used
globalflags.DataSource='geant4'
#globalflags.DataSource='data'


#
#==============================================================
# Load Detector Description for Inner Detector.
#==============================================================
#

DetFlags.detdescr.ID_setOn()
DetFlags.detdescr.TRT_setOff()

# Select the geometry version. 
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = 'ATLAS-SLHC-01-00-00'

# import the # the conditions setup
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')
       
# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# --- setup version
#SLHC_Version = "SLHC-08-03"
# Fully DB version
SLHC_Version = ""

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.SLHC_Version = SLHC_Version

print  SLHC_Flags

#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

# Start VP1
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# To display the tracking geometry
#--------------------------------------------------------------
# --- setup of tracking geometry
if True :
  from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
  TrkDetFlags.SLHC_Geometry                   = True
  TrkDetFlags.PixelBuildingOutputLevel        = VERBOSE
  TrkDetFlags.SCT_BuildingOutputLevel         = VERBOSE
  TrkDetFlags.TRT_BuildingOutputLevel         = VERBOSE
  TrkDetFlags.MagneticFieldCallbackEnforced   = False
  TrkDetFlags.TRT_BuildStrawLayers            = False
  TrkDetFlags.MaterialFromCool                = True
  TrkDetFlags.MaterialDatabaseLocal           =  SLHC_Flags.SLHC_Version() != '' and TrkDetFlags.MaterialFromCool()
  TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/SLHC_LayerMaterial'
  TrkDetFlags.MaterialTagBase                 = 'SLHC_LayerMat_v'
  TrkDetFlags.MaterialVersion                 = 6
  if TrkDetFlags.MaterialDatabaseLocal() is True :
     TrkDetFlags.MaterialMagicTag                = SLHC_Flags.SLHC_Version()
     TrkDetFlags.MaterialDatabaseLocalPath    = ''
     TrkDetFlags.MaterialDatabaseLocalName    = 'SLHC_LayerMaterial-'+SLHC_Flags.SLHC_Version()+'.db'
  TrkDetFlags.MagneticFieldCallbackEnforced         = False
  TrkDetFlags.LArUseMaterialEffectsOnTrackProvider  = False
  TrkDetFlags.TileUseMaterialEffectsOnTrackProvider = False



# VP1 setup
from VP1Algs.VP1AlgsConf import VP1Alg
topSequence += VP1Alg()

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
# To set global output level use 
# athena -l DEBUG

#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1
#--------------------------------------------------------------

#--------------------------------------------------------------
# To read in pool file (HITS or RDOs) uncomment the following as appropriate
#--------------------------------------------------------------
#import AthenaPoolCnvSvc.ReadAthenaPool
#from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#svcMgr.EventSelector.InputCollections=["singleMuon_test_slhc_Hits.pool.root"]
#svcMgr.EventSelector.InputCollections=["singleMuon_test_slhc_Digits.pool.root"]
#theApp.EvtMax = -1

