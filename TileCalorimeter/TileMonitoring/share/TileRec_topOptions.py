#*****************************************************************
#
# """topOptions file for Tile Reconstruciton and Monitoring in Athena""" 
# """This topOptions is intended to test the monitoring code"""
#=================================================================

# MonitorOutput="EXPERT"

from os import system, popen

def FindFile(path, runinput, filter):

    run = str(runinput)

    while len(run) < 7:
        run = '0' + run
        
    files = []
    fullname = []

    if path.startswith('/castor') :
        for f in popen('nsls %(path)s | grep %(run)s | grep %(filt)s' % {'path': path, 'run':run, 'filt':filter }):
            files.append(f)

    elif path.startswith('/eos') :
        for f in popen('eos ls %(path)s | grep %(run)s | grep %(filt)s' % {'path': path, 'run':run, 'filt':filter }):
            files.append(f)

    else:
        for f in popen('ls  %(path)s | grep %(run)s | grep %(filt)s' % {'path': path, 'run':run, 'filt':filter }):
            files.append(f)
            

    for nn in range(len(files)):
        temp = files[nn].split('\n')
        fullname.append(path + '/' + temp[0])

    return [fullname, run]

# include Flags jobOption
include('TileMonitoring/TileRec_FlagOptions.py')



# # get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon import CfgMgr

# set global flags
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('commis')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')

from AthenaCommon.BeamFlags import jobproperties     
jobproperties.Beam.beamType.set_Value_and_Lock(beamType)

# reset everything which is not needed
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()  # Switched off to avoid geometry
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
    athenaCommonFlags.isOnline = True
    print 'athenaCommonFlags.isOnline = True : Online Mode'
    if doStateless:
        athenaCommonFlags.isOnlineStateless = True
        print 'athenaCommonFlags.isOnlineStateless = True : Stateless Online Mode'


#-----------------
# ByteSream Input 
#-----------------

if not athenaCommonFlags.isOnline():

    include('ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py')
    include('ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py')

    if not 'InputDirectory' in dir():
        InputDirectory = '/castor/cern.ch/grid/atlas/t0/perm/DAQ'
    if not 'RunNumber' in dir():
        RunNumber = 0
    if not 'RunFromLocal' in dir():
       if InputDirectory == '.' or RunNumber < 10:
           RunFromLocal = True
       else:
           RunFromLocal = False
   
    if not 'FileNameVec' in dir() or len(FileNameVec) == 0 or len(FileNameVec[0]) == 0:
        if not 'FileName' in dir() or len(FileName)==0:

            if not 'FileFilter' in dir():
                FileFilter = "data"
            tmp = FindFile(InputDirectory, RunNumber, FileFilter)
            FileNameVec = tmp[0]
            FormattedRunNumber = tmp[1]
            
        else:
            FileNameVec = [ InputDirectory + '/' + FileName ]
            FormattedRunNumber = RunNumber
    else:
        FormattedRunNumber = RunNumber

    svcMgr.EventSelector.SkipEvents = EvtMin
    theApp.EvtMax = EvtMax

    print 'InputDirectory is ' + str(InputDirectory)
    print 'RunNumber is ' + str(FormattedRunNumber)
    print 'FullFileName is ' + str(FileNameVec)
    print 'Skip Events is ' + str(EvtMin)
    print 'Max events is ' + str(EvtMax)

    athenaCommonFlags.FilesInput = FileNameVec
    svcMgr.EventSelector.Input = FileNameVec
    svcMgr.EventSelector.MaxBadEvents = MaxBadEvents


    projectName = FileNameVec[0].split('/').pop().split('.')[0]
    log.info( 'Project name is ' + projectName )
    rec.projectName = projectName


# init DetDescr
from AthenaCommon.GlobalFlags import jobproperties
if not 'DetDescrVersion' in dir():
    DetDescrVersion = 'ATLAS-R2-2015-04-00-00'
jobproperties.Global.DetDescrVersion = DetDescrVersion 
log.info('DetDescrVersion = %s' % (jobproperties.Global.DetDescrVersion()))

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True
log.info('GeoModelSvc.AtlasVersion = %s' % (GeoModelSvc.AtlasVersion))

# Setup Db stuff
if TileUseCOOL:
    from IOVDbSvc.CondDB import conddb
    log.info( 'Tile COOL tag: ' + tileCOOLtag )
    conddb.setGlobalTag(tileCOOLtag)

