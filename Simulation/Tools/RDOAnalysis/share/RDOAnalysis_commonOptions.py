from PartPropSvc.PartPropSvcConf import PartPropSvc

include("ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include("EventAthenaPool/EventAthenaPool_joboptions.py")

#----------------------------
# Input Dataset
#----------------------------
import os
from glob import glob

from AthenaCommon import CfgMgr
#----------------------------
# Histogram and Tree Service
#----------------------------
ServiceMgr += CfgMgr.THistSvc()

#----------------------------
# Algorithms
#----------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.DetFlags import DetFlags
if DetFlags.TRT_on():
    topSequence += CfgMgr.TRT_RDOAnalysis()
    topSequence.TRT_RDOAnalysis.NtupleFileName = '/TRT_RDOAnalysis/'
    topSequence.TRT_RDOAnalysis.HistPath = '/TRT_RDOAnalysis/'
    ServiceMgr.THistSvc.Output += ["TRT_RDOAnalysis DATAFILE='TRT_RDOAnalysis.root' OPT='RECREATE'"]
if DetFlags.SCT_on():
    topSequence += CfgMgr.SCT_RDOAnalysis()
    topSequence.SCT_RDOAnalysis.NtupleFileName = '/SCT_RDOAnalysis/'
    topSequence.SCT_RDOAnalysis.HistPath = '/SCT_RDOAnalysis/'
    ServiceMgr.THistSvc.Output += ["SCT_RDOAnalysis DATAFILE='SCT_RDOAnalysis.root' OPT='RECREATE'"]
if DetFlags.pixel_on():
    topSequence += CfgMgr.PixelRDOAnalysis()
    topSequence.PixelRDOAnalysis.NtupleFileName = '/PixelRDOAnalysis/'
    topSequence.PixelRDOAnalysis.HistPath = '/PixelRDOAnalysis/'
    ServiceMgr.THistSvc.Output += ["PixelRDOAnalysis DATAFILE='PixelRDOAnalysis.root' OPT='RECREATE'"]
if DetFlags.BCM_on():
    topSequence += CfgMgr.BCM_RDOAnalysis()
    topSequence.BCM_RDOAnalysis.NtupleFileName = '/BCM_RDOAnalysis/'
    topSequence.BCM_RDOAnalysis.HistPath = '/BCM_RDOAnalysis/'
    ServiceMgr.THistSvc.Output += ["BCM_RDOAnalysis DATAFILE='BCM_RDOAnalysis.root' OPT='RECREATE'"]
if DetFlags.Tile_on():
    topSequence += CfgMgr.TileRDOAnalysis()
    topSequence.TileRDOAnalysis.NtupleFileName = '/TileRDOAnalysis/'
    topSequence.TileRDOAnalysis.HistPath = '/TileRDOAnalysis/'
    ServiceMgr.THistSvc.Output += ["TileRDOAnalysis DATAFILE='TileRDOAnalysis.root' OPT='RECREATE'"]
if DetFlags.LAr_on():
    topSequence += CfgMgr.LArRDOAnalysis()
    topSequence.LArRDOAnalysis.NtupleFileName = '/LArRDOAnalysis/'
    topSequence.LArRDOAnalysis.HistPath = '/LArRDOAnalysis/'
    ServiceMgr.THistSvc.Output += ["LArRDOAnalysis DATAFILE='LArRDOAnalysis.root' OPT='RECREATE'"]
if DetFlags.MDT_on():
    topSequence += CfgMgr.MDT_RDOAnalysis()
    topSequence.MDT_RDOAnalysis.NtupleFileName = '/MDT_RDOAnalysis/'
    topSequence.MDT_RDOAnalysis.HistPath = '/MDT_RDOAnalysis/'
    ServiceMgr.THistSvc.Output += ["MDT_RDOAnalysis DATAFILE='MDT_RDOAnalysis.root' OPT='RECREATE'"]
if DetFlags.CSC_on():
    topSequence += CfgMgr.CSC_RDOAnalysis()
    topSequence.CSC_RDOAnalysis.NtupleFileName = '/CSC_RDOAnalysis/'
    topSequence.CSC_RDOAnalysis.HistPath = '/CSC_RDOAnalysis/'
    ServiceMgr.THistSvc.Output += ["CSC_RDOAnalysis DATAFILE='CSC_RDOAnalysis.root' OPT='RECREATE'"]
if DetFlags.TGC_on():
    topSequence += CfgMgr.TGC_RDOAnalysis()
    topSequence.TGC_RDOAnalysis.NtupleFileName = '/TGC_RDOAnalysis/'
    topSequence.TGC_RDOAnalysis.HistPath = '/TGC_RDOAnalysis/'
    ServiceMgr.THistSvc.Output += ["TGC_RDOAnalysis DATAFILE='TGC_RDOAnalysis.root' OPT='RECREATE'"]
if DetFlags.RPC_on():
    topSequence += CfgMgr.RPC_RDOAnalysis()
    topSequence.RPC_RDOAnalysis.NtupleFileName = '/RPC_RDOAnalysis/'
    topSequence.RPC_RDOAnalysis.HistPath = '/RPC_RDOAnalysis/'
    ServiceMgr.THistSvc.Output += ["RPC_RDOAnalysis DATAFILE='RPC_RDOAnalysis.root' OPT='RECREATE'"]


#---------------------------
# Detector geometry
#---------------------------
from RecExConfig.AutoConfiguration import *
ConfigureFieldAndGeo()
include("RecExCond/AllDet_detDescr.py")

import MuonCnvExample.MuonCablingConfig

