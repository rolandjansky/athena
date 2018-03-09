#from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool


from PartPropSvc.PartPropSvcConf import PartPropSvc

include("ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include("EventAthenaPool/EventAthenaPool_joboptions.py" )

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.07.00-NSW"

import os
from glob import glob
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
#athenaCommonFlags.FilesInput = glob( "/afs/cern.ch/work/a/ancuetog/HitsAreas/20.3.X.Y-VAL-r3/atlasG4_10ttbar_20.3.X.Y-VAL-r3-modtime.NoFrozenShower.DeadLAr.hits.pool.root" )
athenaCommonFlags.FilesInput = glob( "/tmp/"+os.environ['USER']+"/"+"HITS*root*" )
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

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



topSequence.PixelHitAnalysis.HistPath='/HitAnalysis/Pixel/histos/'
topSequence.SCTHitAnalysis.HistPath='/HitAnalysis/SCT/histos/'
topSequence.BCMHitAnalysis.HistPath='/HitAnalysis/BCM/histos/'
topSequence.BLMHitAnalysis.HistPath='/HitAnalysis/BLM/histos/'
topSequence.TRTHitAnalysis.HistPath='/HitAnalysis/TRT/histos/'
topSequence.RPCHitAnalysis.HistPath='/HitAnalysis/RPC/histos/'
topSequence.MDTHitAnalysis.HistPath='/HitAnalysis/MDT/histos/'
topSequence.CSCHitAnalysis.HistPath='/HitAnalysis/CSC/histos/'
topSequence.TGCHitAnalysis.HistPath='/HitAnalysis/TGC/histos/'
#topSequence.ALFAHitAnalysis.HistPath='/HitAnalysis/'
#topSequence.LucidHitAnalysis.HistPath='/HitAnalysis/'
#topSequence.ZDCHitAnalysis.HistPath='/HitAnalysis/'
topSequence.TrackRecordAnalysis.HistPath='/HitAnalysis/Track/histos/'
topSequence.TruthHitAnalysis.HistPath = '/HitAnalysis/Truth/histos/'
topSequence.CaloHitAnalysis.HistPath = '/HitAnalysis/Calo/histos/'
topSequence.MMHitAnalysis.HistPath='/HitAnalysis/MM/histos/'
topSequence.sTGCHitAnalysis.HistPath='/HitAnalysis/sTGC/histos/'
topSequence.PixelHitAnalysis.NtupleFileName='/HitAnalysis/Pixel/ntuple/'
topSequence.SCTHitAnalysis.NtupleFileName='/HitAnalysis/SCT/ntuple/'
topSequence.BCMHitAnalysis.NtupleFileName='/HitAnalysis/BCM/ntuple/'
topSequence.BLMHitAnalysis.NtupleFileName='/HitAnalysis/BLM/ntuple/'
topSequence.TRTHitAnalysis.NtupleFileName='/HitAnalysis/TRT/ntuple/'
topSequence.RPCHitAnalysis.NtupleFileName='/HitAnalysis/RPC/ntuple/'
topSequence.MDTHitAnalysis.NtupleFileName='/HitAnalysis/MDT/ntuple/'
topSequence.CSCHitAnalysis.NtupleFileName='/HitAnalysis/CSC/ntuple/'
topSequence.TGCHitAnalysis.NtupleFileName='/HitAnalysis/TGC/ntuple/'
#topSequence.ALFAHitAnalysis.NtupleFileName='/HitAnalysis/'
#topSequence.LucidHitAnalysis.NtupleFileName='/HitAnalysis/'
#topSequence.ZDCHitAnalysis.NtupleFileName='/HitAnalysis/'
topSequence.TrackRecordAnalysis.NtupleFileName='/HitAnalysis/Track/ntuple/'
topSequence.TruthHitAnalysis.NtupleFileName = '/HitAnalysis/Truth/ntuple/'
topSequence.CaloHitAnalysis.NtupleFileName = '/HitAnalysis/Calo/ntuple/'
topSequence.MMHitAnalysis.NtupleFileName='/HitAnalysis/MM/ntuple/'
topSequence.sTGCHitAnalysis.NtupleFileName='/HitAnalysis/sTGC/ntuple/'

#Add some more TH2 histograms


topSequence.PixelHitAnalysis.ExpertMode= "off"
topSequence.SCTHitAnalysis.ExpertMode= "off"
topSequence.BCMHitAnalysis.ExpertMode= "off"
topSequence.BLMHitAnalysis.ExpertMode= "off"
topSequence.CaloHitAnalysis.ExpertMode = "off"
topSequence.CaloHitAnalysis.CalibHits = "off"

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["HitAnalysis DATAFILE='AllHitAnalysis.root' OPT='RECREATE'" ]


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
