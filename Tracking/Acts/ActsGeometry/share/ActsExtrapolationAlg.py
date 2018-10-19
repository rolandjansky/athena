"""
This job options file will run an example extrapolation using the
Acts tracking geometry and the Acts extrapolation toolchain.
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

# Just the pixel and SCT
DetFlags.ID_setOn()
DetFlags.detdescr.pixel_setOn()
DetFlags.detdescr.SCT_setOn()

# MC or data - affects which conditions database instance is used
globalflags.DataSource='geant4'

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

# nominal alignment: all deltas are identity
condSeq += ActsGeometryConf.NominalAlignmentCondAlg("NominalAlignmentCondAlg",
                                                     OutputLevel=VERBOSE)

# periodic shift alignment. Configurable z-shift per lumiblock.
# (currently pixel only)
# condSeq+=ActsGeometryConf.GeomShiftCondAlg("GeomShiftCondAlg_1",
                                            # ZShiftPerLB=0.5,
                                            # OutputLevel=VERBOSE)
## END OF CONDITIONS SETUP


from AthenaCommon.AppMgr import ServiceMgr

# set up and configure the acts geometry construction
from ActsGeometry.ActsGeometryConfig import ActsTrackingGeometrySvc
trkGeomSvc = ActsTrackingGeometrySvc()
# used for the proxies during material mapping
trkGeomSvc.BarrelMaterialBins = [40, 60] # phi z
trkGeomSvc.EndcapMaterialBins = [50, 20] # phi r
trkGeomSvc.OutputLevel = INFO
ServiceMgr += trkGeomSvc


# We need the Magnetic fiels
import MagFieldServices.SetupField

# setup the McEventSelector
# This enables pseudo LBs. Required for the conditions algorithms,
# and thus the whole extrapolation alg to work
import AthenaCommon.AtlasUnixGeneratorJob
svcMgr.EventSelector.FirstEvent=0
svcMgr.EventSelector.RunNumber=1
svcMgr.EventSelector.InitialTimeStamp=0
svcMgr.EventSelector.TimeStampInterval=1
svcMgr.EventSelector.FirstLB=1
svcMgr.EventSelector.EventsPerLB = 100

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.EvtMax = 1

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()


# This is the main extrapolation demo algorithm
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

# not really needed right now
# this can be used to test an input material map file
alg.WriteMaterialTracks = False
# we only need this if the extrap alg is set up to write mat tracks
if alg.WriteMaterialTracks == True:
  mTrackWriterSvc = CfgMgr.ActsMaterialTrackWriterSvc("ActsMaterialTrackWriterSvc")
  mTrackWriterSvc.OutputLevel = DEBUG
  mTrackWriterSvc.FilePath = "MaterialTracks_mapped.root"
  ServiceMgr += mTrackWriterSvc

# sets up the extrapolation tool
# this sets up the tracking geometry svc through the tracking geometry tool
exTool = CfgMgr.ActsExtrapolationTool("ActsExtrapolationTool")
exTool.OutputLevel = INFO
exTool.FieldMode = "ATLAS"

# The extrapolation tool accesses the trackinggeometry service
# through this tool. This tool has the conditions dependencies
# on the alignment GeoAlignmentStores (pseudo-alignment only right now).
# For each event, the GAS for the IOV needs to be set from the algorithm.
trkGeomTool = CfgMgr.ActsTrackingGeometryTool("ActsTrackingGeometryTool")
trkGeomTool.OutputLevel = INFO;
exTool.TrackingGeometryTool = trkGeomTool

alg.ExtrapolationTool = exTool

# Make the event heardbeat output a bit nicer
eventPrintFrequency = 100
if hasattr(ServiceMgr,"AthenaEventLoopMgr"):
    ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = eventPrintFrequency
if hasattr(ServiceMgr,"AthenaHiveEventLoopMgr"):
    ServiceMgr.AthenaHiveEventLoopMgr.EventPrintoutInterval = eventPrintFrequency

job += alg

theApp.EvtMax = 1234
