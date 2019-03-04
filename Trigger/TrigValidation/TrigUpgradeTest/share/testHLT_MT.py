################################################################################
# TrigUpgradeTest/testHLT_MT.py
#
#   Job options for trigger MT tests to be used as first include
#   in signature specific job options. Example:
#
#      include('TrigUpgradeTest/testHLT_MT.py')
#      ... your job options ...
#
# Several flags are supported on the command line to steer these job options.
# See below for a complete list of all flags and their default value. If used
# in athena.py the auto-configuration is used to setup most flags correctly.
#
# Additional "modifiers" can be specified by using
#   -c "myModifier=True/False"
# Existing modifiers can be found in "TriggerRelease/python/Modifiers.py"
#
class opt :
    setupForMC       = None           # force MC setup
    setLVL1XML       = 'TriggerMenuMT/LVL1config_LS2_v1.xml' # 'TriggerMenu/LVL1config_Physics_pp_v7.xml' # default for legacy
    setDetDescr      = None           # force geometry tag
    setGlobalTag     = None           # force global conditions tag
    useCONDBR2       = True           # if False, use run-1 conditions DB
    condOverride     = {}             # overwrite conditions folder tags e.g. '{"Folder1":"Tag1", "Folder2":"Tag2"}'
    HLTOutputLevel   = INFO           # change OutputLevel of HLT relevant components
    doHLT            = True           # run HLT?
    doID             = True           # TriggerFlags.doID
    doCalo           = True           # TriggerFlags.doCalo
    doMuon           = True           # TriggerFlags.doMuon
    doDBConfig       = None           # dump trigger configuration
    trigBase         = None           # file name for trigger config dump
    enableCostD3PD   = False          # enable cost monitoring
    doL1Unpacking    = True           # decode L1 data in input file if True, else setup emulation
    doL1Sim          = False          # (re)run L1 simulation
#
################################################################################

import AthenaCommon.CfgMgr as CfgMgr
from AthenaCommon.AppMgr import theApp, ServiceMgr as svcMgr
from AthenaCommon.Logging import logging
log = logging.getLogger('testHLT_MT.py')

#-------------------------------------------------------------
# Setup options
#-------------------------------------------------------------
log.info('Setup options:')
defaultOptions = [a for a in dir(opt) if not a.startswith('__')]
for option in defaultOptions:
    if option in globals():
        setattr(opt, option, globals()[option])
        print ' %20s = %s' % (option, getattr(opt, option))
    else:        
        print ' %20s = (Default) %s' % (option, getattr(opt, option))

#-------------------------------------------------------------
# Setting Global Flags
#-------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.BeamFlags import jobproperties
from TriggerJobOpts.TriggerFlags import TriggerFlags
import TriggerRelease.Modifiers

# Input format and file for athena running
if len(athenaCommonFlags.FilesInput())>0:
    from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys, GetRunNumber
    ConfigureFromListOfKeys(['everything'])
    TriggerRelease.Modifiers._run_number = GetRunNumber()
else:
    if '_run_number' in dir(): TriggerRelease.Modifiers._run_number = _run_number   # set by athenaHLT
    globalflags.InputFormat = 'bytestream'  # default for athenaHLT
    if opt.setupForMC:
        globalflags.DetGeo = 'atlas'
        globalflags.DataSource = 'geant4'
    else:
        globalflags.DetGeo = 'commis'
        globalflags.DataSource = 'data'
        jobproperties.Beam.beamType = 'collisions'
        jobproperties.Beam.bunchSpacing = 25
        globalflags.DetDescrVersion = TriggerFlags.OnlineGeoTag()
        globalflags.ConditionsTag = TriggerFlags.OnlineCondTag()
        

# Overwrite tags if specified on command line
if opt.setDetDescr is not None:
    globalflags.DetDescrVersion = opt.setDetDescr
if opt.setGlobalTag is not None:
    globalflags.ConditionsTag = opt.setGlobalTag

globalflags.DatabaseInstance='CONDBR2' if opt.useCONDBR2 else 'COMP200'

athenaCommonFlags.isOnline = False # for the moment, run in offline mode

