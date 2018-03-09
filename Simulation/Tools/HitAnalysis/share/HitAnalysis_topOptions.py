#from AthenaCommon.AppMgr import ToolSvc
#from AthenaCommon.AppMgr import ServiceMgr
#import AthenaPoolCnvSvc.ReadAthenaPool

from PartPropSvc.PartPropSvcConf import PartPropSvc

include("ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include("EventAthenaPool/EventAthenaPool_joboptions.py" )

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.07.00-NSW"

import os
from glob import glob
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from HitAnalysis.HitAnalysisConf import *

topSequence += SiHitAnalysis('PixelHitAnalysis')
topSequence.PixelHitAnalysis.CollectionName='PixelHits'
topSequence += SiHitAnalysis('SCTHitAnalysis')
topSequence.SCTHitAnalysis.CollectionName='SCT_Hits'
topSequence += SiHitAnalysis('BCMHitAnalysis')
topSequence.BCMHitAnalysis.CollectionName='BCMHits'
topSequence += SiHitAnalysis('BLMHitAnalysis')
topSequence.BLMHitAnalysis.CollectionName='BLMHits'
topSequence += TRTHitAnalysis('TRTHitAnalysis')
topSequence += RPCHitAnalysis('RPCHitAnalysis')
topSequence += MDTHitAnalysis('MDTHitAnalysis')
topSequence += CSCHitAnalysis('CSCHitAnalysis')
topSequence += TGCHitAnalysis('TGCHitAnalysis')
#topSequence += ALFAHitAnalysis('ALFAHitAnalysis')
#topSequence += LucidHitAnalysis('LucidHitAnalysis')
#topSequence += ZDCHitAnalysis('ZDCHitAnalysis')
topSequence += TrackRecordAnalysis('TrackRecordAnalysis')
topSequence += TruthHitAnalysis('TruthHitAnalysis')
topSequence += CaloHitAnalysis('CaloHitAnalysis')
topSequence += MMHitAnalysis('MMHitAnalysis')
topSequence += sTGCHitAnalysis('sTGCHitAnalysis')


topSequence.PixelHitAnalysis.HistPath='/HitAnalysis/'
topSequence.SCTHitAnalysis.HistPath='/HitAnalysis/'
topSequence.BCMHitAnalysis.HistPath='/HitAnalysis/'
topSequence.BLMHitAnalysis.HistPath='/HitAnalysis/'
topSequence.TRTHitAnalysis.HistPath='/HitAnalysis/'
topSequence.RPCHitAnalysis.HistPath='/HitAnalysis/'
topSequence.MDTHitAnalysis.HistPath='/HitAnalysis/'
topSequence.CSCHitAnalysis.HistPath='/HitAnalysis/'
topSequence.TGCHitAnalysis.HistPath='/HitAnalysis/'
#topSequence.ALFAHitAnalysis.HistPath='/HitAnalysis/'
#topSequence.LucidHitAnalysis.HistPath='/HitAnalysis/'
#topSequence.ZDCHitAnalysis.HistPath='/HitAnalysis/'
topSequence.TrackRecordAnalysis.HistPath='/HitAnalysis/'
topSequence.TruthHitAnalysis.HistPath = '/HitAnalysis/'
topSequence.CaloHitAnalysis.HistPath = '/HitAnalysis/'
topSequence.MMHitAnalysis.HistPath='/HitAnalysis/'
topSequence.sTGCHitAnalysis.HistPath='/HitAnalysis/'
#Add some more TH2 histograms


topSequence.PixelHitAnalysis.ExpertMode= "off"
topSequence.SCTHitAnalysis.ExpertMode= "off"
topSequence.BCMHitAnalysis.ExpertMode= "off"
topSequence.BLMHitAnalysis.ExpertMode= "off"
topSequence.CaloHitAnalysis.ExpertMode = "off"
topSequence.CaloHitAnalysis.CalibHits = "off"

#from GaudiSvc.GaudiSvcConf import THistSvc
#ServiceMgr += THistSvc()
#ServiceMgr.THistSvc.Output += ["CaloHitAnalysis DATAFILE='CaloHitAnalysis_v2.root' OPT='RECREATE'" ]


ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999

theApp.EvtMax = -1

#ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

# To set up a geometry
from RecExConfig.AutoConfiguration import *
ConfigureFieldAndGeo() # Configure the settings for the geometry
include("RecExCond/AllDet_detDescr.py") # Actually load the geometry
#include("TrkDetDescrSvc/AtlasTrackingGeometrySvc.py" ) # Tracking geometry, handy for ID work