# connStr = '<dbConnection>sqlite://DUMMY;schema=caloSqlite.db;dbname=COMP200</dbConnection>'
# tag     = '<tag>CaloNoiseCellnoise-UPD1-00</tag>'
# tag     = '<tag>CaloNoiseCellnoise-UPD3-00</tag>'
# folder  = '/CALO/Noise/CellNoise'
# svcMgr.IOVDbSvc.Folders += [ folder + tag + connStr ]



# setting option to build frag->ROB mapping at the begin of run
ByteStreamCnvSvc = Service('ByteStreamCnvSvc')
ByteStreamCnvSvc.ROD2ROBmap = [ '-1' ] 


if doTrigger:
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool  
    tdt = Trig__TrigDecisionTool('TrigDecisionTool')
    ToolSvc += tdt
    if not athenaCommonFlags.isOnline():
        # To read files with trigger config stored as in-file meta-data,
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        TriggerFlags.configurationSourceList = ['ds']

        # set up trigger config service
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        cfg = TriggerConfigGetter()


topSequence += CfgMgr.xAODMaker__EventInfoCnvAlg()


if not athenaCommonFlags.isOnline():
    from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
    lumiTool = LuminosityToolDefault()
    lumiTool.OutputLevel = INFO
    ToolSvc += lumiTool
else:
    from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
    ToolSvc += LuminosityToolOnline()

if not athenaCommonFlags.isOnline() and False:
    from LumiBlockComps.TrigLivefractionToolDefault import TrigLivefractionToolDefault
    liveTool = TrigLivefractionToolDefault()
    liveTool.OutputLevel = INFO
    ToolSvc += liveTool


TileCorrectTime = True
TileCorrectAmplitude = True

# load conditions data
include('TileRec/TileDefaults_jobOptions.py')
include('TileConditions/TileConditions_jobOptions.py')

# set reconstruction flags and reconstruct data
from TileRecUtils.TileRecFlags import jobproperties
jobproperties.TileRecFlags.calibrateEnergy.set_Value_and_Lock(False)  # don't need pC in raw channels, keep ADC counts
jobproperties.TileRecFlags.noiseFilter.set_Value_and_Lock(TileNoiseFilter)  # Enable noise filter tool
jobproperties.TileRecFlags.BestPhaseFromCOOL.set_Value_and_Lock(True)  # Use best phase from COOL
include('TileRec/TileRec_jobOptions.py')

if jobproperties.Beam.beamType() == 'collisions' or doTileNoiseMon:
    jobproperties.TileRecFlags.TileRawChannelContainer = 'TileRawChannelFixed'
else:
    jobproperties.TileRecFlags.TileRawChannelContainer = 'TileRawChannelOpt2'


# To read CTP RESULTS and DSP Raw Channels
if not hasattr(svcMgr, 'ByteStreamAddressProviderSvc'):
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
    svcMgr += ByteStreamAddressProviderSvc()
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ 'TileRawChannelContainer/TileRawChannelCnt'
                                                  , 'CTP_RDO/CTP_RDO'
                                                  , 'CTP_RIO/CTP_RIO' ]


#----------------
# create TileCell from TileRawChannel and store it in CaloCellContainer
#----------------
if doTileCells:
   
   # enable interpolation for dead cells
    doCaloNeighborsCorr = False
    if TileBiGainRun:
        include( "TileRec/TileCellMaker_jobOptions_doublegain.py" )
    else:
        include('TileRec/TileCellMaker_jobOptions.py')

    #----------------
    # create towers from TileCells
    #----------------
    if doTowers:
        include('TileMonitoring/TileMonTower_jobOptions.py')
        # CmbTowerBldr +=  TileCmbTwrBldr
        # CmbTowerBldr.TowerBuilderTools = [ TileCmbTwrBldr ]

    #----------------
    # create clusters from TileCells
    #----------------
    if doClusters:
        # include( 'CaloRec/CaloTopoCluster_jobOptions.py' )
        include('TileMonitoring/TileMonTopoCluster_jobOptions.py')      

    if doTileMuId:
        include ('TileMuId/TileMuId_cosmics_jobOptions.py')

    if doTileMuonFit:
        include('TileCosmicAlgs/TileMuonFitter_jobOptions.py')