#-------------------------------------------------------------
# Transfer flags into TriggerFlags
#-------------------------------------------------------------

# To turn off HLT for athena running
TriggerFlags.doHLT = bool(opt.doHLT)
    
# To extract the Trigger configuration
TriggerFlags.Online.doDBConfig = bool(opt.doDBConfig)
if opt.trigBase is not None:
    TriggerFlags.Online.doDBConfigBaseName = opt.trigBase


# Setup list of modifiers
# Common modifiers for MC and data
setModifiers = ['noLArCalibFolders',
                'ForceMuonDataType',
                'useNewRPCCabling',
                #'enableCostMonitoring', 
                #'enableCoherentPS',
                'useOracle',
                'enableHotIDMasking',
                'openThresholdRPCCabling',
]

if globalflags.DataSource.is_geant4():  # MC modifiers
    setModifiers += ['BFieldFromDCS']
else:           # More data modifiers
    setModifiers += ['allowCOOLUpdates',
                     'BFieldAutoConfig',
                     'useHLTMuonAlign',
                     #Check for beamspot quality flag
                     'UseBeamSpotFlagForBjet',
                     'UseParamFromDataForBjet',
                     #Use online luminosity
                     'useOnlineLumi',
                     #for running with real data
                     'DisableMdtT0Fit',
                     #Setup mufast tuning for data
                     'UseLUTFromDataForMufast',
                     'UseRPCTimeDelayFromDataForMufast',
                     #Set muComb/muIso Backextrapolator tuned for real data
                     'UseBackExtrapolatorDataForMuIso',
                     #Monitoring for L1 muon group
                     #'muCTPicheck',
                     #Monitoring L1Topo at ROB level
                     #'L1TopoCheck',
                     'forceTileRODMap',
    ]

#make some more common trig cost operations easier to setup
if opt.enableCostD3PD:
    enableCostMonitoring = True # This goes without saying!
    enableCostForCAF = True # This sets the cost mon to monitor every event and always export the data.


TriggerFlags.doID = opt.doID
TriggerFlags.doMuon = opt.doMuon
TriggerFlags.doCalo = opt.doCalo

#-------------------------------------------------------------
# Modifiers
#-------------------------------------------------------------
modifierList=[]
from TrigConfigSvc.TrigConfMetaData import TrigConfMetaData
meta = TrigConfMetaData()
    
for mod in dir(TriggerRelease.Modifiers):
    if not hasattr(getattr(TriggerRelease.Modifiers,mod),'preSetup'): continue
    if mod in dir():  #allow turning on and off modifiers by variable of same name
        if globals()[mod]:
            if mod not in setModifiers:
                setModifiers+=[mod]
        else:
            if mod in setModifiers: setModifiers.remove(mod)
    if mod in setModifiers:
        modifierList+=[getattr(TriggerRelease.Modifiers,mod)()]
        meta.Modifiers += [mod]    # store in trig conf meta data
        setModifiers.remove(mod)

if setModifiers:
    log.error('Unknown modifier(s): '+str(setModifiers))


# never include this
include.block("RecExCond/RecExCommon_flags.py")

#-------------------------------------------------------------
# Setting DetFlags
#-------------------------------------------------------------
if globalflags.InputFormat.is_bytestream():
    TriggerFlags.doLVL1 = False

from AthenaCommon.DetFlags import DetFlags
if TriggerFlags.doLVL1():
    DetFlags.detdescr.all_setOn()
if TriggerFlags.doID():
    DetFlags.detdescr.ID_setOn()
    DetFlags.makeRIO.ID_setOn()
else:
    DetFlags.ID_setOff()
if TriggerFlags.doMuon():
    DetFlags.detdescr.Muon_setOn()
    DetFlags.makeRIO.all_setOn()
else:
    DetFlags.Muon_setOff()
if TriggerFlags.doCalo():
    DetFlags.detdescr.Calo_setOn()
    from LArConditionsCommon.LArCondFlags import larCondFlags
    larCondFlags.LoadElecCalib.set_Value_and_Lock(False)
else:
    DetFlags.Calo_setOff()

