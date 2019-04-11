ReadESD=True

from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import jobproperties

if "ReadRaw" not in dir():
    ReadRaw=False

if "ReadESD" not in dir():
    ReadESD=False

if "ReadAOD" not in dir():
    ReadAOD=False

if ReadRaw:
    ReadESD=False
    ReadAOD=False    
    if "tileRecDSP" in dir():
        tileRecDSP=True
        tileRecItr=False
        tileRecNonItr=False
    elif "tileRecItr" in dir():
        tileRecDSP=False
        tileRecItr=True
        tileRecNonItr=False
    elif "tileRecNonItr" in dir():
        tileRecDSP=False
        tileRecItr=False
        tileRecNonItr=True
    else:
        tileRecDSP=False
        tileRecItr=False
        tileRecNonItr=False

if ReadESD:
    ReadRaw=False
    ReadAOD=False

if ReadAOD:
    ReadRaw=False
    ReadESD=False

if "FileNameVec" not in dir():
    if 'FileName' not in dir() or FileName=="":
        FileNameVec=['/afs/cern.ch/user/t/tilecomm/w0/rawdata/data10_7TeV.00166927.physics_Muons.merge.RAW._lb0077._SFO-11._0001.1']
    else:
        FileNameVec=[FileName]
        
if "OutputDirectory" not in dir():
    OutputDirectory="."

if "histo_name" not in dir():
    histo_name="tilemon.root"

RootHistOutputFileName=OutputDirectory+"/"+histo_name

if "tuple_name" not in dir():
    tuple_name="tileD3PD_out.root"

tuple_name=OutputDirectory+"/"+tuple_name

if ReadRaw :
    athenaCommonFlags.FilesInput.set_Value_and_Lock(FileNameVec)
    athenaCommonFlags.PoolRDOInput.set_Value_and_Lock(FileNameVec)
    athenaCommonFlags.BSRDOInput.set_Value_and_Lock(FileNameVec)
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    rec.readRDO.set_Value_and_Lock(True)
    rec.readESD.set_Value_and_Lock(False)
    rec.doESD.set_Value_and_Lock(True)
    rec.doWriteESD.set_Value_and_Lock(True)
    rec.readAOD.set_Value_and_Lock(False)
    if tileRecDSP or tileRecItr or tileRecNonItr: 
        from CaloRec.CaloCellFlags import jobproperties
        jobproperties.CaloCellFlags.doLArDeadOTXCorr.set_Value_and_Lock(False)
        from TileRecUtils.TileRecFlags import jobproperties
        jobproperties.TileRecFlags.correctAmplitude.set_Value_and_Lock(False)
        jobproperties.TileRecFlags.correctTime.set_Value_and_Lock(False)
        jobproperties.TileRecFlags.BestPhaseFromCOOL.set_Value_and_Lock(True)
        jobproperties.TileRecFlags.noiseFilter.set_Value_and_Lock(0);
        if tileRecDSP:
            jobproperties.TileRecFlags.TileRawChannelContainer.set_Value_and_Lock('TileRawChannelCnt')
        elif tileRecItr:
            jobproperties.TileRecFlags.TileRawChannelContainer.set_Value_and_Lock('TileRawChannelOpt2')
            jobproperties.TileRecFlags.doTileOpt2.set_Value_and_Lock(True)
        elif tileRecNonItr:
            jobproperties.TileRecFlags.TileRawChannelContainer.set_Value_and_Lock('TileRawChannelFixed')
            jobproperties.TileRecFlags.doTileOptATLAS.set_Value_and_Lock(True)
    
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.Calo_setOff()  #Switched off to avoid geometry
    DetFlags.ID_setOff()
    DetFlags.Muon_setOff()
    DetFlags.Truth_setOff()
    DetFlags.LVL1_setOff()
    DetFlags.digitize.all_setOff()

    DetFlags.dcs.Tile_setOn()
    DetFlags.detdescr.ID_setOff()
    DetFlags.detdescr.Muon_setOff()
    DetFlags.detdescr.LAr_setOn()
    DetFlags.detdescr.Tile_setOn()
    DetFlags.detdescr.LVL1_setOn()
    DetFlags.readRDOBS.Tile_setOn()
    DetFlags.makeRIO.Tile_setOn()
    DetFlags.Print()

    from RecExConfig.RecFlags import rec
    rec.doInDet = False
    rec.doLArg  = False
    rec.doMuon  = False
    rec.doLucid = False
    rec.doTau = False
    rec.doEgamma = False
    rec.doMuonCombined = False
    rec.doJetMissingETTag = False
    rec.doTrigger = False

    if not rec.doLArg:
        from LArROD.LArRODFlags import larRODFlags
        larRODFlags.readDigits=False
        larRODFlags.doLArFebErrorSummary=False
        from CaloRec.CaloCellFlags import jobproperties
        jobproperties.CaloCellFlags.doDeadCellCorr=False
        jobproperties.CaloCellFlags.doLArCreateMissingCells=False
        jobproperties.CaloCellFlags.doLArDeadOTXCorr=False
        jobproperties.CaloCellFlags.doLArThinnedDigits=False
        jobproperties.CaloCellFlags.doLArNoiseMasking=False
        jobproperties.CaloCellFlags.doLArSporadicMasking=False
        jobproperties.CaloCellFlags.doLArBadFebMasking=False
        from CaloRec.CaloRecFlags import jobproperties
        jobproperties.CaloRecFlags.doLArAffectedRegion=False
        jobproperties.CaloRecFlags.doLArNoisyRO=False
        jobproperties.CaloRecFlags.doEMDigits=False
        jobproperties.CaloRecFlags.doLArNoiseBurstVeto=False
        jobproperties.CaloRecFlags.doCaloEMTopoCluster=False
        jobproperties.CaloRecFlags.doEmCluster=False
        jobproperties.CaloRecFlags.doTileMuId=False
    
