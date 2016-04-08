# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ToolSvc

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
globalflags.DetDescrVersion = 'ATLAS-P2-ITK-01-00-00'

# import the # the conditions setup
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')
       
# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# --- setup version

# to load the data file named slhc_blablabla_BeamPipe.txt
SLHC_Version = "SLHC-BeamPipe"
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.SLHC_Version = SLHC_Version


print  SLHC_Flags

# Start VP1
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup_BeamPipe import SLHC_Setup
SLHC_Setup = SLHC_Setup()

#--------------------------------------------------------------
# To display the tracking geometry
#--------------------------------------------------------------
# --- setup of tracking geometry
## if True :
##   from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
##   TrkDetFlags.SLHC_Geometry                   = True
##   TrkDetFlags.PixelBuildingOutputLevel        = VERBOSE
##   TrkDetFlags.SCT_BuildingOutputLevel         = VERBOSE
##   TrkDetFlags.TRT_BuildingOutputLevel         = VERBOSE
##   TrkDetFlags.MagneticFieldCallbackEnforced   = False
##   TrkDetFlags.TRT_BuildStrawLayers            = False
##   TrkDetFlags.MaterialDatabaseLocal           =  SLHC_Flags.SLHC_Version() != '' 
##   TrkDetFlags.MaterialStoreGateKey            = '/GLOBAL/TrackingGeo/LayerMaterialITK'
##   TrkDetFlags.MaterialTagBase                 = 'AtlasLayerMat_v'
##   TrkDetFlags.MaterialVersion                 = 17
##   if TrkDetFlags.MaterialDatabaseLocal() is True :
##      TrkDetFlags.MaterialMagicTag                = jobproperties.Global.DetDescrVersion()
##      TrkDetFlags.MaterialDatabaseLocalPath    = ''
##      TrkDetFlags.MaterialDatabaseLocalName    = 'AtlasLayerMaterial-'+SLHC_Flags.SLHC_Version()+'.db'
##   TrkDetFlags.MagneticFieldCallbackEnforced         = False

# VP1 setup
from VP1Algs.VP1AlgsConf import VP1Alg
VP1Alg.InitiallyLoadedVP1Files = [ "BasicVisualisationGeoModel.vp1" ]  ## Extra line from Sabine
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


