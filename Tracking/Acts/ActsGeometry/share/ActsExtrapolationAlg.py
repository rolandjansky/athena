###############################################################
#
# Loads Detector Description for Pixel and SCT
# and prints out various quanities for each detector element.
#
###############################################################

import os
import logging

# Use Global flags and DetFlags.
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
from AthenaCommon.Logging import log as msg
nThreads = jp.ConcurrencyFlags.NumThreads()
if (nThreads < 1) :
    msg.fatal('numThreads must be >0. Did you set the --threads=N option?')
    sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

# Just the pixel and SCT
DetFlags.ID_setOn()
DetFlags.detdescr.pixel_setOn()
DetFlags.detdescr.SCT_setOn()


# MC or data - affects which conditions database instance is used
globalflags.DataSource='geant4'
#globalflags.DataSource='data'

# Select the geometry version. 
globalflags.DetDescrVersion = 'ATLAS-R2-2016-00-00-00'

# Initialize geometry
# THIS ACTUALLY DOES STUFF!!
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# For misalignments
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-SIM-00-00-00')
conddb.addOverride("/Indet/Align", "InDetAlign_R2_Nominal")

## SET UP ALIGNMENT CONDITIONS ALGORITHM
from IOVSvc.IOVSvcConf import CondSvc
svcMgr += CondSvc( OutputLevel=INFO )
from ActsGeometry import ActsGeometryConf
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
condSeq += ActsGeometryConf.NominalAlignmentCondAlg("NominalAlignmentCondAlg",
                                                     OutputLevel=VERBOSE)
## END OF CONDITIONS SETUP


from AthenaCommon.AppMgr import ServiceMgr
from ActsGeometry.ActsGeometryConfig import ActsTrackingGeometrySvc

trkGeomSvc = ActsTrackingGeometrySvc()

# skip material map input on CI job
matMapFile = "MaterialMaps.pb"

if os.path.exists(matMapFile):
  trkGeomSvc.UseMaterialMap = True
  trkGeomSvc.MaterialMapInputFile = "MaterialMaps.pb"
  logging.info("Material map from %s configured", matMapFile)
else:
  logging.info("Material map file at %s not found, do not configure", matMapFile)

trkGeomSvc.BarrelMaterialBins = [40, 60] # phi z
trkGeomSvc.EndcapMaterialBins = [50, 20] # phi r
trkGeomSvc.OutputLevel = INFO
ServiceMgr += trkGeomSvc

exCellWriterSvc = CfgMgr.ActsExCellWriterSvc("ExCellWriterSvc")
exCellWriterSvc.FilePath = "excells_charged.root"
ServiceMgr += exCellWriterSvc

import MagFieldServices.SetupField

# setup the McEventSelector
import AthenaCommon.AtlasUnixGeneratorJob
svcMgr.EventSelector.FirstEvent=0
svcMgr.EventSelector.RunNumber=1
svcMgr.EventSelector.InitialTimeStamp=0
svcMgr.EventSelector.TimeStampInterval=1
svcMgr.EventSelector.FirstLB=1
svcMgr.EventSelector.EventsPerLB = 100

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.EvtMax = 1

from ActsGeometry.ActsGeometryConf import ActsExtrapolationAlg

alg = ActsExtrapolationAlg()
alg.OutputLevel = INFO
# these options will move to the ExtrapolationTool at some point
alg.SearchMode = 1
alg.CollectSensitive = True
alg.CollectPassive = True
alg.CollectBoundary = True
alg.CollectMaterial = True
alg.StopAtBoundary = True
alg.FATRAS = True
alg.nParticles = 1
alg.Cardinality = 0

alg.WriteMaterialTracks = True
# we only need this if the extrap alg is set up to write mat tracks
mTrackWriterSvc = CfgMgr.ActsMaterialTrackWriterSvc("ActsMaterialTrackWriterSvc")
mTrackWriterSvc.OutputLevel = DEBUG
mTrackWriterSvc.FilePath = "MaterialTracks_mapped.root"
ServiceMgr += mTrackWriterSvc

exTool = CfgMgr.ActsExtrapolationTool("ActsExtrapolationTool")
exTool.OutputLevel = INFO
exTool.FieldMode = "ATLAS"
exTool.MatEngineDoELossCorrection = False

trkGeomTool = CfgMgr.ActsTrackingGeometryTool("ActsTrackingGeometryTool")
trkGeomTool.OutputLevel = INFO;
exTool.TrackingGeometryTool = trkGeomTool

alg.ExtrapolationTool = exTool

eventPrintFrequency = 100
if hasattr(ServiceMgr,"AthenaEventLoopMgr"):
    ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = eventPrintFrequency
if hasattr(ServiceMgr,"AthenaHiveEventLoopMgr"):
    ServiceMgr.AthenaHiveEventLoopMgr.EventPrintoutInterval = eventPrintFrequency

job += alg


theApp.EvtMax = 1234
