#==============================================================
#
# Job options file
#
# @file TrigT1CTMonitoringJobOptions.py
#
# @brief Monitoring for LVL1 Central Trigger bytestream.
#
# @author David Berge <berge@cern.ch>
#
#==============================================================
#==============================================================
# Merge of both TrigT1CTMonitoringJobOptions_forRecExCommission and TrigT1CTMonitoringJobOptions

## get a handle to the ServiceManager
#-----------COMMON CODE---------------------

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *

LHCFillStateAvailable=False
UsedFillStateCoolFolderName="/LHC/DCS/FILLSTATE"
if LHCFillStateAvailable:
    UsedFillStateCoolFolderName="Unavailable"

if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc 
    svcMgr += ByteStreamAddressProviderSvc()

svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
    "ROIB::RoIBResult/RoIBResult",
    "MuCTPI_RDO/MUCTPI_RDO",
    "CTP_RDO/CTP_RDO",
    "MuCTPI_RIO/MUCTPI_RIO",
    "CTP_RIO/CTP_RIO"
    ]

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#---------------------------------------------------------------
# Central-Trigger Monitoring
#---------------------------------------------------------------
from TrigT1CTMonitoring.TrigT1CTMonitoringConfig import *
## get a handle on the ToolSvc
from AthenaCommon.AppMgr import ToolSvc as toolSvc
from TrigT1CTMonitoring.TrigT1CTMonitoringConf import TrigT1CTMonitoring__BSMonitoring as BSMon

