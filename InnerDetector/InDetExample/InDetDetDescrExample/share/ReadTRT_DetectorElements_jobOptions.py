###############################################################
#
# Loads Detector Description for the TRT
# and prints out various quanities for each detector element.
#
###############################################################


# Use Global flags and DetFlags.
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

# Just the TRT
DetFlags.detdescr.TRT_setOn()

# MC or data - affects which conditions database instance is used
globalflags.DataSource='geant4'
#globalflags.DataSource='data'

# Select the geometry version. 
globalflags.DetDescrVersion='ATLAS-GEO-08-00-00'

# For misalignments
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-CSC-00-01-00')

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# Use MagneticFieldSvc
include( "BFieldAth/BFieldAth_jobOptions.py" )

# Set up the job.
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Load algorithms

from InDetDetDescrExample.InDetDetDescrExampleConf import ReadTRT_DetectorElements

job += ReadTRT_DetectorElements()
print job.ReadTRT_DetectorElements

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
job.ReadTRT_DetectorElements = INFO

# To set global output level use 
# athena -l DEBUG

# Number of events to be processed (default is 10)
theApp.EvtMax = 1

###############################################################
