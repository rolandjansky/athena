from PartPropSvc.PartPropSvcConf import PartPropSvc

include("ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include("EventAthenaPool/EventAthenaPool_joboptions.py")

#----------------------------
# Input Dataset
#----------------------------
import os
from glob import glob
#----------------------------
# Algorithms
#----------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from RDOAnalysis.RDOAnalysisConf import *

topSequence += TRT_RDOAnalysis(), SCT_RDOAnalysis(), PixelRDOAnalysis(), BCM_RDOAnalysis(), TileRDOAnalysis(), LArRDOAnalysis(), MDT_RDOAnalysis(), CSC_RDOAnalysis(), TGC_RDOAnalysis(), RPC_RDOAnalysis()

TRT_RDOAnalysis = TRT_RDOAnalysis()
TRT_RDOAnalysis.NtupleFileName = '/TRT_RDOAnalysis/'
TRT_RDOAnalysis.HistPath = '/TRT_RDOAnalysis/'

SCT_RDOAnalysis = SCT_RDOAnalysis()
SCT_RDOAnalysis.NtupleFileName = '/SCT_RDOAnalysis/'
SCT_RDOAnalysis.HistPath = '/SCT_RDOAnalysis/'

PixelRDOAnalysis = PixelRDOAnalysis()
PixelRDOAnalysis.NtupleFileName = '/PixelRDOAnalysis/'
PixelRDOAnalysis.HistPath = '/PixelRDOAnalysis/'

BCM_RDOAnalysis = BCM_RDOAnalysis()
BCM_RDOAnalysis.NtupleFileName = '/BCM_RDOAnalysis/'
BCM_RDOAnalysis.HistPath = '/BCM_RDOAnalysis/'

TileRDOAnalysis = TileRDOAnalysis()
TileRDOAnalysis.NtupleFileName = '/TileRDOAnalysis/'
TileRDOAnalysis.HistPath = '/TileRDOAnalysis/'

LArRDOAnalysis = LArRDOAnalysis()
LArRDOAnalysis.NtupleFileName = '/LArRDOAnalysis/'
LArRDOAnalysis.HistPath = '/LArRDOAnalysis/'

MDT_RDOAnalysis = MDT_RDOAnalysis()
MDT_RDOAnalysis.NtupleFileName = '/MDT_RDOAnalysis/'
MDT_RDOAnalysis.HistPath = '/MDT_RDOAnalysis/'

CSC_RDOAnalysis = CSC_RDOAnalysis()
CSC_RDOAnalysis.NtupleFileName = '/CSC_RDOAnalysis/'
CSC_RDOAnalysis.HistPath = '/CSC_RDOAnalysis/'

TGC_RDOAnalysis = TGC_RDOAnalysis()
TGC_RDOAnalysis.NtupleFileName = '/TGC_RDOAnalysis/'
TGC_RDOAnalysis.HistPath = '/TGC_RDOAnalysis/'

RPC_RDOAnalysis = RPC_RDOAnalysis()
RPC_RDOAnalysis.NtupleFileName = '/RPC_RDOAnalysis/'
RPC_RDOAnalysis.HistPath = '/RPC_RDOAnalysis/'

#----------------------------
# Histogram and Tree Service
#----------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["TRT_RDOAnalysis DATAFILE='TRT_RDOAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["SCT_RDOAnalysis DATAFILE='SCT_RDOAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["PixelRDOAnalysis DATAFILE='PixelRDOAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["BCM_RDOAnalysis DATAFILE='BCM_RDOAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["TileRDOAnalysis DATAFILE='TileRDOAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["LArRDOAnalysis DATAFILE='LArRDOAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["MDT_RDOAnalysis DATAFILE='MDT_RDOAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["CSC_RDOAnalysis DATAFILE='CSC_RDOAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["TGC_RDOAnalysis DATAFILE='TGC_RDOAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["RPC_RDOAnalysis DATAFILE='RPC_RDOAnalysis.root' OPT='RECREATE'"]


#---------------------------
# Detector geometry
#---------------------------
from RecExConfig.AutoConfiguration import *
ConfigureFieldAndGeo()
include("RecExCond/AllDet_detDescr.py")

import MuonCnvExample.MuonCablingConfig

