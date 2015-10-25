#*****************************************************************
#
# """topOptions file for Tile Laser Reconstruciton and Monitoring in Athena""" 
# """This topOptions is intended to test the monitoring code"""
#=================================================================

from os import system, popen
from AthenaCommon.Logging import logging
log = logging.getLogger( 'jobOptions_TileTBMon.py' )


MonitorOutput = 'Tile'
TileDCS = False

if not 'TestOnline' in dir():
    TestOnline = False

if TestOnline:
    doOnline = True;
    storeHisto = True;


if not 'TileMonoRun' in dir():
    TileMonoRun = False

def FindFile(path, runinput):

    run = str(runinput)

    while len(run) < 7:
        run = '0' + run
        
    files = []
    fullname = []

    if path.startswith("/castor") :
        for f in popen('ls %(path)s | grep %(run)s' % {'path': path, 'run':run }):
            files.append(f)

    elif path.startswith("/eos") :
        for f in popen('eos ls %(path)s | grep %(run)s' % {'path': path, 'run':run }):
            files.append(f)

    else:
        for f in popen('nsls  %(path)s | grep %(run)s' % {'path': path, 'run':run }):
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
DetFlags.detdescr.LAr_setOff()
DetFlags.detdescr.Tile_setOn()
DetFlags.readRDOBS.Tile_setOn()

# Switch off DCS
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

if not athenaCommonFlags.isOnline() or TestOnline:

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
    DetDescrVersion = 'ATLAS-R2-2015-02-00-00'
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

    db = '/afs/cern.ch/user/t/tiledemo/public/efmon/condb/tileSqlite.db'
    from TileConditions.TileCoolMgr import tileCoolMgr
    tileCoolMgr.setGlobalDbConn(db)


# setting option to build frag->ROB mapping at the begin of run
ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
ByteStreamCnvSvc.ROD2ROBmap = [ "-1" ] 

topSequence += CfgMgr.xAODMaker__EventInfoCnvAlg()

TileCorrectTime = False    
doTileOptATLAS = False

if TileMonoRun:
    doTileOpt2 = False
    doTileFit = True


# load conditions data
include( "TileRec/TileDefaults_jobOptions.py" )
include( "TileConditions/TileConditions_jobOptions.py" )


# set reconstruction flags and reconstruct data
from TileRecUtils.TileRecFlags import jobproperties
jobproperties.TileRecFlags.calibrateEnergy.set_Value_and_Lock(False) #don't need pC in raw channels, keep ADC counts
jobproperties.TileRecFlags.noiseFilter.set_Value_and_Lock(0) #Enable noise filter tool
jobproperties.TileRecFlags.BestPhaseFromCOOL.set_Value_and_Lock(False) #Use best phase from COOL
jobproperties.TileRecFlags.doTileOverflowFit.set_Value_and_Lock(True)
include( "TileRec/TileRec_jobOptions.py" )


if doTileCells:
    # enable interpolation for dead cells
    doCaloNeighborsCorr = False
    include('TileRec/TileCellMaker_jobOptions.py')
    ToolSvc.TileCellBuilder.UseDemoCabling = 2015
    ToolSvc.TileCellBuilder.maskBadChannels = False

#----------------
# TileMonitoring
#----------------
if doMonitoring:




    topSequence += CfgMgr.AthenaMonManager( "TileTBMonManager"
                                            , ManualRunLBSetup    = True
                                            , ManualDataTypeSetup = True
                                            , Environment         = "online"
                                            , FileKey             = MonitorOutput
                                            , Run                 = RunNumber
                                            , LumiBlock           = 1)


    #-------------------------------
    #   Tile raw channel time monitoring
    #-------------------------------
    toolSvc += CfgMgr.TileTBMonTool ( name                  = 'TileTBMonTool'
                                      , histoPathBase       = '/Tile/TestBeam'
                                      # , doOnline            = athenaCommonFlags.isOnline()
                                      , CellsContainerName  = '' # used to check if the current event is collision
                                      , MBTSCellContainerID = '' # used to check if the current event is collision
                                      , CellContainer       = 'AllCalo')


    topSequence.TileTBMonManager.AthenaMonTools += [ toolSvc.TileTBMonTool ]
    print toolSvc.TileTBMonTool
    

    toolSvc += CfgMgr.TileTBBeamMonTool ( name                  = 'TileTBBeamMonTool'
                                          , histoPathBase       = '/Tile/TestBeam/BeamElements'
                                          # , doOnline            = athenaCommonFlags.isOnline()
                                          , CellsContainerName  = '' # used to check if the current event is collision
                                          , MBTSCellContainerID = '' # used to check if the current event is collision
                                          , CutEnergyMin = 40
                                          , CutEnergyMax = 70 
                                          );



    topSequence.TileTBMonManager.AthenaMonTools += [ toolSvc.TileTBBeamMonTool ]
    
    print toolSvc.TileTBBeamMonTool
    

    if TileMonoRun:
        toolSvc += CfgMgr.TileRawChannelMonTool ( name            ="TileRawChannelMon"
                                                  , histoPathBase   = "/Tile/RawChannel"
                                                  , book2D          = False
                                                  , PlotDSP         = False
                                                  , runType         = 9
                                                  , SummaryUpdateFrequency = 100
                                                  , TileRawChannelContainer = 'TileRawChannelFit')

        topSequence.TileTBMonManager.AthenaMonTools += [ toolSvc.TileRawChannelMon ]
        
        print toolSvc.TileRawChannelMon

    print topSequence.TileTBMonManager


import os
# -- use root histos --
# THistService for native root in Athena
if not  athenaCommonFlags.isOnline() or storeHisto or athenaCommonFlags.isOnlineStateless():
    if not hasattr(svcMgr, 'THistSvc'):
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr += THistSvc('THistSvc')
    if os.path.exists(RootHistOutputFileName):
        os.remove(RootHistOutputFileName)
    svcMgr.THistSvc.Output = [MonitorOutput + " DATAFILE='" + RootHistOutputFileName + "' OPT='RECREATE'"]
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

