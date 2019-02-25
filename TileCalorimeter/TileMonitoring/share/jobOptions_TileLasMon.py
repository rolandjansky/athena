#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

#*****************************************************************
#
# """topOptions file for Tile Laser Reconstruciton and Monitoring in Athena""" 
# """This topOptions is intended to test the monitoring code"""
#=================================================================

MonitorOutput='Tile'

from AthenaCommon.Logging import logging
log = logging.getLogger( 'jobOptions_TileLasMon.py' )

from os import system, popen

def FindFile(path, runinput):

    run = str(runinput)

    while len(run) < 7:
        run = '0' + run
        
    files = []
    fullname = []

    if path.startswith("/castor") :
        for f in popen('nsls %(path)s | grep %(run)s' % {'path': path, 'run':run }):
            files.append(f)

    elif path.startswith("/eos") :
        for f in popen('eos ls %(path)s | grep %(run)s' % {'path': path, 'run':run }):
            files.append(f)

    else:
        for f in popen('ls  %(path)s | grep %(run)s' % {'path': path, 'run':run }):
            files.append(f)
            

    for nn in range(len(files)):
        temp = files[nn].split('\n')
        fullname.append(path + '/' + temp[0])

    return [fullname,run]

# include Flags jobOption
include( "TileMonitoring/TileRec_FlagOptions.py" )

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon import CfgMgr
toolSvc = CfgMgr.ToolSvc()


# set global flags
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('commis')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')

from AthenaCommon.BeamFlags import jobproperties     
jobproperties.Beam.beamType.set_Value_and_Lock(beamType)

# reset everything which is not needed
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()  #Switched off to avoid geometry
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.LAr_setOn()
DetFlags.detdescr.Tile_setOn()
DetFlags.readRDOBS.Tile_setOn()

if CheckDCS:
    DetFlags.dcs.Tile_setOn()
else:
    DetFlags.dcs.Tile_setOff()

DetFlags.Print()

from RecExConfig.RecFlags import rec
rec.doLArg = False

# set online flag if neeed
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if athenaCommonFlags.isOnline() or doOnline or doStateless:
    athenaCommonFlags.isOnline=True
    log.info( 'athenaCommonFlags.isOnline = True : Online Mode' )
    if doStateless:
        athenaCommonFlags.isOnlineStateless=True
        log.info( 'athenaCommonFlags.isOnlineStateless = True : Stateless Online Mode' )


#-----------------
# ByteSream Input 
#-----------------

if not athenaCommonFlags.isOnline():

    include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
    include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )

    if not 'InputDirectory' in dir():
        InputDirectory="/castor/cern.ch/grid/atlas/t0/perm/DAQ"
    if not 'RunNumber' in dir():
        RunNumber=0
    if not 'RunFromLocal' in dir():
        if InputDirectory=="." or RunNumber<10:
            RunFromLocal=True
        else:
            RunFromLocal=False
   
    if not 'FileNameVec' in dir():
        if not 'FileName' in dir():

            tmp = FindFile(InputDirectory,RunNumber)
            FileNameVec  = tmp[0]
            FormattedRunNumber = tmp[1]
            
        else:
            FileNameVec = [ InputDirectory+'/'+FileName ]
            FormattedRunNumber = RunNumber
    else:
       FormattedRunNumber = RunNumber

    svcMgr.EventSelector.SkipEvents = EvtMin
    theApp.EvtMax = EvtMax


    log.info( "InputDirectory is " + str(InputDirectory) )
    log.info( "RunNumber is " + str(FormattedRunNumber) )
    log.info( "FullFileName is " + str(FileNameVec) )
    log.info( "Skip Events is " + str(EvtMin) )
    log.info( "Max events is " + str(EvtMax) )

    svcMgr.ByteStreamInputSvc.FullFileName = FileNameVec
    svcMgr.EventSelector.MaxBadEvents = MaxBadEvents
   
    athenaCommonFlags.FilesInput = FileNameVec

    projectName = FileNameVec[0].split('/').pop().split('.')[0]
    log.info( "Project name is " + projectName )
    rec.projectName = projectName


# init DetDescr
from AthenaCommon.GlobalFlags import jobproperties
if not 'DetDescrVersion' in dir():
    DetDescrVersion = 'ATLAS-R2-2015-04-00-00'
jobproperties.Global.DetDescrVersion = DetDescrVersion 
log.info( "DetDescrVersion = %s" % (jobproperties.Global.DetDescrVersion() ))

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True
log.info( "GeoModelSvc.AtlasVersion = %s" % (GeoModelSvc.AtlasVersion) )

# Setup Db stuff
if TileUseCOOL:
    from IOVDbSvc.CondDB import conddb
    log.info( 'Tile COOL tag: ' + tileCOOLtag )
    conddb.setGlobalTag(tileCOOLtag)


# setting option to build frag->ROB mapping at the begin of run
ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
ByteStreamCnvSvc.ROD2ROBmap = [ "-1" ] 

topSequence += CfgMgr.xAODMaker__EventInfoCnvAlg()

from TileRecUtils.TileDQstatusAlgDefault import TileDQstatusAlgDefault
TileDQstatusAlgDefault()

if not athenaCommonFlags.isOnline():
    from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
    lumiTool = LuminosityToolDefault()
    lumiTool.OutputLevel = DEBUG
    toolSvc += lumiTool

