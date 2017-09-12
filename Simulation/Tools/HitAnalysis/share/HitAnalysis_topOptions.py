#from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

from PartPropSvc.PartPropSvcConf import PartPropSvc

include("ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include("EventAthenaPool/EventAthenaPool_joboptions.py" )

#import os
#from glob import glob
#from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
#athenaCommonFlags.FilesInput = glob( "/tmp/" + os.environ['USER'] + "/" + "HITS*root*" )
#ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from HitAnalysis.HitAnalysisConf import *
topSequence += SiHitAnalysis('PixelHitAnalysis')
topSequence.PixelHitAnalysis.CollectionName = 'PixelHits'
topSequence.PixelHitAnalysis.HistPath = '/HitAnalysis/Pixel/'

topSequence += SiHitAnalysis('SCTHitAnalysis')
topSequence.SCTHitAnalysis.CollectionName = 'SCT_Hits'
topSequence.SCTHitAnalysis.HistPath = '/HitAnalysis/SCT/'

topSequence += SiHitAnalysis('BCMHitAnalysis')
topSequence.BCMHitAnalysis.CollectionName = 'BCMHits'
topSequence.BCMHitAnalysis.HistPath = '/HitAnalysis/BCM/'

topSequence += SiHitAnalysis('BLMHitAnalysis')
topSequence.BLMHitAnalysis.CollectionName = 'BLMHits'
topSequence.BLMHitAnalysis.HistPath = '/HitAnalysis/BLM/'

topSequence += TRTHitAnalysis('TRTHitAnalysis')
topSequence.TRTHitAnalysis.HistPath = '/HitAnalysis/TRT/'

topSequence += RPCHitAnalysis('RPCHitAnalysis')
topSequence.RPCHitAnalysis.HistPath = '/HitAnalysis/RPC/'

topSequence += MDTHitAnalysis('MDTHitAnalysis')
topSequence.MDTHitAnalysis.HistPath = '/HitAnalysis/MDT/'

topSequence += CSCHitAnalysis('CSCHitAnalysis')
topSequence.CSCHitAnalysis.HistPath = '/HitAnalysis/CSC/'

topSequence += TGCHitAnalysis('TGCHitAnalysis')
topSequence.TGCHitAnalysis.HistPath = '/HitAnalysis/TGC/'

#topSequence += ALFAHitAnalysis('ALFAHitAnalysis')
#topSequence.ALFAHitAnalysis.HistPath = '/HitAnalysis/ALFA/'

#topSequence += LucidHitAnalysis('LucidHitAnalysis')
#topSequence.LucidHitAnalysis.HistPath = '/HitAnalysis/Lucid/'

#topSequence += ZDCHitAnalysis('ZDCHitAnalysis')
#topSequence.ZDCHitAnalysis.HistPath = '/HitAnalysis/ZDC/'

topSequence += TrackRecordAnalysis('TrackRecordAnalysis')
topSequence.TrackRecordAnalysis.HistPath = '/HitAnalysis/Track/'

topSequence += TruthHitAnalysis('TruthHitAnalysis')
topSequence.TruthHitAnalysis.HistPath = '/HitAnalysis/Truth/'

topSequence += CaloHitAnalysis('CaloHitAnalysis')
topSequence.CaloHitAnalysis.HistPath = '/HitAnalysis/Calo'           
                                                                                                     
#Add some more TH2 histograms


topSequence.PixelHitAnalysis.ExpertMode= "off"
topSequence.SCTHitAnalysis.ExpertMode= "off"
topSequence.BCMHitAnalysis.ExpertMode= "off"
topSequence.BLMHitAnalysis.ExpertMode= "off"
topSequence.CaloHitAnalysis.ExpertMode = "off"
topSequence.CaloHitAnalysis.CalibHits = "off" 


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
#include( "TrkDetDescrSvc/AtlasTrackingGeometrySvc.py" ) # Tracking geometry, handy for ID work