if doTileTMDBRawChannel:
    # Set up TileCondToolPulseShape to be used in
    # TileCondToolOfc
    from TileConditions.TileCondToolConf import getTileCondToolMuRcvPulseShape
    ToolSvc += getTileCondToolMuRcvPulseShape('FILE', 'TileCondToolMuRcvPulseShape')
    
    # Set up TileCondToolOfc to be used in TileRawChannelBuilderMF
    ToolSvc += CfgMgr.TileCondToolOfc(name = 'TileCondToolMuRcvOfc'
                                      , OptFilterDeltaCorrelation = True
                                      , TileCondToolPulseShape = ToolSvc.TileCondToolMuRcvPulseShape)


    # Set up TileRawChannelBuilderOpt2 to be used
    ToolSvc += CfgMgr.TileRawChannelBuilderOpt2Filter(name = 'TileMuRcvRawChannelBuilderOpt2'
                                                      , TileRawChannelContainer = 'TileMuRcvRawChannelOpt2'
                                                      , PedestalMode = 1
                                                      , Minus1Iteration = TRUE
                                                      , calibrateEnergy = False
                                                      , correctTime = False
                                                      , TileCondToolOfc = ToolSvc.TileCondToolMuRcvOfc)


    # Set up TileRawChannelBuilderMF to be used
#    ToolSvc += CfgMgr.TileRawChannelBuilderMF(name = 'TileMuRcvRawChannelBuilderMF'
#                                              , MF = 1
#                                              , PedestalMode = 0
#                                              , DefaultPedestal = 30
#                                              , TileRawChannelContainer = 'TileMuRcvRawChannelMF'
#                                              , calibrateEnergy = False
#                                              , TileCondToolOfc = ToolSvc.TileCondToolMuRcvOfc)


    topSequence += CfgMgr.TileRawChannelMaker(name = 'TileMuRcvRChMaker'
                                              , TileDigitsContainer = 'MuRcvDigitsCnt'
                                              , TileRawChannelBuilder = [ ToolSvc.TileMuRcvRawChannelBuilderOpt2 ])

#----------------
# TileMonitoring
#----------------
if doMonitoring:
    if doTileNoiseMon:
        include('TileMonitoring/jobOptions_TileNoiseMon.py')
    else:
        include('TileMonitoring/TileMon_standalone_jobOptions.py')
   


#--------------------------------------------------------------
# ATLANTIS
#--------------------------------------------------------------
if doAtlantis:

    # #To read CTP RESULTS and more
    if not hasattr(svcMgr, 'ByteStreamAddressProviderSvc'):
        from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
        svcMgr += ByteStreamAddressProviderSvc()

    svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ 'ROIB::RoIBResult/RoIBResult'
                                                      , 'MuCTPI_RDO/MUCTPI_RDO'
                                                      , 'MuCTPI_RIO/MUCTPI_RIO'
                                                      , 'LVL_ROI/LVL_ROI' ]


    include('JiveXML/JiveXML_jobOptionBase.py')
    include ('TrigJiveXML/TrigJiveXML_DataTypes.py')

    theEventData2XML.WriteToFile = False

    from CaloJiveXML.CaloJiveXMLConf import JiveXML__CaloTileRetriever
    theCaloTileRetriever = JiveXML__CaloTileRetriever (name = 'CaloTileRetriever')
    theCaloTileRetriever.DoTileCellDetails = True
    theCaloTileRetriever.DoTileDigit = True
    ToolSvc += theCaloTileRetriever
    theEventData2XML.DataTypes += ['JiveXML::CaloTileRetriever/CaloTileRetriever']

    from CaloJiveXML.CaloJiveXMLConf import JiveXML__CaloMBTSRetriever
    theCaloMBTSRetriever = JiveXML__CaloMBTSRetriever (name = 'CaloMBTSRetriever')
    theCaloMBTSRetriever.DoMBTSDigits = True
    ToolSvc += theCaloMBTSRetriever
    theEventData2XML.DataTypes += ['JiveXML::CaloMBTSRetriever/CaloMBTSRetriever']


    if doClusters:
        from xAODJiveXML.xAODJiveXMLConf import JiveXML__xAODCaloClusterRetriever
        thexAODCaloClusterRetriever = JiveXML__xAODCaloClusterRetriever (name = 'xAODCaloClusterRetriever')
        thexAODCaloClusterRetriever.FavouriteClusterCollection = 'TileTopoCluster'
        thexAODCaloClusterRetriever.OtherClusterCollections = [ '' ]
        ToolSvc += thexAODCaloClusterRetriever
        theEventData2XML.DataTypes += ['JiveXML::xAODCaloClusterRetriever/xAODCaloClusterRetriever']
    

    if OnlineAtlantis:
        from JiveXML.JiveXMLConf import JiveXML__ONCRPCServerSvc
        svcMgr += JiveXML__ONCRPCServerSvc('ONCRPCServerSvc', OutputLevel = DEBUG)

        from JiveXML.JiveXMLConf import JiveXML__StreamToServerTool
        StreamToServerTool = JiveXML__StreamToServerTool(OutputLevel = DEBUG
                                                         , ServerService = svcMgr.ONCRPCServerSvc
                                                         , StreamName = 'Tile')

        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += StreamToServerTool
        theEventData2XML.StreamTools += [ StreamToServerTool ]


    else:
        from JiveXML.JiveXMLConf import JiveXML__StreamToFileTool
        theStreamToFileTool = JiveXML__StreamToFileTool(FileNamePrefix = OutputDirectory + '/JiveXML')
        ToolSvc += theStreamToFileTool
        theEventData2XML.StreamTools += [ theStreamToFileTool ]

    print theEventData2XML

