###############################################################
#
# Loads Detector Description for Pixel and SCT
# and prints out various quanities for each detector element.
#
###############################################################


# Use Global flags and DetFlags.
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()

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


from AthenaCommon.AppMgr import ServiceMgr
from GeomACTS.GeomACTSConfig import TrackingGeometrySvc
ServiceMgr += TrackingGeometrySvc()
ServiceMgr += CfgMgr.Acts__ExCellWriterSvc("ExCellWriterSvc")

import MagFieldServices.SetupField

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Load algorithms
# include("GeneratorUtils/StdEvgenSetup.py")
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.EvtMax = 1

from GeomACTS.GeomACTSConf import ACTSTrackingGeometry

alg = ACTSTrackingGeometry()
alg.nParticles = 1
alg.Cardinality = nThreads

exTool = CfgMgr.Acts__ExtrapolationTool("ExtrapolationTool")
alg.ExtrapolationTool = exTool

objWriterTool = CfgMgr.Acts__ObjWriterTool("ObjWriterTool")
objWriterTool.OutputDirectory = "./"
objWriterTool.SubDetectors = [
    # "Pixel", 
    # "SCT",
    "TRT",
]
alg.ObjWriterTool = objWriterTool


# alg.OutputLevel = VERBOSE
job += alg


# job.ACTSTrackingGeometry.OutputLevel = INFO


# To set global output level use 
# athena -l DEBUG

# Number of events to be processed (default is 10)
theApp.EvtMax = 123400

###############################################################
