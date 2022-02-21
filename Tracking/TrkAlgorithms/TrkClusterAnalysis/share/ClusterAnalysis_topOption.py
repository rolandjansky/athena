#   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

ServiceMgr.AuditorSvc.Auditors += ["ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

from PartPropSvc.PartPropSvcConf import PartPropSvc

include("ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include("EventAthenaPool/EventAthenaPool_joboptions.py")

# build GeoModel
detDescrVersion = "ATLAS-P2-ITK-23-00-03"
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = detDescrVersion

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.InDetBuildingOutputLevel = VERBOSE

include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SiliconOnly.py")

include("InDetSLHC_Example/preInclude.SLHC_Setup.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py")

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
ServiceMgr += GeoModelSvc
GeoModelSvc.AtlasVersion = detDescrVersion

#----------------------------
# Input Dataset
#----------------------------
import os
from glob import glob
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = ["./21.9/ESD.pool.root"]
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

theApp.EvtMax = 300

#----------------------------
# Message Service
#----------------------------
# set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
ServiceMgr.MessageSvc.OutputLevel = INFO
#ServiceMgr.MessageSvc.defaultLimit = 9999999

#----------------------------
# Algorithm
#----------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TrkClusterAnalysis.TrkClusterAnalysisConf import PixelClusterAnalysis
pixelClusterAnalysis = PixelClusterAnalysis()
pixelClusterAnalysis.NtupleFileName = '/PixelClusterAnalysis/'
pixelClusterAnalysis.HistPath = '/PixelClusterAnalysis/'
topSequence += pixelClusterAnalysis

from TrkClusterAnalysis.TrkClusterAnalysisConf import SCT_ClusterAnalysis
sctClusterAnalysis = SCT_ClusterAnalysis()
sctClusterAnalysis.NtupleFileName = '/SCT_ClusterAnalysis/'
sctClusterAnalysis.HistPath = '/SCT_ClusterAnalysis/'
topSequence += sctClusterAnalysis

#----------------------------
# Histogram and Tree Service
#----------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["PixelClusterAnalysis DATAFILE='PixelClusterAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["SCT_ClusterAnalysis DATAFILE='SCT_ClusterAnalysis.root' OPT='RECREATE'"]

#---------------------------
# Detector geometry
#---------------------------
from RecExConfig.AutoConfiguration import ConfigureFieldAndGeo
ConfigureFieldAndGeo()
include("RecExCond/AllDet_detDescr.py")

include("InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py")

#from AthenaCommon.ConfigurationShelve import saveToAscii
#saveToAscii("config.cluster.analysis.txt")
