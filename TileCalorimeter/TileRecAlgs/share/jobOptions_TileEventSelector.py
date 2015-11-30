if not 'RUN2' in dir():
    RUN2=True
if RUN2:
    from AthenaCommon.GlobalFlags import globalflags
    globalflags.DetGeo.set_Value_and_Lock('atlas')
    globalflags.DataSource.set_Value_and_Lock('data')
    globalflags.DatabaseInstance="CONDBR2"

    from IOVDbSvc.CondDB import conddb
    from AthenaCommon.GlobalFlags import jobproperties

    if not 'CondDbTag' in dir():
        CondDbTag = 'CONDBR2-BLKPA-2015-14'

    jobproperties.Global.ConditionsTag = CondDbTag
    conddb.setGlobalTag(CondDbTag)
    globalflags.ConditionsTag.set_Value_and_Lock(CondDbTag)

    if not 'DetDescrVersion' in dir():
        DetDescrVersion = 'ATLAS-R2-2015-03-01-00'

    jobproperties.Global.DetDescrVersion = DetDescrVersion
    globalflags.DetDescrVersion.set_Value_and_Lock(DetDescrVersion)

if not 'EvtMax' in dir():
    EvtMax = -1

if not 'EvtMin' in dir():
    EvtMin=0

if not 'ReadESD' in dir():
    ReadESD=False
if ReadESD:
    filter='recon.ESD'
else:
    filter='RAW'

if not 'WriteESD' in dir():
    WriteESD=True

if not 'TileUseDCS' in dir():
    TileUseDCS = True

from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()

#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 4
OutputLEVEL = OutputLevel
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 5000000
svcMgr.MessageSvc.Format = "% F%30W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

if 'FileNameVec' in dir():
    InputFile=FileNameVec

if not 'InputFile' in dir():
    from os import system,popen,path
    if not 'FileFilter' in dir():
        FileFilter = "data"
    if not 'InputDirectory' in dir():
        if not 'RunNumber' in dir():
            print "RunNumer not defined"
            exit(1)
        if not 'RunStream' in dir():
            RunStream = ("express_express" if ReadESD else "physics_Main") if RUN2 else "physics_JetTauEtmiss"
        if not 'DataProject' in dir():
            DataProject = "data15_13TeV" if RUN2 else "data12_8TeV"
        if RUN2:
            TopDir="/eos/atlas/atlastier0/rucio/%(D)s/%(S)s/%(R)08d" % {'D':DataProject, 'S':RunStream, 'R':RunNumber}
            for f in popen("xrd eosatlas dirlist %(path)s | grep %(filt)s | grep -v -i -e log -e tgz | awk '{print $NF}' | sort -r | tail -1" % {'path': TopDir, 'filt':filter }):
                temp=f.split('\n')
                InputDirectory=temp[0]
        else:
            TopDir="/castor/cern.ch/grid/atlas/tzero/prod1/perm/%(D)s/%(S)s/%(R)08d" % {'D':DataProject, 'S':RunStream, 'R':RunNumber}
            for f in popen("nsls %(path)s | grep %(filt)s | grep -v -i -e log -e tgz | sort -r | tail -1" % {'path': TopDir, 'filt':filter }):
                temp=f.split('\n')
                InputDirectory=TopDir+'/'+temp[0]
    if not 'InputDirectory' in dir():
        print "Input directory not found"
        exit(1)
    if InputDirectory.startswith('/castor'):
        cmd='nsls'
    elif InputDirectory.startswith('/eos'):
        cmd='xrd eosatlas dirlist'
    else:
        cmd='ls'
    files=[]
    print "Input directory is ", InputDirectory
    for f in popen("%(cm)s %(path)s | grep %(run)s | grep %(filt)s | grep -v '     0 ' | awk '{print $NF}'" % {'cm': cmd, 'path': InputDirectory, 'run':RunNumber, 'filt':FileFilter }):
        files.append(f.strip())
    if len(files)==0:
        print "Input file(s) not found"
        exit(1)
    if ReadESD: dummy="dummy.ESD.pool.root"
    else:       dummy="dummy.RAW.data"
    if path.isfile(dummy): InputFile=[dummy]
    else:                  InputFile=[]
    for name in files:
        if InputDirectory.startswith('/castor'):
            InputFile.append('rfio:'+InputDirectory+'/'+name)
        elif InputDirectory.startswith('/eos'):
            InputFile.append('root://eosatlas/'+name)
        else:
            InputFile.append(InputDirectory+'/'+name)

    #import glob
    #InputFile = glob.glob(InputDirectory)

for item in InputFile:
        print item
print len(InputFile) , " files for analysis"


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock(InputFile)
athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)

from RecExConfig.RecFlags import rec
from AthenaCommon.GlobalFlags import globalflags
if ReadESD:
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readRDO.set_Value_and_Lock(False)
else:
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    rec.readRDO.set_Value_and_Lock(True)
rec.readTAG.set_Value_and_Lock(False)
rec.readESD.set_Value_and_Lock(ReadESD)
rec.readAOD.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(WriteESD)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False) 
rec.doAOD.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doDetailedPerfMon.set_Value_and_Lock(False)
rec.doSemiDetailedPerfMon.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(False)

if not ReadESD and WriteESD:
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.Calo_setOff()
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
    DetFlags.detdescr.BField_setOn()
    DetFlags.Print()

    rec.doInDet = False
    rec.doLArg  = False
    rec.doMuon  = False
    rec.doLucid = False
    rec.doTau = False
    rec.doEgamma = False
    rec.doMuonCombined = False
    rec.doJetMissingETTag = False
    rec.doTrigger = False
    from RecExConfig.RecAlgsFlags import recAlgs
    recAlgs.doTrackParticleCellAssociation=False

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
        jobproperties.CaloRecFlags.doCaloTopoCluster=False
        jobproperties.CaloRecFlags.doTileMuId=False
        include.block("TrigT1CaloCalibTools/DecorateL1CaloTriggerTowers_prodJobOFragment.py")
        include.block("TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py")
        include.block("TrigT1CaloCalibTools/DecorateL1CaloTriggerTowers_prodJobOFragment.py")