#-----------ONLINE CODE---------------------
if isOnline:
    #from TrigServices.TrigServicesConf import TrigMonTHistSvc
    #THistSvc = TrigMonTHistSvc("THistSvc") 
    #svcMgr += THistSvc 
    doHist=True
    InputDir = "."
    OutputDir="."
    FilePreFix = [ "MonTest"]
    RootHistFileName = 'HistFile.root'
    
    ## add an AthenaMonManager algorithm to the list of algorithms to be ran
    if not hasattr(topSequence,"PrimaryManager"):
        from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
        topSequence += AthenaMonManager( "PrimaryManager" )
    ## AthenaMonManager is the Algorithm that manages many classes inheriting
    ## from ManagedMonitorToolBase
    CTmonMan = topSequence.PrimaryManager

    theApp.Dlls += [ "TrigT1CTMonitoring" ]
    
    CTmonMan.AthenaMonTools += [ "TrigT1CTMonitoring__BSMonitoring/BSMon" ]
   
    ## FILEKEY must match that given to THistSvc
    CTmonMan.FileKey = "GLOBAL"

    DetDescrVersion="ATLAS-DC3-05" # 11.0.42 files 
    AllAlgs = False # if false, all algorithms are switched off by defaults 


    from AthenaCommon.GlobalFlags import GlobalFlags
    GlobalFlags.DataSource.set_data()
    GlobalFlags.InputFormat.set_bytestream()
    GlobalFlags.DetGeo.set_atlas()
    
    from AthenaCommon.DetFlags import DetFlags
    
    DetFlags.detdescr.all_setOn()
    DetFlags.geometry.all_setOn()
    DetFlags.ID_setOff()
    DetFlags.Calo_setOff() 
    DetFlags.Truth_setOff()
    #switch on/off detectors
   
    DetFlags.Muon_setOn()
    #DetFlags.Muon_setOff()
   
    DetFlags.LVL1_setOn()
    DetFlags.pileup.all_setOff()
    DetFlags.simulate.all_setOff()
    DetFlags.digitize.all_setOff()
    DetFlags.makeRIO.all_setOff()
    DetFlags.writeBS.all_setOff()
    DetFlags.readRDOBS.all_setOff()
    DetFlags.readRIOBS.all_setOff()
    DetFlags.readRIOPool.all_setOff()
    DetFlags.writeRIOPool.all_setOff()
    DetFlags.writeRDOPool.all_setOff()
    DetFlags.readRDOBS.LVL1_setOn()
    DetFlags.readRIOBS.LVL1_setOn()
    DetFlags.Print()
    GlobalFlags.Print()
    
    #--------------------------------------------------------------
    # GeoModel stuff:
    #--------------------------------------------------------------
    include( "AtlasGeoModel/SetGeometryVersion.py" )
    include( "AtlasGeoModel/GeoModelInit.py" )

    #--------------------------------------------------------------
    # 
    #--------------------------------------------------------------
    include( "AmdcAth/AmdcAth_jobOptions.py" )
    include( "AtlasGeoModel/MuonGeoModel.py" )

    # Cabling and geometry of muon detectors
    from RPCgeometry.RPCgeometryConfig import RPCgeometryConfig
    from RPCcabling.RPCcablingConfig import RPCcablingConfig
    RPCcablingSvc = RPCcablingConfig()
    RPCcablingSvc.ConfFileName = "LVL1conf.data"
    RPCcablingSvc.CorrFileName = "LVL1corr.data"
    
    from MDTcabling.MDTcablingConfig import MDTcablingConfig
    MDTcablingSvc = MDTcablingConfig()
    MDTcablingSvc.RODfile    = "RODmap.data"
    from TGCcabling.TGCcablingConfig import TGCcablingConfig
    
    # Needed to decode the RoI information
    from TrigT1RPCRecRoiSvc.TrigT1RPCRecRoiConfig import RPCRecRoiConfig
    from TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiConfig import TGCRecRoiConfig

    CTmonMan.Environment = "online" 
    CTmonMan.ManualDataTypeSetup = False
    CTmonMan.DataType            = "cosmics"
    CTmonMan.ManualRunLBSetup    = False
    CTmonMan.Run                 = 1
    CTmonMan.LumiBlock           = 1
    
    #---------------------------------------------------------------
    # LVL1 configuration
    #---------------------------------------------------------------
    log.info("will setup LVL1ConfigSvc and add instance to ServiceMgr")
    from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
    LVL1ConfigSvc = LVL1ConfigSvc('LVL1ConfigSvc')
    LVL1ConfigSvc.ConfigSource = "XML"
    LVL1ConfigSvc.XMLFile = "L1MenuM5.xml"
    LVL1ConfigSvc.CreateLegacyObjects = True
    LVL1ConfigSvc.DumpTTVmap = False
    #LVL1ConfigSvc.OutputLevel = VERBOSE
    svcMgr += LVL1ConfigSvc
    theApp.CreateSvc += [ "TrigConf::LVL1ConfigSvc/LVL1ConfigSvc" ]
   
    #    svcMgr.ToolSvc += BSMon( OutputLevel=INFO )
    svcMgr.ToolSvc += BSMon( ProcessMuctpiData=True )
    svcMgr.ToolSvc += BSMon( ProcessMuctpiDataRIO=True )
    svcMgr.ToolSvc += BSMon( ProcessCTPData=True )
    svcMgr.ToolSvc += BSMon( ProcessRoIBResult=True )
    svcMgr.ToolSvc += BSMon( InclusiveTriggerThresholds=True )
    svcMgr.ToolSvc += BSMon( FillStateCoolFolderName=UsedFillStateCoolFolderName)
    #    RecMuCTPIByteStreamTool.OutputLevel = INFO #DEBUG
    
    print topSequence
    print svcMgr


   
