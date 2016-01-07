###############################################################
#
# Job options file to test reading distortions
#
#==============================================================


from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.DetFlags import DetFlags


#GlobalFlags.DetGeo.set_ctbh8()
GlobalFlags.DetGeo.set_atlas()
#GlobalFlags.DetGeo.set_commis()

GlobalFlags.DataSource.set_geant4()
#GlobalFlags.DataSource.set_data()

############################################################################################
#
#  Set up conditions DB folders

from IOVDbSvc.CondDB import conddb
conddb.addFolder('INDET','/Indet/PixelDist')
#conddb.setGlobalTag('OFLCOND-CSC-01-00-00') # No distortions
conddb.setGlobalTag('OFLCOND-CSC-01-01-00')  # Survey distortions

#conddb.addOverride('/Indet/PixelDist','newfoldertag')
#conddb.addOverride('/Indet/PixelDist','InDetPixelDist-000-00')   # Survey distortions
#conddb.addOverride('/Indet/PixelDist','InDetPixelDist-nominal')  # No distortions

# Override with local sqlite file:
#conddb.blockFolder('/Indet/PixelDist')
#conddb.addFolderWithTag('LOCAL','/Indet/PixelDist','InDetPixelDist-test', force=True)

#
###########################################################################################

# Just the pixel and SCT
DetFlags.detdescr.pixel_setOn()

# Select the geometry version.
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion='ATLAS-GEO-10-00-00'

# Initialize geometry
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion



# Load algorithms Any algorithm that uses the tool will do
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from PixelConditionsTools.PixelConditionsToolsConf import PixelDistortionsTestReadWrite
topSequence += PixelDistortionsTestReadWrite()
topSequence.PixelDistortionsTestReadWrite.FullTest = True
topSequence.PixelDistortionsTestReadWrite.OutputLevel = DEBUG

from PixelConditionsTools.PixelConditionsToolsConf import PixelDistortionsTool
ToolSvc += PixelDistortionsTool()
#ToolSvc.PixelDistortionsTool.OutputLevel = VERBOSE
ToolSvc.PixelDistortionsTool.OutputLevel = DEBUG
#ToolSvc.PixelDistortionsTool.InputSource = 0  # None
#ToolSvc.PixelDistortionsTool.InputSource = 1  # Constant
#ToolSvc.PixelDistortionsTool.InputSource = 2  # Read from text file
#ToolSvc.PixelDistortionsTool.InputSource = 3  # Random
#ToolSvc.PixelDistortionsTool.InputSource = 4  # Read from DB (Default)
#ToolSvc.PixelDistortionsTool.TextFileName = "PixelSurveyDistortions.txt"

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = INFO
MessageSvc.defaultLimit = 99999999
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