#-----------------------
# Perfomance monitor
#-----------------------
if doPerfMon and not athenaCommonFlags.isOnline():

    from GaudiSvc.GaudiSvcConf import AuditorSvc
    theAuditorSvc = svcMgr.AuditorSvc
    theApp.AuditAlgorithms = True  
    theApp.AuditServices = True
    theApp.AuditTools = True  

    theAuditorSvc += CfgMgr.AlgErrorAuditor()
    theAuditorSvc += CfgMgr.ChronoAuditor()
    theAuditorSvc += CfgMgr.NameAuditor()
    theAuditorSvc += CfgMgr.MemStatAuditor()
    from PerfMonComps.PerfMonFlags import jobproperties
    jobproperties.PerfMonFlags.OutputFile = 'perfmon_ntuple.root'
    jobproperties.PerfMonFlags.doMonitoring = True
    jobproperties.PerfMonFlags.doDetailedMonitoring = True
    jobproperties.PerfMonFlags.doFastMon = False
    include('PerfMonComps/PerfMonSvc_jobOptions.py')


#-----------------------
# And some final options
#-----------------------

if OutputLevel < 2:
    from TileByteStream.TileByteStreamConf import TileROD_Decoder
    ToolSvc += TileROD_Decoder()
    ToolSvc.TileROD_Decoder.VerboseOutput = True
    # svcMgr.ByteStreamInputSvc.DumpFlag = True

svcMgr.MessageSvc.defaultLimit = MsgLinesLimit
svcMgr.MessageSvc.OutputLevel = OutputLevel
svcMgr.MessageSvc.Format = '% F%35W%S%7W%R%T %0W%M'
svcMgr.MessageSvc.useColors = useColors
# svcMgr.HistorySvc.OutputLevel = 3

theApp.EvtMax = EvtMax

from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
svcMgr += AthenaEventLoopMgr()
svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100

if TileUseCOOL:
    from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
    svcMgr += DBReplicaSvc(UseCOOLSQLite=False)
   # svcMgr.PoolSvc.SortReplicas=False
   
   # svcMgr.PoolSvc.ReadCatalog += ['xmlcatalog_file:/sw/DbSuppForMx/poolcond/PoolCat_comcond.xml']
   # svcMgr.PoolSvc.ReadCatalog += ['xmlcatalog_file:/det/lar/lar/project/databases/cond08_data.000001.lar.COND/PoolFileCatalog.xml']
   # svcMgr.PoolSvc.ReadCatalog += ['xmlcatalog_file:/det/lar/lar/project/databases/comcond.000006.lar_conditions.recon.pool.v0000/PoolFileCatalog.xml']
   # svcMgr.PoolSvc.ReadCatalog += ['xmlcatalog_file:/det/lar/lar/project/databases/cond09_data.000001.lar.COND/PoolFileCatalog.xml']
   # from LArConditionsCommon.LArCondFlags import larCondFlags 
   # larCondFlags.SingleVersion=True
   # larCondFlags.OFCShapeFolder = ''
   # larCondFlags.LArDBConnection.statusOn = False

# ToolSvc.TileCellMon.OutputLevel = 2
# ToolSvc.TileCellBuilder.OutputLevel = 2
# ToolSvc.TileRawChannelBuilderOptATLAS.OutputLevel = 1
# TileTopoCluster.OutputLevel=1
# TileTopoCluster.TileTopoMaker.OutputLevel=1
# TileTopoCluster.TileTopoSplitter.OutputLevel=1
# ToolSvc.CaloCellNeighborsAverageCorr.OutputLevel=1

#if hasattr (svcMgr.ToolSvc, 'TileCellMon'):
#    ToolSvc.TileCellMon.OutputLevel = DEBUG
#if hasattr (svcMgr.ToolSvc, 'TileCellBuilder'):
#    ToolSvc.TileCellBuilder.OutputLevel = DEBUG

if hasattr (svcMgr.ToolSvc, 'CaloNoiseToolDefault'):
    ToolSvc.CaloNoiseToolDefault.RescaleForHV = False