if 'ForceTimeStamp' in dir():
    include.block("LumiBlockComps/LumiBlockMuWriter_jobOptions.py")
    include.block("RecBackgroundAlgs/RecBackground_jobOptions.py")


# main jobOption - must always be included
##############################
include ("RecExCommon/RecExCommon_topOptions.py")
##############################

if not ReadESD and WriteESD:
    if not rec.doLArg:
        topSequence.CaloCellMaker.CaloCellMakerToolNames = [ ToolSvc.TileCellBuilder.getFullName() ]
    #ToolSvc.TileCellBuilder.OutputLevel=2


# define filter
import AthenaCommon.AlgSequence as acas
job = acas.AlgSequence()

seq = acas.AthSequencer("AthFilterSeq")

# example how to copy few events from input to output
#
#import AthenaCommon.Constants as Lvl
#from GaudiSequencer.PyComps import PyEvtFilter
#seq += PyEvtFilter(
#    'alg',
#    # the store-gate key. leave as an empty string to take any eventinfo instance
#    evt_info='',
#    OutputLevel=Lvl.INFO)
#
################################################################
##       List of event to keep
################################################################
##seq.alg.evt_list = [ 1084655, 17950341 ] #Event Numbers
##seq.alg.evt_list = [ (155228,1815419) ] #Run,Event
#seq.alg.evt_list = [ (189660, 17617277),
#                     (191138, 6857449),
#                     (191513, 20422820),
#                     (191920, 34336488) ]

if RUN2:
    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    #topSequence+=xAODMaker__EventInfoCnvAlg()
    seq+=xAODMaker__EventInfoCnvAlg()

from TileRecAlgs.TileRecAlgsConf import TileCellSelector
seq += TileCellSelector('TileSelector')
if ReadESD:
    seq.TileSelector.CellContainerName = "AllCalo"
    seq.TileSelector.DigitsContainerName = "TileDigitsFlt"
    seq.TileSelector.RawChannelContainerName = "TileRawChannelFlt"
else:
    seq.TileSelector.CellContainerName = ""
    seq.TileSelector.DigitsContainerName = "TileDigitsCnt"
    seq.TileSelector.RawChannelContainerName = "TileRawChannelCnt"

seq.TileSelector.MinEnergyCell=-10000.
seq.TileSelector.MaxEnergyCell=1000000.
seq.TileSelector.PtnEnergyCell=101
seq.TileSelector.MinEnergyChan=-5000.
seq.TileSelector.MaxEnergyChan=500000.
seq.TileSelector.PtnEnergyChan=101
seq.TileSelector.MinEnergyGap=-10000.
seq.TileSelector.MaxEnergyGap=500000.
seq.TileSelector.PtnEnergyGap=101
seq.TileSelector.MinEnergyMBTS=-10000.
seq.TileSelector.MaxEnergyMBTS=500000.
seq.TileSelector.PtnEnergyMBTS=101

seq.TileSelector.MinTimeCell=-100.
seq.TileSelector.MaxTimeCell=100.
seq.TileSelector.PtnTimeCell=10
seq.TileSelector.MinTimeChan=-100.
seq.TileSelector.MaxTimeChan=100.
seq.TileSelector.PtnTimeChan=10
seq.TileSelector.MinTimeGap=-100.
seq.TileSelector.MaxTimeGap=100.
seq.TileSelector.PtnTimeGap=10
seq.TileSelector.MinTimeMBTS=-100.
seq.TileSelector.MaxTimeMBTS=100.
seq.TileSelector.PtnTimeMBTS=10

seq.TileSelector.JumpDeltaHG=50.
seq.TileSelector.JumpDeltaLG=20.
seq.TileSelector.PedDetlaHG=4.1
seq.TileSelector.PedDetlaLG=4.1
seq.TileSelector.ConstLength=7
seq.TileSelector.MinBadMB=4
seq.TileSelector.MinBadDMU=4
seq.TileSelector.MaxBadDMU=16
seq.TileSelector.SkipMasked=True
seq.TileSelector.CheckDCS=TileUseDCS
seq.TileSelector.SkipMBTS=True
seq.TileSelector.CheckDMUs=True
seq.TileSelector.CheckJumps=True
seq.TileSelector.CheckOverLG=True
seq.TileSelector.CheckOverHG=False
seq.TileSelector.MaxVerboseCnt=99999999
seq.TileSelector.OutputLevel=1

# get a handle on the job main sequence                                         
def _retrieve_outputstream_name():
    for alg in topSequence.getChildren():
        if isinstance(alg, CfgMgr.AthenaOutputStream):
            print "-->",alg.name()
            if not alg.name().endswith('_FH'):
                return alg.name()

output_algname = _retrieve_outputstream_name()
if output_algname:
    output_stream = getattr(topSequence, output_algname)
    output_stream.AcceptAlgs = [seq.TileSelector.name()]

svcMgr.MessageSvc.OutputLevel = OutputLEVEL
svcMgr.EventSelector.SkipEvents = EvtMin

if 'ForceTimeStamp' in dir():
    svcMgr.IOVDbSvc.forceTimestamp = ForceTimeStamp

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
svcMgr += AthenaEventLoopMgr()
svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 1
svcMgr.MessageSvc.defaultLimit = 5000000

from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
svcMgr+=DBReplicaSvc(UseCOOLSQLite=False)
