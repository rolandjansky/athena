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
    FilesInput       = None           # Input files (string or list of strings)
    setupForMC       = None           # force MC setup
    setLVL1XML       = 'TriggerMenuXML/LVL1config_Physics_pp_v7.xml'
    setDetDescr      = None           # force geometry tag
    setGlobalTag     = None           # force global conditions tag
    useCONDBR2       = True           # if False, use run-1 conditions DB
    condOverride     = {}             # overwrite conditions folder tags e.g. '{"Folder1":"Tag1", "Folder2":"Tag2"}'
    HLTOutputLevel   = INFO           # change OutputLevel of HLT relevant components
    doHLT            = True           # run HLT?
    doID             = True           # TriggerFlags.doID
    doCalo           = True           # TriggerFlags.doCalo
    doMuon           = True           # TriggerFlags.doMuon
    enableViews      = True           # setup infrastructre for Views
    doDBConfig       = None           # dump trigger configuration
    trigBase         = None           # file name for trigger config dump
    enableCostD3PD   = False          # enable cost monitoring
    EvtMax           = -1             # events to process
    SkipEvents       =  0             # events to skip
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
if opt.FilesInput is not None:
    athenaCommonFlags.FilesInput = [opt.FilesInput] if isinstance(opt.FilesInput,str) else opt.FilesInput
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
                'softTRTsettings',
                'openThresholdRPCCabling',
]

if globalflags.DataSource.is_geant4():  # MC modifiers
    setModifiers += ['BFieldFromDCS']
else:           # More data modifiers
    setModifiers += ['allowCOOLUpdates',
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
from IOVSvc.IOVSvcConf import CondSvc 
svcMgr += CondSvc()

from AthenaCommon.AlgSequence import AthSequencer 
condSeq = AthSequencer("AthCondSeq") 

from IOVSvc.IOVSvcConf import CondInputLoader 
condSeq += CondInputLoader("CondInputLoader") 

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(globalflags.ConditionsTag())

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Thread-specific setup
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties
if jobproperties.ConcurrencyFlags.NumThreads() > 0:
    from SGComps.SGCompsConf import SGInputLoader
    topSequence += SGInputLoader( OutputLevel = INFO, 
                                  ShowEventDump = False,
                                  FailIfNoProxy = False )
    #topSequence.SGInputLoader.Load = [ ('ROIB::RoIBResult','StoreGateSvc+RoIBResult') ]

    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )
    AlgScheduler.setDataLoaderAlg( 'SGInputLoader' )

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

if globalflags.InputFormat.is_bytestream():
    from TrigUpgradeTest.TestUtils import L1DecoderTest
    #topSequence += L1DecoderTest(OutputLevel = opt.HLTOutputLevel)
    topSequence += L1DecoderTest(OutputLevel = DEBUG)
else:
    from TrigUpgradeTest.TestUtils import L1EmulationTest
    topSequence += L1EmulationTest(OutputLevel = opt.HLTOutputLevel)

# ----------------------------------------------------------------
# Setup Views
# ----------------------------------------------------------------
if opt.enableViews:
    log.info('Setting up Views...')
    # Make a separate alg pool for the view algs
    from GaudiHive.GaudiHiveConf import AlgResourcePool
    svcMgr += AlgResourcePool('ViewAlgPool')
    #Create IdentifiableCaches
    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__CacheCreator
    InDetCacheCreatorTrigViews = InDet__CacheCreator(name = "InDetCacheCreatorTrigViews",
                                        Pixel_ClusterKey = "PixelTrigClustersCache",
                                        SCT_ClusterKey   = "SCT_ClustersCache",
                                        SpacePointCachePix = "PixelSpacePointCache",
                                        SpacePointCacheSCT   = "SctSpacePointCache",
                                        OutputLevel=DEBUG)
    topSequence += InDetCacheCreatorTrigViews    
    
    # Set of view algs
    allViewAlgs = AthSequencer( "allViewAlgorithms" )
    allViewAlgs.ModeOR = False
    allViewAlgs.Sequential = True
    allViewAlgs.StopOverride = False
    topSequence += allViewAlgs
    
    # Filter to stop view algs from running on whole event
    allViewAlgs += CfgMgr.AthPrescaler( "alwaysFail" )
    allViewAlgs.alwaysFail.PercentPass = 0.0

    # dummy alg that just says you're running in a view
    # allViewAlgs += CfgMgr.AthViews__ViewTestAlg( "viewTest" )
    # svcMgr.ViewAlgPool.TopAlg += [ "viewTest" ]
    # viewMaker.AlgorithmNameSequence = [ "viewTest" ] #Eventually scheduler will do this
else:
    #This is to workaround the problem CondHandle bug, this can be removed once a proper solution is made
    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__CacheCreator
    InDetCacheCreatorTrigViews = InDet__CacheCreator(name = "InDetCacheCreatorTrigViews",
                                        Pixel_ClusterKey = "",
                                        SCT_ClusterKey   = "",
                                        SpacePointCachePix = "",
                                        SpacePointCacheSCT   = "",
                                        OutputLevel=INFO)
    topSequence += InDetCacheCreatorTrigViews    
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

# ----------------------------------------------------------------
# Number of events to be processed - for athena
# ----------------------------------------------------------------
theApp.EvtMax = opt.EvtMax
if hasattr(svcMgr,"EventSelector"):
    svcMgr.EventSelector.SkipEvents = opt.SkipEvents 
    
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
