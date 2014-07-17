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

# Select the geometry version. 
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = 'ATLAS-IBL-01-00-00'

#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc.InDetVersionOverride="InnerDetector-IBL-01"

# import the # the conditions setup
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')
       
# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# --- setup version
from InDetIBL_Example.SLHC_JobProperties import SLHC_Flags
## Leave commented out unless overriding with text file.
## Default is to use Geom DB only
#SLHC_Flags.SLHC_Version = "IBL-01"

print  SLHC_Flags

#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetIBL_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()

# Start VP1
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

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
#svcMgr.EventSelector.InputCollections=["singleMuon_IBL_Hits.pool.root"]
#svcMgr.EventSelector.InputCollections=["singleMuon_IBL_Digits.pool.root"]
#theApp.EvtMax = -1
