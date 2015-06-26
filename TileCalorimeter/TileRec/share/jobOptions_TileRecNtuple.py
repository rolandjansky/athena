from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import jobproperties

if not 'OutputLevel' in dir():
    OutputLevel = WARNING

if not 'OutputDirectory' in dir():
    OutputDirectory = '.'

if not 'Version' in dir():
    Version = 0


if "FileNameVec" not in dir():
    if 'FileName' not in dir() or FileName=="":
        FileNameVec=['']
    else:
        FileNameVec=[FileName]

if 'EvtMax' in dir():
    athenaCommonFlags.EvtMax.set_Value_and_Lock(EvtMax)

athenaCommonFlags.FilesInput.set_Value_and_Lock(FileNameVec)
athenaCommonFlags.PoolESDInput.set_Value_and_Lock(FileNameVec)

from RecExConfig.InputFilePeeker import inputFileSummary

if not 'RunNumber' in dir():
    if inputFileSummary.has_key('run_number'): RunNumber = inputFileSummary['run_number'][0]
    else:                                            RunNumber = 000000


rec.AutoConfiguration=['everything']
rec.readESD.set_Value_and_Lock(True)
rec.readAOD.set_Value_and_Lock(False)
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
rec.OutputLevel.set_Value_and_Lock(OutputLevel)

# RecExCommon
include ("RecExCommon/RecExCommon_topOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


if 'doTileDigitsNtuple' in dir():
    if doTileDigitsNtuple:
        from AthenaCommon import CfgMgr
        topSequence += CfgMgr.TileDigitsToNtuple( "TileDigitsToNtuple")


from AthenaCommon.AppMgr import theApp

theApp.HistogramPersistency = 'ROOT'
NTupleSvc = Service( 'NTupleSvc' )
NTupleSvc.Output = [ "TILE DATAFILE='tile_" + str(RunNumber) + "_" + str(Version) + ".ntuple.root' OPT='NEW'" ]



svcMgr = theApp.serviceMgr()

if 'EvtMin' in dir():
    svcMgr.EventSelector.SkipEvents = EvtMin

if 'EventPrintoutInterval' in dir():
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    svcMgr += AthenaEventLoopMgr()
    svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = EventPrintoutInterval
