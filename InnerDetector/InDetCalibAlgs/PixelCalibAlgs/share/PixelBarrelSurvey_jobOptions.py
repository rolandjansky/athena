#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# initialization of DB services
IOVDbSvc=Service("IOVDbSvc")
IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=mycool.db;X:TESTDB"
include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
regSvc = Service( "IOVRegistrationSvc" )
regSvc.RecreateFolders = False

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
# GlobalFlags.DetGeo.set_ctbh8()
# GlobalFlags.DetGeo.set_commis()
GlobalFlags.DataSource.set_geant4()
# GlobalFlags.DataSource.set_data()

DetDescrVersion = "ATLAS-CSC-01-00-00"
from AthenaCommon.DetFlags import DetFlags 
DetFlags.pixel_setOn()
include("AtlasGeoModel/SetGeometryVersion.py" )
include("AtlasGeoModel/GeoModelInit.py" )
GeoModelSvc.InDetVersionOverride="InnerDetector-DC3-07"

#load relevant libraries
theApp.Dlls += [ "PixelCalibAlgs" ]

# Example of PixelBarrelSurvey algorithm configuration
from PixelCalibAlgs.PixelCalibAlgsConf import PixelBarrelSurvey
MyAlg = PixelBarrelSurvey()
# MyAlg.OldFileFormat = True
# MyAlg.SurveyFile = "MySurveyFile.txt"
# MyAlg.MappingFile = "NamingConversions.txt"
# MyAlg.OutputFile = "MyOutputFile.txt"
# MyAlg.DistoFile = "MyDistoFile.txt"
# MyAlg.COOL_Tag   = "testtag"
print MyAlg

#top algorithms to be run
theApp.TopAlg += [ "PixelBarrelSurvey" ]

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

MessageSvc.OutputLevel = INFO

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
 
