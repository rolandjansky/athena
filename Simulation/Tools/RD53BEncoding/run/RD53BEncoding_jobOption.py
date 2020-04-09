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

#detDescrVersion = "ATLAS-P2-ITK-22-02-00"
detDescrVersion = "ATLAS-P2-ITK-22-00-00"

# build GeoModel
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = detDescrVersion

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.InDetBuildingOutputLevel = VERBOSE

include("InDetSLHC_Example/preInclude.SLHC.py")
include("InDetSLHC_Example/preInclude.SiliconOnly.py")

SLHC_Flags.LayoutOption = 'InclinedAlternative'
include("InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py")

#include("InDetSLHC_Example/preInclude.SLHC_Setup.py")
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
athenaCommonFlags.FilesInput = glob("/eos/atlas/atlascerngroupdisk/perf-idtracking/21.9/RDO/user.ncalace.Step3p1.ttbar.mu200/RDO.user.ncalace.Step3p1.ttbar.mu200.*.pool.root")   
print "CHECK FilesInput --> ", athenaCommonFlags.FilesInput()

ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

#theApp.EvtMax = -1
theApp.EvtMax = 10
#theApp.EvtMax = 50
#theApp.EvtMax = 20
#theApp.EvtMax = 120
#theApp.EvtMax = 100
#theApp.EvtMax = doEvents

#----------------------------
# Message Service
#----------------------------
# set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999

#----------------------------
# Tools
#----------------------------
from RD53BEncoding.RD53BEncodingConf import RD53BEncodingTool
from AthenaCommon.AppMgr import ToolSvc

encodingCompBTNS1=RD53BEncodingTool("encodingCompBTNS1")
encodingCompBTNS1.DoExpertPlots       =True
encodingCompBTNS1.EventsPerStream     =1
encodingCompBTNS1.AuroraFactor        =1.04
encodingCompBTNS1.Path                ="/Validation/encodingCompBTNS1/"
encodingCompBTNS1.DoToTSuppression    =False
encodingCompBTNS1.TestStreamFileName  ="TestStream_compBTNS1"
encodingCompBTNS1.TestStreamCreation  =False
encodingCompBTNS1.Debug               =False
encodingCompBTNS1.OutputLevel         =INFO
ToolSvc += encodingCompBTNS1

encodingTools = [encodingCompBTNS1]

#----------------------------
# Algorithm
#----------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from RD53BEncoding.RD53BEncodingConf import RD53BEncodingAlg
encodingAlg = RD53BEncodingAlg()
encodingAlg.Use50x50 = True
encodingAlg.RD53BEncodingTools=encodingTools
encodingAlg.Path="/Validation/"
encodingAlg.OutputLevel=INFO
topSequence += RD53BEncodingAlg()

#----------------------------
# Histogram and Tree Service
#----------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["Validation DATAFILE='RD53BEncoding.root' OPT='RECREATE'"]

#---------------------------
# Detector geometry
#---------------------------
from RecExConfig.AutoConfiguration import *
ConfigureFieldAndGeo()
include("RecExCond/AllDet_detDescr.py")

include("InDetSLHC_Example/postInclude.SLHC_Setup_InclBrl_4.py")
#include("InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py")

from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.reco.txt")
