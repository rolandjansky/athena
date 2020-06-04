"""
This job options file will run an example extrapolation using the
Acts tracking geometry, the material map and the Acts extrapolation toolchain.
"""

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
cfg = apcl.AutoCfg(name = 'MaterialMapValidation', input_files=athenaCommonFlags.FilesInput())

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


# Initialize geometry
# THIS ACTUALLY DOES STUFF!!
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

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

# nominal alignment: all deltas are identity
# condSeq += ActsGeometryConf.NominalAlignmentCondAlg("NominalAlignmentCondAlg",
                                                     # OutputLevel=VERBOSE)

condSeq += ActsGeometryConf.ActsAlignmentCondAlg("ActsAlignCondAlg",
                                                 OutputLevel=INFO)
# periodic shift alignment. Configurable z-shift per lumiblock.
# (currently pixel only)
# condSeq+=ActsGeometryConf.GeomShiftCondAlg("GeomShiftCondAlg_1",
                                            # ZShiftPerLB=0.5,
                                            # OutputLevel=VERBOSE)
## END OF CONDITIONS SETUP

from AthenaCommon.AppMgr import ServiceMgr

# set up and configure the acts geometry construction
from ActsGeometry.ActsGeometryConf import ActsTrackingGeometrySvc
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
trkGeomSvc.UseMaterialMap = True
trkGeomSvc.MaterialMapInputFile = "material-maps.json"
ServiceMgr += trkGeomSvc

# We need the Magnetic fiels
import MagFieldServices.SetupField

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# This is the main extrapolation demo algorithm
from ActsGeometry.ActsGeometryConf import ActsExtrapolationAlg
alg = ActsExtrapolationAlg()
alg.EtaRange = [-2.4, 2.4]
alg.OutputLevel = INFO
alg.NParticlesPerEvent = int(1e4)


# Record the material track for material map validation
alg.WriteMaterialTracks = True
# we only need this if the extrap alg is set up to write mat tracks
if alg.WriteMaterialTracks == True:
  mTrackWriterSvc = CfgMgr.ActsMaterialTrackWriterSvc("ActsMaterialTrackWriterSvc")
  mTrackWriterSvc.OutputLevel = INFO
  mTrackWriterSvc.FilePath = "MaterialTracks_mapped.root"
  ServiceMgr += mTrackWriterSvc

# sets up the extrapolation tool
# this sets up the tracking geometry svc through the tracking geometry tool
exTool = CfgMgr.ActsExtrapolationTool("ActsExtrapolationTool")
exTool.OutputLevel = INFO
exTool.FieldMode = "ATLAS"
exTool.InteractionMultiScatering = True
exTool.InteractionEloss = True
exTool.InteractionRecord = True
# The extrapolation tool accesses the trackinggeometry service
# through this tool. This tool has the conditions dependencies
# on the alignment GeoAlignmentStores (pseudo-alignment only right now).
# For each event, the GAS for the IOV needs to be set from the algorithm.
trkGeomTool = CfgMgr.ActsTrackingGeometryTool("ActsTrackingGeometryTool")
trkGeomTool.OutputLevel = INFO;
exTool.TrackingGeometryTool = trkGeomTool

alg.ExtrapolationTool = exTool

# Make the event heardbeat output a bit nicer
eventPrintFrequency = 10000
if hasattr(ServiceMgr,"AthenaEventLoopMgr"):
    ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = eventPrintFrequency
if hasattr(ServiceMgr,"AthenaHiveEventLoopMgr"):
    ServiceMgr.AthenaHiveEventLoopMgr.EventPrintoutInterval = eventPrintFrequency

job += alg

theApp.EvtMax = 10000
