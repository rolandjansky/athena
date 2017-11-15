
#EvtMax=-1
EvtMax=20
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput =[
# "/eos/atlas/atlasdatadisk/rucio/mc16_13TeV/a1/c3/RDO_FTK.11718543._000001.pool.root.1" #  MC16 v2
"RDO_FTK.root"
 ]


from RecExConfig.RecFlags import rec
doWriteRDO=False
rec.doWriteAOD=False
rec.doWriteESD=False
#rec.doAOD=True
rec.doWriteTAG=False
rec.doHist=True
#rec.doCBNT=True
#rec.doMonitoring=True


# Switch on FTK in offline reconstruction - needed to create ESD & AOD
rec.doFTK.set_Value_and_Lock( True )

rec.doTrigger=False

include ("RecExCommon/RecExCommon_topOptions.py")

from TrigFTK_RawDataAlgs.TrigFTK_RawDataAlgsConf import FTK_RDO_MonitorAlgo
FTK_RDO_Monitor = FTK_RDO_MonitorAlgo( "FTK_RDO_MonitorAlgo")
FTK_RDO_Monitor.RDO_CollectionName="FTK_RDO_Tracks"
FTK_RDO_Monitor.offlineTracksName="Tracks"
FTK_RDO_Monitor.FTK_DataProvider=theFTK_DataProviderSvc

alg += FTK_RDO_Monitor

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc(Output = ["TRACKS DATAFILE='ftk.root', OPT='RECREATE'"])

#ServiceMgr.ConfiguredFTK_DataProviderSvc.OutputLevel=VERBOSE
#topSequence.FTK_RDO_MonitorAlgo.reverseIBLlocx=True
#topSequence.FTK_RDO_MonitorAlgo.OutputLevel=DEBUG
#topSequence.FTK_Tracks_DetailedTruthCollectionMaker.OutputLevel=VERBOSE
#topSequence.FTK_RefitTracks_DetailedTruthCollectionMaker.OutputLevel=VERBOSE
#ToolSvc.FTK_TrackFitter.OutputLevel=VERBOSE


#MessageSvc.verboseLimit = 9999999
#MessageSvc.debugLimit = 9999999
#MessageSvc.warningLimit = 9999999
#MessageSvc.infoLimit = 9999999
#MessageSvc.errorLimit = 9999999





