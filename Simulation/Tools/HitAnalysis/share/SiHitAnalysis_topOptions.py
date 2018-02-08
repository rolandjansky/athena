#from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

from PartPropSvc.PartPropSvcConf import PartPropSvc

include("ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include("EventAthenaPool/EventAthenaPool_joboptions.py" )

#Use these lines if the NSW is included in the simulation
#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc = GeoModelSvc()
#GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.07.00-NSW"

import os
from glob import glob
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput = glob( "/tmp/"+os.environ['USER']+"/"+"HITS*root*" )
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput() # This is stupid and redundant, but necessary

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from HitAnalysis.HitAnalysisConf import SiHitAnalysis
topSequence += SiHitAnalysis('PixelHitAnalysis')
topSequence.PixelHitAnalysis.CollectionName='PixelHits'
topSequence += SiHitAnalysis('SCTHitAnalysis')
topSequence.SCTHitAnalysis.CollectionName='SCT_Hits'
topSequence += SiHitAnalysis('BCMHitAnalysis')
topSequence.BCMHitAnalysis.CollectionName='BCMHits'
topSequence += SiHitAnalysis('BLMHitAnalysis')
topSequence.BLMHitAnalysis.CollectionName='BLMHits'

topSequence.PixelHitAnalysis.HistPath='/SiHitAnalysis/histos/'
topSequence.SCTHitAnalysis.HistPath='/SiHitAnalysis/histos/'
topSequence.BCMHitAnalysis.HistPath='/SiHitAnalysis/histos/'
topSequence.BLMHitAnalysis.HistPath='/SiHitAnalysis/histos/'

topSequence.PixelHitAnalysis.NtupleFileName='/SiHitAnalysis/ntuples/'
topSequence.SCTHitAnalysis.NtupleFileName='/SiHitAnalysis/ntuples/'
topSequence.BCMHitAnalysis.NtupleFileName='/SiHitAnalysis/ntuples/'
topSequence.BLMHitAnalysis.NtupleFileName='/SiHitAnalysis/ntuples/'


#Add some more TH2 histograms
topSequence.PixelHitAnalysis.ExpertMode= "off"
topSequence.SCTHitAnalysis.ExpertMode= "off"
topSequence.BCMHitAnalysis.ExpertMode= "off"
topSequence.BLMHitAnalysis.ExpertMode= "off"

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "SiHitAnalysis DATAFILE='SiHitAnalysis.root' OPT='RECREATE'" ]

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
#include("TrkDetDescrSvc/AtlasTrackingGeometrySvc.py" ) # Tracking geometry, handy for ID work
