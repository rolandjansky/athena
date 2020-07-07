import os
import logging

# Use Global flags and DetFlags.
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
from AthenaCommon.Logging import log as msg
nThreads = jp.ConcurrencyFlags.NumThreads()
# for some reason, the synchronization fails if we run in ST...
if (nThreads < 1) :
    msg.fatal('numThreads must be >0. Did you set the --threads=N option?')
    sys.exit(AthenaCommon.ExitCodes.CONFIGURATION_ERROR)

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = [
    "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/esd/100evts10lumiblocks.ESD.root"
]

import AthenaPoolCnvSvc.ReadAthenaPool

# build GeoModel
import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'TrackingGeometryTest', input_files=athenaCommonFlags.FilesInput())

cfg.configure_job()

from AthenaCommon.GlobalFlags import globalflags
if len(globalflags.ConditionsTag())!=0:
  from IOVDbSvc.CondDB import conddb
  conddb.setGlobalTag(globalflags.ConditionsTag())

from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
InDetGeometryFlags.useDynamicAlignFolders=True

# Just the pixel and SCT
DetFlags.ID_setOn()
DetFlags.Calo_setOn()

from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion
from SubDetectorEnvelopes.SubDetectorEnvelopesConfig import getEnvelopeDefSvc
svcMgr += getEnvelopeDefSvc()


from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.OutputLevel( INFO )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.EnableConditions( True )
AlgScheduler.setDataLoaderAlg( "SGInputLoader" )

## SET UP ALIGNMENT CONDITIONS ALGORITHM
from IOVSvc.IOVSvcConf import CondSvc
svcMgr += CondSvc( OutputLevel=INFO )
from ActsGeometry import ActsGeometryConf
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

condSeq += ActsGeometryConf.ActsAlignmentCondAlg("ActsAlignCondAlg",
                                                 OutputLevel=VERBOSE)
## END OF CONDITIONS SETUP


from AthenaCommon.AppMgr import ServiceMgr

# set up and configure the acts geometry construction
from ActsGeometry.ActsGeometryConfig import ActsTrackingGeometrySvc
trkGeomSvc = ActsTrackingGeometrySvc()
# used for the proxies during material mapping
trkGeomSvc.BarrelMaterialBins = [40, 60] # phi z
trkGeomSvc.EndcapMaterialBins = [50, 20] # phi r
trkGeomSvc.OutputLevel = INFO
trkGeomSvc.BuildSubDetectors = [
  "Pixel",
  "SCT",
  # "TRT",
  # "Calo",
]
ServiceMgr += trkGeomSvc

import MagFieldServices.SetupField

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

trkGeomTool = CfgMgr.ActsTrackingGeometryTool("ActsTrackingGeometryTool")
trkGeomTool.OutputLevel = INFO;

# from ActsGeometry.ActsGeometryConf import ActsWriteTrackingGeometryTransforms
# alg = ActsWriteTrackingGeometryTransforms(OutputLevel = VERBOSE)
# alg.TrackingGeometryTool = trkGeomTool
# job += alg

mActsMaterialJsonWriterTool = CfgMgr.ActsMaterialJsonWriterTool("ActsMaterialJsonWriterTool")
mActsMaterialJsonWriterTool.OutputLevel = VERBOSE
mActsMaterialJsonWriterTool.FilePath = "geometry-maps.json"

from ActsGeometry.ActsGeometryConf import ActsWriteTrackingGeometry
alg = ActsWriteTrackingGeometry(OutputLevel = VERBOSE)
alg.TrackingGeometryTool = trkGeomTool
alg.ObjWriterTool.OutputDirectory = "obj"
alg.ObjWriterTool.SubDetectors = ["Pixel", "SCT"]
alg.MaterialJsonWriterTool = mActsMaterialJsonWriterTool

job += alg


# Make the event heardbeat output a bit nicer
eventPrintFrequency = 100
if hasattr(ServiceMgr,"AthenaEventLoopMgr"):
    ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = eventPrintFrequency
if hasattr(ServiceMgr,"AthenaHiveEventLoopMgr"):
    ServiceMgr.AthenaHiveEventLoopMgr.EventPrintoutInterval = eventPrintFrequency


theApp.EvtMax = 1
