# Job option to just load InDet only
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

# MC or data - affects which conditions database instance is used
globalflags.DataSource='geant4'
#globalflags.DataSource='data'

# Needed for CTB geometry only
#globalflags.DetGeo='ctbh8' # default is atlas

#
#==============================================================
# Load Detector Description for Inner Detector.
#==============================================================
#

DetFlags.detdescr.ID_setOn()
#DetFlags.detdescr.all_setOn()
#DetFlags.detdescr.pixel_setOn()
#DetFlags.detdescr.SCT_setOn()
#DetFlags.detdescr.TRT_setOn()

# Select the geometry version. 
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = 'ATLAS-GEO-08-00-00'

#globalflags.DetDescrVersion = 'ATLAS-GEO-02-01-00'
#globalflags.DetDescrVersion = 'ATLAS-CSC-01-02-00'
#globalflags.DetDescrVersion = 'CTB'

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# For misalignments
# To use the same as was used in the simulation comment this out.
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-CSC-00-00-00')
#conddb.setGlobalTag('OFLCOND-CSC-00-01-00')

## Override subsystem tags.
#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc.PixelVersionOverride="Pixel-GEO-00"
#GeoModelSvc.SCT_VersionOverride="SCT-GEO-00"
#GeoModelSvc.TRT_VersionOverride="TRT-GEO-00"
#GeoModelSvc.InDetVersionOverride="InnerDetector-GEO-Dev"

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
# To set global output level use 
# athena -l DEBUG

#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
#--------------------------------------------------------------