if not athenaCommonFlags.isOnline() and False:
    from LumiBlockComps.TrigLivefractionToolDefault import TrigLivefractionToolDefault
    liveTool = TrigLivefractionToolDefault()
    liveTool.OutputLevel = DEBUG
    toolSvc += liveTool

TileRunType = 2 # laser run
doTileFit = True
TileCorrectTime = True    
doTileOptATLAS = False


# load conditions data
include( "TileRec/TileDefaults_jobOptions.py" )
include( "TileConditions/TileConditions_jobOptions.py" )
from TileConditions.TileCondToolConf import getTileCondToolTiming
tileInfoConfigurator.TileCondToolTiming = getTileCondToolTiming( 'COOL','GAPLAS')

# set reconstruction flags and reconstruct data
from TileRecUtils.TileRecFlags import jobproperties
jobproperties.TileRecFlags.calibrateEnergy.set_Value_and_Lock(False) #don't need pC in raw channels, keep ADC counts
jobproperties.TileRecFlags.noiseFilter.set_Value_and_Lock(1) #Enable noise filter tool
jobproperties.TileRecFlags.BestPhaseFromCOOL.set_Value_and_Lock(True) #Use best phase from COOL
jobproperties.TileRecFlags.doTileOverflowFit.set_Value_and_Lock(False)
include( "TileRec/TileRec_jobOptions.py" )


if not 'LaserUpdateFrequency' in dir():
    LaserUpdateFrequency = 0

if not 'LaserResetAfterUpdate' in dir():
    LaserResetAfterUpdate = False

if not 'LaserDoSummaryVsPMT' in dir():
    LaserDoSummaryVsPMT = False


#----------------
# TileMonitoring
#----------------
topSequence += CfgMgr.AthenaMonManager( "TileLasMon"
                                       , ManualRunLBSetup    = True
                                       , ManualDataTypeSetup = True
                                       , Environment         = "online"
                                       , FileKey             = MonitorOutput
                                       , Run                 = RunNumber
                                       , LumiBlock           = 1)


#-------------------------------
#   Tile raw channel time monitoring
#-------------------------------
TileLasRawChannelTimeMon = CfgMgr.TileRawChannelTimeMonTool ( name              = "TileLasRawChannelTimeMon"
                                                              , histoPathBase   = "/Tile/RawChannelTime"
                                                              , runType         = TileRunType
                                                              , doOnline        = athenaCommonFlags.isOnline()
                                                              , TimeCorrectionLBA  = -15.18
                                                              , TimeCorrectionLBC  = -15.37
                                                              , TimeCorrectionEBA  = 47.65
                                                              , TimeCorrectionEBC  = 47.42
                                                              , TileRawChannelContainer = "TileRawChannelFit")

topSequence.TileLasMon.AthenaMonTools += [ TileLasRawChannelTimeMon ]
print TileLasRawChannelTimeMon

#-------------------------------
#   Tile DQFrag monitoring
#-------------------------------
TileLasDQFragMon = CfgMgr.TileDQFragMonTool( name               = 'TileLasDQFragMon'
                                             , OutputLevel        = 3
                                             , TileRawChannelContainerDSP    = "TileRawChannelCnt"
                                             , TileRawChannelContainerOffl   = "TileRawChannelFit"
                                             , TileDigitsContainer           = "TileDigitsCnt"
                                             , NegAmpHG           = -200.
                                             , NegAmpLG           = -15.
                                             , SkipMasked         = True
                                             , SkipGapCells       = True
                                             , doOnline           = athenaCommonFlags.isOnline()
                                             , doPlots            = False
                                             , CheckDCS           = TileUseDCS
                                             , histoPathBase      = "/Tile/DMUErrors");

topSequence.TileLasMon.AthenaMonTools += [ TileLasDQFragMon ];
print TileLasDQFragMon
print topSequence.TileLasMon


import os
# -- use root histos --
# THistService for native root in Athena
if not  athenaCommonFlags.isOnline() or storeHisto or athenaCommonFlags.isOnlineStateless():
    #theApp.HistogramPersistency = "ROOT"
    if not hasattr(svcMgr,"THistSvc"):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr += THistSvc("THistSvc")
    if os.path.exists(RootHistOutputFileName):
        os.remove(RootHistOutputFileName)
    svcMgr.THistSvc.Output = [MonitorOutput+" DATAFILE='"+RootHistOutputFileName+"' OPT='RECREATE'"]
else:
    from TrigServices.TrigServicesConf import TrigMonTHistSvc
    trigmonTHistSvc = TrigMonTHistSvc("THistSvc")
    svcMgr += trigmonTHistSvc



#To read CTP RESULTS and DSP Raw Channels
if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
    svcMgr += ByteStreamAddressProviderSvc()

svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
                                                   "TileRawChannelContainer/TileRawChannelCnt",
                                                   "CTP_RDO/CTP_RDO",
                                                   "CTP_RIO/CTP_RIO",
                                                  ]


svcMgr.MessageSvc.defaultLimit= MsgLinesLimit
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.Format = "% F%35W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = useColors
#svcMgr.HistorySvc.OutputLevel = 3

theApp.EvtMax = EvtMax

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
svcMgr += AthenaEventLoopMgr()
svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100

if TileUseCOOL:
    from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    svcMgr += DBReplicaSvc(UseCOOLSQLite=False)

