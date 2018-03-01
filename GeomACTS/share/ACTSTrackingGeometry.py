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
DetFlags.ID_setOn()
DetFlags.detdescr.pixel_setOn()
DetFlags.detdescr.SCT_setOn()


# MC or data - affects which conditions database instance is used
globalflags.DataSource='geant4'
#globalflags.DataSource='data'

# Select the geometry version. 
globalflags.DetDescrVersion = 'ATLAS-R2-2016-00-00-00'

# print "HERE"
# print globalflags.DetDescrVersion

# Initialize geometry
# THIS ACTUALLY DOES STUFF!!
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# For misalignments
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')
conddb.addOverride("/Indet/Align", "InDetAlign_R2_Nominal")

# Use MagneticFieldSvc
# include( "BFieldAth/BFieldAth_jobOptions.py" )


from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Load algorithms

# from InDetDetDescrExample.InDetDetDescrExampleConf import ACTSTrackingGeometry

# job += ACTSTrackingGeometry()
#job.ACTSTrackingGeometry.ModulesOnly = True
#job.ACTSTrackingGeometry.ExpandId = True
#job.ACTSTrackingGeometry.NominalPosition = True
#job.ACTSTrackingGeometry.AlignedPosition = False
#job.ACTSTrackingGeometry.FullRotationMatrix = False
#job.ACTSTrackingGeometry.OutputFile = "geometry.dat"
# print job.ACTSTrackingGeometry

# include("GeneratorUtils/StdEvgenSetup.py")
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.EvtMax = 1

from GeomACTS.GeomACTSConf import ACTSTrackingGeometry

alg = ACTSTrackingGeometry()
alg.OutputLevel = DEBUG
job += alg

from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::VolumeDebuggerTool',['BeginOfRun'])

# from AthenaCommon.CfgGetter import getPublicTool
# getPublicTool('G4UA::VolumeDebuggerTool', tryDefaultConfigurable=True).OutputPath="./test.gdml"
# getPublicTool('G4UA::VolumeDebuggerTool').TargetVolume='IDET::IDET'
# getPublicTool('G4UA::VolumeDebuggerTool').Resolution=1000000 # Default = 1000$
# getPublicTool('G4UA::VolumeDebuggerTool').Tolerance=0.000001
# getPublicTool('G4UA::VolumeDebuggerTool').Verbose=True
# getPublicTool('G4UA::VolumeDebuggerTool').RunGeoTest=False

# from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
# job += PyG4AtlasAlg()

# #--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
# job.ACTSTrackingGeometry.OutputLevel = INFO


# To set global output level use 
# athena -l DEBUG

# Number of events to be processed (default is 10)
theApp.EvtMax = 1

###############################################################
