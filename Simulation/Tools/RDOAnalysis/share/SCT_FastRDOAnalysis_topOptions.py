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

#----------------------------
# Input Dataset
#----------------------------
import os
from glob import glob
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = glob("/tmp/" + os.environ['USER'] + "/" + "RDO*fastdigi*.root*")
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

theApp.EvtMax = -1
#theApp.EvtMax = 100

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
from RDOAnalysis.RDOAnalysisConf import SCT_FastRDOAnalysis
topSequence += SCT_FastRDOAnalysis()
SCT_FastRDOAnalysis = SCT_FastRDOAnalysis()
SCT_FastRDOAnalysis.NtupleFileName = '/SCT_FastRDOAnalysis/'
SCT_FastRDOAnalysis.HistPath = '/SCT_FastRDOAnalysis/'

#----------------------------
# Histogram and Tree Service
#----------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["SCT_FastRDOAnalysis DATAFILE='SCT_FastRDOAnalysis.root' OPT='RECREATE'"]

#----------------------------
# Detector geometry
#----------------------------
from RecExConfig.AutoConfiguration import *
ConfigureFieldAndGeo()
include("RecExCond/AllDet_detDescr.py")