DetFlags.Print()

# RecEx flags
from RecExConfig.RecFlags import rec
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
rec.doESD = False
rec.doAOD = False
rec.doTruth = False

TriggerFlags.writeBS = True

#-------------------------------------------------------------
# Apply modifiers
#-------------------------------------------------------------
for mod in modifierList:
    mod.preSetup()

#--------------------------------------------------------------
# Conditions setup.
#--------------------------------------------------------------
from IOVDbSvc.CondDB import conddb #This import will also set up CondInputLoader
conddb.setGlobalTag(globalflags.ConditionsTag())

from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()

#--------------------------------------------------------------
# Thread-specific setup
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties
if jobproperties.ConcurrencyFlags.NumThreads() > 0:
    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )

# EventInfo creation if needed
from RecExConfig.ObjKeyStore import objKeyStore
if ( not objKeyStore.isInInput( "xAOD::EventInfo_v1") ) and ( not hasattr( topSequence, "xAODMaker::EventInfoCnvAlg" ) ):
    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    topSequence += xAODMaker__EventInfoCnvAlg()


# ----------------------------------------------------------------
# Detector geometry 
# ----------------------------------------------------------------
# Always enable AtlasFieldSvc
from AthenaCommon.DetFlags import DetFlags
DetFlags.BField_setOn()
include ("RecExCond/AllDet_detDescr.py")

from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
svcMgr += RegSelSvcDefault()

if TriggerFlags.doID():
    include( "InDetRecExample/InDetRecCabling.py" )

if TriggerFlags.doCalo():
    from TrigT2CaloCommon.TrigT2CaloCommonConfig import TrigDataAccess
    svcMgr.ToolSvc += TrigDataAccess()

if TriggerFlags.doMuon():
    import MuonCnvExample.MuonCablingConfig
    import MuonRecExample.MuonReadCalib
    if globalflags.InputFormat.is_pool():
        include( "MuonByteStreamCnvTest/jobOptions_MuonRDOToDigit.py" )

    include ("MuonRecExample/MuonRecLoadTools.py")

# ----------------------------------------------------------------
# Pool input
# ----------------------------------------------------------------
if globalflags.InputFormat.is_pool():
    import AthenaPoolCnvSvc.ReadAthenaPool   # noqa
    svcMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]
    svcMgr.PoolSvc.AttemptCatalogPatch=True 
     
# ----------------------------------------------------------------
# ByteStream input
# ----------------------------------------------------------------
elif globalflags.InputFormat.is_bytestream():

    # This is only needed running athena (as opposed to athenaHLT)
    if not hasattr(svcMgr,"ByteStreamCnvSvc"):
        from ByteStreamCnvSvc import ReadByteStream   # noqa
        # Define the input
        svcMgr.ByteStreamInputSvc.FullFileName = athenaCommonFlags.FilesInput()
        theApp.ExtSvc += [ "ByteStreamCnvSvc"]

    # Online specific setup of BS converters
    include( "TriggerRelease/jobOfragment_ReadBS_standalone.py" )    


# ---------------------------------------------------------------
# Trigger config
# ---------------------------------------------------------------
TriggerFlags.inputLVL1configFile = opt.setLVL1XML
TriggerFlags.readLVL1configFromXML = True
TriggerFlags.outputLVL1configFile = None
from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc, findFileInXMLPATH
svcMgr += LVL1ConfigSvc()
svcMgr.LVL1ConfigSvc.XMLMenuFile = findFileInXMLPATH(TriggerFlags.inputLVL1configFile())