#-----------OFFLINE CODE---------------------
if not isOnline:

    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg = TriggerConfigGetter()

    ## add pre algorithms for rerunning CTP simulation
    if not 'IS_SIMULATION' in inputFileSummary['evt_type']:
        #svcMgr.DSConfigSvc.readLVL1Thr=True
        #svcMgr.DSConfigSvc.readLVL1BG=True
        
        from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi_on_Data
        topSequence += L1Muctpi_on_Data()

        from TrigT1CTMonitoring.TrigT1CTMonitoringConf import TrigT1CTMonitoring__DeriveSimulationInputs as DeriveSimulationInputs
        topSequence += DeriveSimulationInputs(do_MuCTPI_input=True,
                                              do_L1Calo_sim=False)
        
        from TrigT1CTP.TrigT1CTPConfig import CTPSimulationOnData
        topSequence += CTPSimulationOnData("CTPSimulation")

        from AthenaMonitoring.DQMonFlags import DQMonFlags
        histbase = "/" + DQMonFlags.monManFileKey() + "/"
        if DQMonFlags.monManRun():
            histbase += "run_RUNNR/"
        try:
            topSequence.CTPSimulation.HistBase = histbase
        except AttributeError, ex:
            print ex," ignore for now"


    ## AthenaMonManager is the Algorithm that manages many classes inheriting
    ## from ManagedMonitorToolBase

    ## add an AthenaMonManager algorithm to the list of algorithms to be run
    monMan = AthenaMonManager( name="CTMonManager",
                               FileKey             = DQMonFlags.monManFileKey(),
                               Environment         = DQMonFlags.monManEnvironment(),
                               ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                               DataType            = DQMonFlags.monManDataType() )
    topSequence += monMan
   
    theApp.Dlls += [ "TrigT1CTMonitoring" ]
    
    
    # check if global muons are on

    if not rec.doMuon:
        if not 'IS_SIMULATION' in inputFileSummary['evt_type']:
            CTBSMonTool = BSMon( ProcessCTPData = True,
                                 ProcessRoIBResult = False,
                                 InclusiveTriggerThresholds = False,
                                 ProcessMuctpiData = False,
                                 ProcessMuctpiDataRIO = False,
                                 CompareRerun = True,
                                 FillStateCoolFolderName=UsedFillStateCoolFolderName)
        
        if 'IS_SIMULATION' in inputFileSummary['evt_type']:
            CTBSMonTool = BSMon( ProcessCTPData = True,
                                 ProcessRoIBResult = False,
                                 InclusiveTriggerThresholds = False,
                                 ProcessMuctpiData = False,
                                 ProcessMuctpiDataRIO = False,
                                 RunOnESD = True,
                                 CompareRerun = False,
                                 FillStateCoolFolderName=UsedFillStateCoolFolderName)
        
    if rec.doMuon:
        if not 'IS_SIMULATION' in inputFileSummary['evt_type']:
            CTBSMonTool = BSMon( ProcessCTPData = True,
                                 ProcessRoIBResult = True,
                                 ProcessMuctpiData = True,
                                 ProcessMuctpiDataRIO = True,
                                 CompareRerun = True)

        if 'IS_SIMULATION' in inputFileSummary['evt_type']:
            CTBSMonTool = BSMon( ProcessCTPData = True,
                                 ProcessRoIBResult = True,
                                 ProcessMuctpiData = True,
                                 ProcessMuctpiDataRIO = False,
                                 RunOnESD = True,
                                 CompareRerun = False,
                                 FillStateCoolFolderName=UsedFillStateCoolFolderName)
        # Needed to decode the RoI information
        from TrigT1RPCRecRoiSvc.TrigT1RPCRecRoiConfig import RPCRecRoiConfig
        from TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiConfig import TGCRecRoiConfig

    processByteStream = True

    from RecExConfig.InputFilePeeker import inputFileSummary
    if not 'IS_SIMULATION' in inputFileSummary['evt_type']:
        from IOVDbSvc.CondDB import conddb
        conddb.addFolder('TRIGGER', '/TRIGGER/LUMI/LBLB') 
        #conddb.addFolder('TRIGGER', "/TRIGGER/LVL1/BunchGroupContent") # already added by some other alg
        if LHCFillStateAvailable:
            conddb.addFolder('DCS_OFL', "/LHC/DCS/FILLSTATE")
        conddb.addFolder('TDAQ', '/TDAQ/RunCtrl/DataTakingMode', className='AthenaAttributeList')
        conddb.addFolder('TRIGGER', "/TRIGGER/LVL1/RFPhase")
        conddb.addFolder('TRIGGER', '/TRIGGER/LVL1/CTPCoreInputMapping')

    # monMan.AthenaMonTools += [ "TrigT1CTMonitoring__BSMonitoring/BSMon" ]
    #ToolSvc += CTBSMonTool
    monMan.AthenaMonTools += [ CTBSMonTool ]
