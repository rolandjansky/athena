#*****************************************************************
#
# """topOptions file for Tile Testbeam Reconstruciton and Monitoring in Athena""" 
# """This topOptions is intended to test the monitoring code"""
#=================================================================

#xxx configure TileDQstatusAlg

from AthenaCommon.Logging import logging
log_tbntuple = logging.getLogger( 'jobOptions_TileTBNtuple.py' )

if not 'EvtMin' in dir():
    EvtMin = 0

if not 'EvtMax' in dir():
    EvtMax = 5000000

if not 'TestOnline' in dir():
    TestOnline = True

if not 'InputDirectory' in dir():
    InputDirectory = "/data1/daq"

if not 'FileName' in dir():
    FileName = 'ALL'

if not 'RunFromLocal' in dir():
    RunFromLocal = True

if not 'RunNumber' in dir():
    RunNumber = 0

if not 'Version' in dir():
    Version = ''

if 'FileFilter' in dir() and len(FileFilter)>0 and FileFilter != '.':
    Version += '_'+FileFilter

if not "histo_name" in dir():
    histo_name = "tilemon_%s%s.root" % (RunNumber,Version)

if not "ntuple_name" in dir():
    ntuple_name = "tiletb_%s%s.root" % (RunNumber,Version)

if len(Version)==0:
    Version = '0'

if not 'TileCisRun' in dir():  
    TileCisRun = False

if not 'TileMonoRun' in dir():
    TileMonoRun = False

if not 'TilePedRun' in dir():
    TilePedRun = False

if not 'TilePhysRun' in dir():
    TilePhysRun = not (TileCisRun or TileMonoRun or TilePedRun)

if not 'doTileOpt2' in dir():
    doTileOpt2 = not (TileCisRun or TileMonoRun)
    doTileOpt2 = True

if not 'doTileFit' in dir():
    doTileFit = (TileCisRun or TileMonoRun)
    doTileFit = True

if not 'doTileOptATLAS' in dir():
    doTileOptATLAS = False

if not 'TileFELIX' in dir():
    TileFELIX = False

if (TileCisRun or TileMonoRun):
    TileOfflineUnits = 0
    TileCalibrateEnergy = False
else:
    TileOfflineUnits = 1
    TileCalibrateEnergy = True

TileCalibMode = (TilePedRun or TileCisRun)
TilePMTOrder = not (TileCisRun or TileMonoRun)
TileTBperiod = 2015 if (RunNumber/100000 == 5) else 2016

if TileFELIX:
    TileTBperiod = 2017

if RunNumber>800000:
    TileTBperiod += 2

if RunNumber >= 2110000:
    TileTBperiod = 2021

#---  Output printout level ----------------------------------- 
#output threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OutputLevel' in dir():
    OutputLevel = 3
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

include('TileMonitoring/jobOptions_TileTBMon.py')

if TileTBperiod < 2016:
    ByteStreamCnvSvc.ROD2ROBmap = [ "-1" ]

topSequence += CfgMgr.TileTBAANtuple( TBperiod = TileTBperiod, CalibrateEnergy=TileCalibrateEnergy, OfflineUnits=TileOfflineUnits, CalibMode=TileCalibMode, PMTOrder=TilePMTOrder, NSamples = 7, TileRawChannelContainerFlat = "", TileRawChannelContainerOpt="TileRawChannelOpt2" if doTileOpt2 else "", TileRawChannelContainerFit="TileRawChannelFit" if doTileFit else "", TileDigitsContainer = 'TileDigitsCnt' if not TileFELIX else 'TileDigitsFiltered', TileHitVector="", TileHitContainer="" )
print(topSequence.TileTBAANtuple)

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
datafile = '%(dir)s/%(name)s' %  {'dir': OutputDirectory, 'name': ntuple_name}
svcMgr.THistSvc.Output += [ "AANT DATAFILE='" + datafile + "' OPT='RECREATE' " ]
svcMgr.THistSvc.MaxFileSize = 32768

if TileCisRun or TileMonoRun:
    # CIS calibration using top calib alg
    from TileCalibAlgs.TileCalibAlgsConf import TileTopCalibAlg
    from TileCalibAlgs.TileCalibAlgsConf import TileCisDefaultCalibTool

    TileCalibAlg = TileTopCalibAlg()
    TileCalibAlg.RunNumber        = RunNumber
    TileCalibAlg.RunType          = 8
    TileCalibAlg.FileName = '%(dir)s/tileCalibCIS_%(RunNum).f_CIS.%(Version)s.root' %  {'dir': OutputDirectory, 'RunNum': RunNumber, 'Version': Version }

    # declare CIS tool(s) and set jobOptions if necessary
    TileCisTool = TileCisDefaultCalibTool()

    if hasattr(ToolSvc, 'TileDigitsMon'):
        TileCisTool.StuckBitsProbsTool = ToolSvc.TileDigitsMon

    TileCisTool.removePed = True
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += TileCisTool
    TileCalibAlg.TileCalibTools += [ TileCisTool ]

    topSequence += TileCalibAlg

print(topSequence)

svcMgr.EventSelector.SkipEvents = EvtMin
theApp.EvtMax=EvtMax

# special settings for TB 2015, not needed in 2016
#ToolSvc.TileRawChannelBuilderFitFilter.FirstSample = 1
#ToolSvc.TileRawChannelBuilderFitFilter.FrameLength = 7