if opt.doL1Sim:
    logLevel=DEBUG
    from TrigT1CaloSim.TrigT1CaloSimRun2Config import Run2TriggerTowerMaker
    caloTowerMaker              = Run2TriggerTowerMaker("Run2TriggerTowerMaker")
    caloTowerMaker.ZeroSuppress = True
    caloTowerMaker.CellType     = 3

    from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2CPMTowerMaker
    from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2JetElementMaker
    from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMSim
    from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMJetSim
    from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMEnergySim
    from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMX
    from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetCMX
    from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyCMX
    from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__RoIROD
    from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Tester

    from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS
    from TrigT1ZDC.TrigT1ZDCConf import LVL1__TrigT1ZDC

    
    from AthenaCommon.CFElements import seqAND
    
    l1CaloSim = seqAND('l1CaloSim',[
        caloTowerMaker,
        #LVL1__Run2CPMTowerMaker( 'CPMTowerMaker', ExtraInputs=["XYZ#1"], ExtraOutputs=["XYZ#2"]) ,
        LVL1__Run2CPMTowerMaker( 'CPMTowerMaker') ,
        LVL1__Run2JetElementMaker( 'JetElementMaker'),
        LVL1__CPMSim( 'CPMSim' ) ,
        LVL1__JEMJetSim( 'JEMJetSim' ) ,
        LVL1__JEMEnergySim( 'JEMEnergySim' ) ,
        LVL1__CPCMX( 'CPCMX' ) ,
        LVL1__JetCMX( 'JetCMX' ) ,
        LVL1__EnergyCMX( 'EnergyCMX' ) ,
        LVL1__RoIROD( 'RoIROD' ),
        LVL1__TrigT1MBTS(),
        LVL1__TrigT1ZDC()
    ])
    for a in l1CaloSim.Members:
        a.OutputLevel=logLevel
        
    from IOVDbSvc.CondDB import conddb
    L1CaloFolderList = []
    #L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib"]
    L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib"]
    #L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/RunParameters"]
    #L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/DerivedRunPars"]
    #L1CaloFolderList += ["/TRIGGER/Receivers/Conditions/VgaDac"]
    #L1CaloFolderList += ["/TRIGGER/Receivers/Conditions/Strategy"]
    L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Conditions/DisabledTowers"]
    L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/PpmDeadChannels"]
    L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults"]

    
    for l1calofolder in L1CaloFolderList:
        #conddb.addFolderWithTag("TRIGGER_OFL", l1calofolder, "HEAD")
        conddb.addFolder( "TRIGGER_OFL", l1calofolder )
    # muons
    from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigitTool
    MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool (DecodeMdtRDO = True,
                                                     DecodeRpcRDO = True,
                                                     DecodeTgcRDO = True,
                                                     DecodeCscRDO = True ) 
    
    MuonRdoToMuonDigitTool.cscCalibTool = ToolSvc.CscCalibTool

    ToolSvc += MuonRdoToMuonDigitTool

    from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigit
    from TrigT1RPCsteering.TrigT1RPCsteeringConf import TrigT1RPC    
    from TrigT1TGC.TrigT1TGCConf import LVL1TGCTrigger__LVL1TGCTrigger
    from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi
    from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool

    ToolSvc += L1MuctpiTool("L1MuctpiTool")
    ToolSvc.L1MuctpiTool.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc
    
    ToolSvc += L1MuctpiTool("LVL1MUCTPI__L1MuctpiTool") # one for topo, no idea why we need two
    ToolSvc.LVL1MUCTPI__L1MuctpiTool.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc
    
    


    muctpi             = L1Muctpi()
    muctpi.OutputLevel = logLevel
    muctpi.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc
    
    l1MuonSim = seqAND("l1MuonSim", [
        
        MuonRdoToMuonDigit( "MuonRdoToMuonDigit",
                            MuonRdoToMuonDigitTool = ToolSvc.MuonRdoToMuonDigitTool,
                            OutputLevel            = logLevel),
        
        TrigT1RPC("TrigT1RPC",
                  Hardware          = True, # not sure if needed, not there in old config, present in JO
                  DataDetail        = False,
                  RPCbytestream     = False,
                  RPCbytestreamFile = "", OutputLevel=logLevel),
        
        # based on Trigger/TrigT1/TrigT1TGC/python/TrigT1TGCConfig.py
        # interesting is that this JO sets inexisting properties, commented out below
        LVL1TGCTrigger__LVL1TGCTrigger("LVL1TGCTrigger",
                                       InputData_perEvent  = "TGC_DIGITS", 
                                      # ASDOutDataLocation = "ASDOutDataLocation",
                                      # MuonTrigConfig     = "/Run/MuonTrigConfig",
                                       MuCTPIInput_TGC     = "L1MuctpiStoreTGC",
                                       MaskFileName        = "TrigT1TGCMaskedChannel.db",
                                       MaskFileName12      = "TrigT1TGCMaskedChannel._12.db",
                                       OutputLevel         = logLevel),
        muctpi
    ])
    # only needed for MC
    conddb.addFolder("TGC_OFL", "/TGC/TRIGGER/CW_EIFI", className="CondAttrListCollection")
    conddb.addFolder("TGC_OFL", "/TGC/TRIGGER/CW_BW", className="CondAttrListCollection")
    conddb.addFolder("TGC_OFL", "/TGC/TRIGGER/CW_TILE", className="CondAttrListCollection")
    from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
    from TrigT1CTP.TrigT1CTPConfig import CTPSimulationInReco
    from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilder
    condSeq = AthSequencer("AthCondSeq")
    from MuonCondSvc.MuonCondSvcConf import TGCTriggerDbAlg
    condSeq += TGCTriggerDbAlg()

    ctp             = CTPSimulationInReco("CTPSimulation")
    ctp.DoLUCID     = False
    ctp.DoBCM       = False
    ctp.DoL1Topo    = False
    ctp.OutputLevel = logLevel
    ctp.TrigConfigSvc = svcMgr.LVL1ConfigSvc
    ctpSim      = seqAND("ctpSim", [ctp, RoIBuilder("RoIBuilder")])
    
    l1Sim = seqAND("l1Sim", [l1CaloSim, l1MuonSim, ctpSim] )
    
    topSequence += l1Sim

