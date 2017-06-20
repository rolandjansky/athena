#from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

from PartPropSvc.PartPropSvcConf import PartPropSvc

include("ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include("EventAthenaPool/EventAthenaPool_joboptions.py" )

import os
from glob import glob
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput = glob( "/tmp/" + os.environ['USER'] + "/" + "HITS*root*" )
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput() # This is stupid and redundant, but necessary

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from HitAnalysis.HitAnalysisConf import CaloHitAnalysis
topSequence += CaloHitAnalysis()
CaloHitAnalysis = CaloHitAnalysis()
CaloHitAnalysis.NtupleFileName = '/CaloHitAnalysis/'
CaloHitAnalysis.HistPath = '/CaloHitAnalysis/'

#ExpertMode adds more histograms to the output. Default mode is off
CaloHitAnalysis.ExpertMode = "off"
#CalibHits adds Calibrated hits histograms to the output. Default mode is off
CaloHitAnalysis.CalibHits = "off"

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["CaloHitAnalysis DATAFILE='CaloHitAnalysis.root' OPT='RECREATE'" ]

ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999

theApp.EvtMax = -1

ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

# To set up a geometry
from RecExConfig.AutoConfiguration import *
ConfigureFieldAndGeo() # Configure the settings for the geometry
include("RecExCond/AllDet_detDescr.py") # Actually load the geometry
#include( "TrkDetDescrSvc/AtlasTrackingGeometrySvc.py" ) # Tracking geometry, handy for ID work
