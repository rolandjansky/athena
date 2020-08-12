from AthenaCommon.Constants import *
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
DetDescrVersion = 'ATLAS-P2-ITK-22-02-00'
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = DetDescrVersion

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.InDetBuildingOutputLevel = VERBOSE

include("InDetSLHC_Example/preInclude.SiliconOnly.py")
include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup.py")
include("InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py")

from AtlasGeoModel import SetGeometryVersion 
from AtlasGeoModel import GeoModelInit 

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
ServiceMgr += GeoModelSvc
GeoModelSvc.AtlasVersion = DetDescrVersion

#----------------------------
# Input Dataset
#----------------------------
import os
from glob import glob
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
FILENAME = "ttbar.RDO.pool.root"
athenaCommonFlags.FilesInput = [FILENAME]
#athenaCommonFlags.FilesInput = ['/afs/cern.ch/work/c/ctreado/RDOAnalysis/datasets/mc15_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.RDO.e4032_s2608_s2183_r6790_tid05706569_00/RDO.05706569._000011.pool.root.1']
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

#theApp.EvtMax = 10000
#theApp.EvtMax = 100
theApp.EvtMax = -1

#----------------------------
# Message Service
#----------------------------
# set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999

#----------------------------
# Algorithms
#----------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from RDOAnalysis.RDOAnalysisConf import PixelRDOAnalysis
topSequence += PixelRDOAnalysis()
PixelRDOAnalysis = PixelRDOAnalysis()
PixelRDOAnalysis.NtupleFileName = '/PixelRDOAnalysis/'
PixelRDOAnalysis.HistPath = '/PixelRDOAnalysis/'
PixelRDOAnalysis.DoITk=True

#from RDOAnalysis.RDOAnalysisConf import SCT_RDOAnalysis
#topSequence += SCT_RDOAnalysis()
#SCT_RDOAnalysis = SCT_RDOAnalysis()
#SCT_RDOAnalysis.NtupleFileName = '/SCT_RDOAnalysis/'
#SCT_RDOAnalysis.HistPath = '/SCT_RDOAnalysis/'
#SCT_RDOAnalysis.DoITk=True

#----------------------------
# Histogram and Tree Service
#----------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["PixelRDOAnalysis DATAFILE='PixelRDOAnalysis.root' OPT='RECREATE'"]
#ServiceMgr.THistSvc.Output += ["SCT_RDOAnalysis DATAFILE='SCT_RDOAnalysis.root' OPT='RECREATE'"]

#---------------------------
# Detector geometry
#---------------------------
from RecExConfig.AutoConfiguration import *
ConfigureFieldAndGeo()
include("RecExCond/AllDet_detDescr.py")

include("InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py")
include("InDetSLHC_Example/postInclude.SLHC_Setup.py")