if opt.doL1Unpacking:
    if globalflags.InputFormat.is_bytestream():
        from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import RoIBResultByteStreamDecoderAlg
        from TrigUpgradeTest.TestUtils import L1DecoderTest
        topSequence += RoIBResultByteStreamDecoderAlg() # creates RoIBResult (input for L1Decoder) from ByteStream
        topSequence += L1DecoderTest(OutputLevel = DEBUG)
    elif opt.doL1Sim:
        from TrigUpgradeTest.TestUtils import L1DecoderTest
        topSequence += L1DecoderTest(OutputLevel = DEBUG)
    else:
        from TrigUpgradeTest.TestUtils import L1EmulationTest
        topSequence += L1EmulationTest(OutputLevel = opt.HLTOutputLevel)

# ---------------------------------------------------------------
# Monitoring
# ---------------------------------------------------------------
if not hasattr(svcMgr, 'THistSvc'):        
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr += THistSvc()
if hasattr(svcMgr.THistSvc, "Output"):
    from TriggerJobOpts.HLTTriggerGetter import setTHistSvcOutput
    setTHistSvcOutput(svcMgr.THistSvc.Output)

# -------------------------------------------------------------
# Message formatting and OutputLevel
# -------------------------------------------------------------
if TriggerFlags.Online.doValidation():
    TriggerFlags.enableMonitoring = TriggerFlags.enableMonitoring.get_Value()+['Log']
else:
    svcMgr.MessageSvc.Format = "%t  " + svcMgr.MessageSvc.Format   # add time stamp
    if hasattr(svcMgr.MessageSvc,'useErsError'):   # ERS forwarding with TrigMessageSvc
        svcMgr.MessageSvc.useErsError = ['*']
        svcMgr.MessageSvc.alwaysUseMsgStream = True

#-------------------------------------------------------------
# Apply modifiers
#-------------------------------------------------------------
for mod in modifierList:
    mod.postSetup()

#-------------------------------------------------------------
# Conditions overrides
#-------------------------------------------------------------    
if len(opt.condOverride)>0:
    for folder,tag in opt.condOverride.iteritems():
        log.warn('Overriding folder %s with tag %s' % (folder,tag))
        conddb.addOverride(folder,tag)

if svcMgr.MessageSvc.OutputLevel<INFO or opt.HLTOutputLevel<INFO:                
    from AthenaCommon.JobProperties import jobproperties
    jobproperties.print_JobProperties('tree&value')
    print svcMgr
