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


if not 'InputFile' in dir():
    from os import system,popen
    if not 'FileFilter' in dir():
        FileFilter = "data"
    if not 'InputDirectory' in dir():
        if not 'RunNumber' in dir():
            print "RunNumer not defined"
            exit(1)
        if not 'RunStream' in dir():
            RunStream = "physics_JetTauEtmiss"
        if not 'DataProject' in dir():
            DataProject = "data12_8TeV"
        if not 'CastorPrefix' in dir():
            CastorPrefix="/castor/cern.ch/grid/atlas/tzero/prod1/perm"
        TopDir="/castor/cern.ch/grid/atlas/tzero/prod1/perm/%(D)s/%(S)s/%(R)08d" % {'D':DataProject, 'S':RunStream, 'R':RunNumber}
        for f in popen('nsls %(path)s | grep %(filt)s | grep -v log | sort -r' % {'path': TopDir, 'filt':filter }):
            temp=f.split('\n')
            InputDirectory=TopDir+'/'+temp[0]
    if not 'InputDirectory' in dir():
        print "Input directory not found"
        exit(1)
    if not 'RunFromLocal' in dir() or not RunFromLocal:
        cmd='nsls'
        RunFromLocal=False
    else:
        cmd='ls'
        RunFromLocal=True
    files=[]
    print "Input directory is ", InputDirectory
    for f in popen('%(cm)s %(path)s | grep %(run)s | grep %(filt)s' % {'cm': cmd, 'path': InputDirectory, 'run':RunNumber, 'filt':FileFilter }):
        files.append(f)
    if len(files)==0:
        print "Input file(s) not found"
        exit(1)
    if ReadESD:
        InputFile=["dummy.ESD.pool.root"]
    else:
        InputFile=["dummy.RAW.data"]
    for nn in range(len(files)):
        temp=files[nn].split('\n')
        if RunFromLocal or not ReadESD:
            InputFile.append(InputDirectory+'/'+temp[0])
        else:
            InputFile.append('castor:'+InputDirectory+'/'+temp[0])

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
seq.TileSelector.MinEnergyChan=-5000.
seq.TileSelector.MaxEnergyChan=500000.
seq.TileSelector.MinEnergyGap=-10000.
seq.TileSelector.MaxEnergyGap=500000.
seq.TileSelector.MinEnergyMBTS=-10000.
seq.TileSelector.MaxEnergyMBTS=500000.
seq.TileSelector.JumpDeltaHG=50.
seq.TileSelector.JumpDeltaLG=20.
seq.TileSelector.PedDetlaHG=4.1
seq.TileSelector.PedDetlaLG=4.1
seq.TileSelector.ConstLength=7
seq.TileSelector.MinBadMB=4
seq.TileSelector.MinBadDMU=5
seq.TileSelector.MaxBadDMU=15
seq.TileSelector.SkipMasked=True
seq.TileSelector.CheckDCS=True
seq.TileSelector.SkipMBTS=True
seq.TileSelector.CheckDMUs=True
seq.TileSelector.CheckJumps=True
seq.TileSelector.CheckOverLG=True
seq.TileSelector.CheckOverHG=False
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
svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100

from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
svcMgr+=DBReplicaSvc(UseCOOLSQLite=False)
