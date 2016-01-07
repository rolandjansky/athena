###############################################################
#
# Job options file to read distortions from text file and output
# a new pool file and sqlite file
#
#==============================================================

outputFile = "pixeldist_test.root"
objectList = [ "DetCondCFloat#/Indet/PixelDist"]
tagList = ["InDetPixelDist-test"]

#==============================================================

from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.DetFlags import DetFlags


#GlobalFlags.DetGeo.set_ctbh8()
GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DetGeo.set_commis()

GlobalFlags.DataSource.set_geant4()
#GlobalFlags.DataSource.set_data()

#include("IOVDbSvc/CondDBSetup.py")
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-CSC-01-00-00')  # Not important which global tag, but need to specify something

# Just the pixel and SCT
DetFlags.detdescr.pixel_setOn()
#DetFlags.detdescr.SCT_setOn()

# Select the geometry version.
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion='ATLAS-GEO-10-00-00'

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion


# This line can be excluded and it will by default connect to SQlite file mycool.db
# IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=mycool.db;X:OFLP200"
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )


from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg(outputFile=outputFile)
myOCA.ObjectList=objectList
myOCA.IOVTagList=tagList

# Load algorithms Any algorithm that uses the tool will do
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from PixelConditionsTools.PixelConditionsToolsConf import PixelDistortionsTestReadWrite
topSequence += PixelDistortionsTestReadWrite()
topSequence.PixelDistortionsTestReadWrite.FullTest = False

from PixelConditionsTools.PixelConditionsToolsConf import PixelDistortionsTool
ToolSvc += PixelDistortionsTool()
ToolSvc.PixelDistortionsTool.OutputLevel = VERBOSE
ToolSvc.PixelDistortionsTool.InputSource = 2  # Read from text file
ToolSvc.PixelDistortionsTool.TextFileName = "PixelSurveyDistortions.txt"
#ToolSvc.PixelDistortionsTool.TextFileName = "PixelSurveyDistortions_test.txt"
ToolSvc.PixelDistortionsTool.WriteData=True

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = INFO
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1

#==============================================================
#
# End of job options file
#
###############################################################
