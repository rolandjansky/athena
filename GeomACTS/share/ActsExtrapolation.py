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

trkGeomSvc = TrackingGeometrySvc()
trkGeomSvc.OutputLevel = INFO
ServiceMgr += trkGeomSvc

exCellWriterSvc = CfgMgr.Acts__ExCellWriterSvc("ExCellWriterSvc")
exCellWriterSvc.FilePath = "excells_charged.root"
ServiceMgr += exCellWriterSvc

import MagFieldServices.SetupField

import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Load algorithms
# include("GeneratorUtils/StdEvgenSetup.py")
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.EvtMax = 1

from GeomACTS.GeomACTSConf import ActsExtrapolation

alg = ActsExtrapolation()
alg.OutputLevel = INFO
alg.SearchMode = 1
alg.CollectSensitive = True
alg.CollectPassive = True
alg.CollectBoundary = True
alg.CollectMaterial = True
alg.StopAtBoundary = True
alg.FATRAS = True
alg.nParticles = 1
alg.Cardinality = 0

exTool = CfgMgr.Acts__ExtrapolationTool("ExtrapolationTool")
exTool.OutputLevel = INFO
alg.ExtrapolationTool = exTool


# alg.OutputLevel = VERBOSE
job += alg


# job.ActsExtrapolation.OutputLevel = INFO


# To set global output level use 
# athena -l DEBUG

# Number of events to be processed (default is 10)
theApp.EvtMax = 123400

###############################################################
