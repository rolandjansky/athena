###############################################################
#
# Loads Detector Description for Pixel and SCT
# and prints out various quanities for each detector element.
#
###############################################################


# Use Global flags and DetFlags.
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

# Just the pixel and SCT
DetFlags.detdescr.pixel_setOn()
DetFlags.detdescr.SCT_setOn()

# MC or data - affects which conditions database instance is used
globalflags.DataSource='geant4'
#globalflags.DataSource='data'

# Select the geometry version. 
globalflags.DetDescrVersion = 'ATLAS-GEO-08-00-00'

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# For misalignments
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-CSC-00-01-00')

# Use MagneticFieldSvc
include( "BFieldAth/BFieldAth_jobOptions.py" )


from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Load algorithms

from InDetDetDescrExample.InDetDetDescrExampleConf import PrintSiElements

job += PrintSiElements()
#job.PrintSiElements.ModulesOnly = True
#job.PrintSiElements.ExpandId = True
#job.PrintSiElements.NominalPosition = True
#job.PrintSiElements.AlignedPosition = False
#job.PrintSiElements.FullRotationMatrix = False
#job.PrintSiElements.OutputFile = "geometry.dat"
print job.PrintSiElements


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
job.PrintSiElements.OutputLevel = INFO


# To set global output level use 
# athena -l DEBUG

# Number of events to be processed (default is 10)
theApp.EvtMax = 1

###############################################################
