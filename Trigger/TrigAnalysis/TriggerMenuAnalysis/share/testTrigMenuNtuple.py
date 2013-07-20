from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource = 'data'
globalflags.DetDescrVersion = 'ATLAS-GEO-08-00-01'


from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.EvtMax=100

from RecExConfig.RecFlags  import rec
rec.readESD=True
rec.readRDO=False
rec.doWriteAOD=False
rec.doWriteESD=False
rec.doTruth=False
rec.doCBNT=False

athenaCommonFlags.FilesInput = [
    # ttbar _r774 (15.3.1.4)
    #'/rscratch/pcatr17/tkohno/data/test/ttbar/AOD.080429._000009.pool.root.1',
    #'/rscratch/pcatr17/tkohno/data/test/ttbar/AOD.080429._000120.pool.root.1',
    #'/rscratch/pcatr17/tkohno/data/test/ttbar/AOD.080429._000023.pool.root.1',

    # ttbar ESD
    #'/rscratch/pcatr18/tkohno/data/test/ESD.092344._000018.pool.root.1', 

    # run 142191 MuonswBeam reprocessed data
    '/rscratch/pcatr18/tkohno/data/900GeV/00142191/reprocessed/data09_900GeV.00142191.physics_MuonswBeam.hltmerge.ESD.c53_c55_m261._0001.1', 
    '/rscratch/pcatr18/tkohno/data/900GeV/00142191/reprocessed/data09_900GeV.00142191.physics_MuonswBeam.hltmerge.ESD.c53_c55_m261._0002.1', 
    '/rscratch/pcatr18/tkohno/data/900GeV/00142191/reprocessed/data09_900GeV.00142191.physics_MuonswBeam.hltmerge.ESD.c53_c55_m261._0003.1', 
    '/rscratch/pcatr18/tkohno/data/900GeV/00142191/reprocessed/data09_900GeV.00142191.physics_MuonswBeam.hltmerge.ESD.c53_c55_m261._0004.1', 
    '/rscratch/pcatr18/tkohno/data/900GeV/00142191/reprocessed/data09_900GeV.00142191.physics_MuonswBeam.hltmerge.ESD.c53_c55_m261._0005.1', 
    '/rscratch/pcatr18/tkohno/data/900GeV/00142191/reprocessed/data09_900GeV.00142191.physics_MuonswBeam.hltmerge.ESD.c53_c55_m261._0006.1', 
    '/rscratch/pcatr18/tkohno/data/900GeV/00142191/reprocessed/data09_900GeV.00142191.physics_MuonswBeam.hltmerge.ESD.c53_c55_m261._0007.1', 
    '/rscratch/pcatr18/tkohno/data/900GeV/00142191/reprocessed/data09_900GeV.00142191.physics_MuonswBeam.hltmerge.ESD.c53_c55_m261._0008.1', 
    '/rscratch/pcatr18/tkohno/data/900GeV/00142191/reprocessed/data09_900GeV.00142191.physics_MuonswBeam.hltmerge.ESD.c53_c55_m261._0009.1', 
    '/rscratch/pcatr18/tkohno/data/900GeV/00142191/reprocessed/data09_900GeV.00142191.physics_MuonswBeam.hltmerge.ESD.c53_c55_m261._0010.1', 
    '/rscratch/pcatr18/tkohno/data/900GeV/00142191/reprocessed/data09_900GeV.00142191.physics_MuonswBeam.hltmerge.ESD.c53_c55_m261._0011.1', 
    ]
# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#---------------------------------------------------------------------
# user modifier should come here
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# Set up trigger configuration service and metadata service it relies on,
# for analysis job without RecExCommon set up trigger decision tool
#---------------------------------------------------------------------
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool('TrigDecisionTool')
ToolSvc += tdt

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList = ['ds']
TriggerFlags.configForStartup = 'HLTofflineNoL1Thr'

# set up trigger config service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter()

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AlgSequence import AlgSequence


#------------------------------------------------------------------------
# My job
#------------------------------------------------------------------------

#TriggerFlags.NtupleProductionFlags.FileNames.forSlice
job = AlgSequence()
include('TriggerMenuAnalysis/TriggerMenuAnalysis_Nt.py')
#include('TrigEgammaNtuple/TrigEgammaNtuple_jo.py')
include('TrigMuonNtuple/TrigMuonNtuple_jo.py')
## #include("TrigTauPerformAthena/TrigTauNtuple_jo.py")
##job.TrigMuonNtupleAlg.ntupleComponents.remove('MuFastESD')
#job.TrigEgammaNtupleAlg.L2ElectronKey = 'L2IDCaloFex'
#job.TrigEgammaNtupleAlg.L1chains = []
#job.TrigEgammaNtupleAlg.L2chains = []
#job.TrigEgammaNtupleAlg.EFchains = []

job.TrigMenuNtupleAlg.ntupleFile = 'trigntuple'
job.TrigMuonNtupleAlg.ntupleFile = 'trigntuple'
#job.TrigEgammaNtupleAlg.ntupleFile = 'trigntuple'
#job.TrigTauPerformAthenaAlgo.StreamName = 'trigntuple'
#job.TrigMenuNtupleAlg.OutputLevel = DEBUG

job.TrigMuonNtupleAlg.ntupleComponents = [
    #'EventTrigInfo,EventInfo:ByteStreamEventInfo',
    'L1MuonRoI', 'Mufast', 'MuFastESD',
    'MuonInDet,SGkey:HLT_TrigSiTrack_Muon,BrName:InDet_SiTrk_Muon',
    'MuonInDet,SGkey:HLT_TrigIDSCAN_Muon,BrName:InDet_IDSCAN_Muon',
    'Mucomb', 'Muisol', 'Tilemu', 'TileTrackMu',
    'TrigMuonEF,SGkey:HLT_MuonEFInfo,BrName:TrigMuonEF',
    'TrigMuonEF,SGkey:HLT_eMuonEFInfo,BrName:TrigMuGirl',
    #'Tgc',
    #'Muon,SGkey:MuidMuonCollection,BrName:Muid',
    #'Muon,SGkey:StacoMuonCollection,BrName:Staco',
    #'Tile,MaxNtag:50'
    ]
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += [ "trigntuple DATAFILE='trigntuple.root' OPT='RECREATE'" ]

#ServiceMgr.StoreGateSvc.OutputLevel = ERROR
#job.TrigMuonNtupleAlg.OutputLevel = ERROR

#ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR
ToolSvc.RoILinksCnvTool.OutputLevel = DEBUG

## ServiceMgr.ToolSvc.HLTAccessTool.OutputLevel = DEBUG
## job.CBNT_AthenaAware.CBNTAA_TrigNtMenu.OutputLevel = DEBUG

ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
#ServiceMgr.MessageSvc.OutputLevel = INFO
#ServiceMgr.MessageSvc.OutputLevel = DEBUG

# ServiceMgr.StoreGateSvc.Dump = True

#
print ServiceMgr
#print ToolSvc

print 'topAlgos: '
print job