if ReadESD:
    athenaCommonFlags.PoolESDInput.set_Value_and_Lock(FileNameVec)
    rec.readESD.set_Value_and_Lock(True)
    rec.readAOD.set_Value_and_Lock(False)
    #rec.doESD.set_Value_and_Lock(True)
    #rec.readRDO.set_Value_and_Lock(False)
    #rec.doWriteESD.set_Value_and_Lock(False)

    # Temporarily
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.ID_setOff()
    DetFlags.Muon_setOff()
    DetFlags.Truth_setOff()
    DetFlags.LVL1_setOff()

    DetFlags.dcs.Tile_setOn()
    DetFlags.detdescr.ID_setOff()
    DetFlags.detdescr.Muon_setOff()
    DetFlags.detdescr.LAr_setOn()
    DetFlags.detdescr.Tile_setOn()
    DetFlags.detdescr.LVL1_setOn()
    DetFlags.Print()


if ReadAOD:
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock(FileNameVec)
    rec.readAOD.set_Value_and_Lock(True)
    #rec.readESD.set_Value_and_Lock(True)
    #rec.doESD.set_Value_and_Lock(True)
    #rec.readRDO.set_Value_and_Lock(False)
    #rec.doWriteESD.set_Value_and_Lock(False)



rec.AutoConfiguration=['everything']

if "EvtMax" not in dir():
    EvtMax=-1

athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)

rec.doHist.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteTAGCOM.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doDetailedPerfMon.set_Value_and_Lock(False)
rec.doSemiDetailedPerfMon.set_Value_and_Lock(False)
rec.OutputLevel.set_Value_and_Lock(INFO)
# RecExCommon
include ("RecExCommon/RecExCommon_topOptions.py")

if not 'MonitorOutput' in dir():
#   MonitorOutput="SHIFT"
   MonitorOutput="Tile"

#**************************************************************
#
# jopOptions file for Tile Monitoring in Athena 
#
#==============================================================

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "ManagedAthenaTileMon" )
ManagedAthenaTileMon = topSequence.ManagedAthenaTileMon

from AthenaCommon.AppMgr import ServiceMgr

ManagedAthenaTileMon.FileKey = "SHIFT"
ManagedAthenaTileMon.ManualDataTypeSetup = True
ManagedAthenaTileMon.DataType            = "cosmics"
if athenaCommonFlags.isOnline():
    ManagedAthenaTileMon.Environment         = "online"
else:
    ManagedAthenaTileMon.Environment         = "online"
    #ManagedAthenaTileMon.Environment         = "tier0"

#ManagedAthenaTileMon.ManualRunLBSetup    = False
ManagedAthenaTileMon.ManualRunLBSetup    = True
ManagedAthenaTileMon.Run                 = RunNumber
ManagedAthenaTileMon.LumiBlock           = 1

## Adding JVT (TD)
from AthenaCommon import CfgMgr
jvt = CfgMgr.JetVertexTaggerTool('JVT')
ToolSvc += jvt
## Adding jet cleaning (TD)
cleaning = CfgMgr.JetCleaningTool("MyCleaningTool")
cleaning.CutLevel = "LooseBad"
cleaning.DoUgly = False
ToolSvc += cleaning
ecTool                 = CfgMgr.ECUtils__EventCleaningTool("MyEventCleaningTool")
ecTool.JetCleaningTool = cleaning
ecTool.PtCut           = 20000.0
ecTool.EtaCut          = 2.4
ecTool.JvtDecorator    = "passJvt"
ecTool.OrDecorator     = "passOR"
ecTool.CleaningLevel   = cleaning.CutLevel
ToolSvc += ecTool

from TileMonitoring.TileMonitoringConf import TileJetMonTool
TileJetMonTool = TileJetMonTool(name                   = 'TileJetMonTool',
                                jetPtMin               = 20000.0,
                                jetEtaMax              = 1.6,
                                jetCollectionName      = 'AntiKt4EMTopoJets',
                                do_1dim_histos         = True,
                                do_2dim_histos         = False,
                                do_enediff_histos      = False,
                                energyChanMin          = 15000, # 2000,
                                energyChanMax          = 50000, # 4000,
                                enediff_threshold      = 2000,
                                do_event_cleaning      = True,
                                do_jet_cleaning        = True,
                                useJVTTool             = jvt,
                                useJetCleaning         = cleaning,
                                useEventCleaning       = ecTool,
                                jet_tracking_eta_limit = 2.4,
                                jet_JVT_threshold      = 0.59,
                                jet_JVT_pTmax          = 120000, # MeV
                                OutputLevel            = INFO);
ToolSvc += TileJetMonTool;    
ManagedAthenaTileMon.AthenaMonTools += [ TileJetMonTool ];


print ManagedAthenaTileMon;


# -- use root histos --
# THistService for native root in Athena
# if not  athenaCommonFlags.isOnline() or storeHisto or athenaCommonFlags.isOnlineStateless():
if True:
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr += THistSvc("THistSvc")
    tTHistSvc = svcMgr.THistSvc
    tTHistSvc.Output = [MonitorOutput+" DATAFILE='"+RootHistOutputFileName+"' OPT='RECREATE'"]
    print tTHistSvc
else:
    from TrigServices.TrigServicesConf import TrigMonTHistSvc
    trigmonTHistSvc = TrigMonTHistSvc("THistSvc")
    svcMgr += trigmonTHistSvc


