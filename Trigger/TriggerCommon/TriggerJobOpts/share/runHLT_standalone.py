# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
################################################################################
# TriggerJobOpts/runHLT_standalone.py
#
#   Job options to run HLT standalone in AthenaMT
#
#   The file can be used as an include in other JO:
#      include('TriggerJobOpts/runHLT_standalone.py')
#   or directly to run the HLT:
#      athena --threads=1 --filesInput=input.pool.root TriggerJobOpts/runHLT_standalone.py
#
# Several flags are supported on the command line to steer these job options, for example
# by adding -c "setMenu='MC_pp_run3_v1'". See below for a complete list of all flags and their default
# value. If used in athena.py the auto-configuration is used to setup most flags correctly.
#
# Additional "modifiers" can be specified by using
#   -c "myModifier=True/False"
# Existing modifiers can be found in "TriggerJobOpts/python/Modifiers.py"
#
class opt:
    setupForMC       = None           # force MC setup
    setMenu          = 'LS2_v1'
    setDetDescr      = None           # force geometry tag
    setGlobalTag     = None           # force global conditions tag
    useCONDBR2       = True           # if False, use run-1 conditions DB
    condOverride     = {}             # overwrite conditions folder tags e.g. '{"Folder1":"Tag1", "Folder2":"Tag2"}'
    doHLT            = True           # run HLT?
    doID             = True           # TriggerFlags.doID
    doCalo           = True           # TriggerFlags.doCalo
    doMuon           = True           # TriggerFlags.doMuon
    doDBConfig       = None           # dump trigger configuration
    trigBase         = None           # file name for trigger config dump
    enableCostD3PD   = False          # enable cost monitoring
    doWriteRDOTrigger = False         # Write out RDOTrigger?
    doWriteBS        = True           # Write out BS?
    doL1Unpacking    = True           # decode L1 data in input file if True, else setup emulation
    doL1Sim          = False          # (re)run L1 simulation
    isOnline         = True           # isOnline flag
    doEmptyMenu      = False          # Disable all chains, except those re-enabled by specific slices
    createHLTMenuExternally = False   # Set to True if the menu is build manually outside runHLT_standalone.py
    endJobAfterGenerate = False       # Finish job after menu generation
    failIfNoProxy     = False         # Sets the SGInputLoader.FailIfNoProxy property
    forceEnableAllChains = False      # if True, all HLT chains will run even if the L1 item is false
    enableL1Phase1   = False          # Enable Run-3 LVL1 simulation and/or decoding
    enableL1CaloLegacy = True         # Enable Run-2 L1Calo simulation and/or decoding (possible even if enablePhase1 is True)
#Individual slice flags
    doCalibSlice        = True
    doTestSlice         = True
    doHeavyIonSlice     = True
    doEnhancedBiasSlice = True
    doEgammaSlice     = True
    doMuonSlice       = True
    doMinBiasSlice    = True
    doJetSlice        = True
    doMETSlice        = True
    doBjetSlice       = True
    doTauSlice        = True
    doCombinedSlice   = True
    doBphysicsSlice   = True
    doStreamingSlice  = True
    doMonitorSlice    = True
    doBeamspotSlice   = True
    doCosmicSlice     = True
    reverseViews      = False
    filterViews       = False
    enabledSignatures = []
    disabledSignatures = []
    selectChains      = []


#
################################################################################
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper, conf2toConfigurable
from AthenaCommon.AppMgr import theApp, ServiceMgr as svcMgr
from AthenaCommon.Include import include
from AthenaCommon.Logging import logging
from AthenaCommon import Constants
log = logging.getLogger('runHLT_standalone.py')

#-------------------------------------------------------------
# Setup options
#-------------------------------------------------------------
log.info('Setup options:')
defaultOptions = [a for a in dir(opt) if not a.startswith('__')]
for option in defaultOptions:
    if option in globals():
        setattr(opt, option, globals()[option])
        print(' %20s = %s' % (option, getattr(opt, option)))
    else:
        print(' %20s = (Default) %s' % (option, getattr(opt, option)))


import re

sliceRe = re.compile("^do.*Slice")
slices = [a for a in dir(opt) if sliceRe.match(a)]
if opt.doEmptyMenu is True:
    log.info("Disabling all slices")
    for s in slices:
        if s in globals():
            log.info("re-enabling %s ", s)
            setattr(opt, s, globals()[s])
        else:
            setattr(opt, s, False)
else:
    for s in slices:
        if s in globals():
            setattr(opt, s, globals()[s])

# Setting the TriggerFlags.XXXSlice to use in TriggerMenuMT
# This is temporary and will be re-worked for after M3.5
for s in slices:
    signature = s[2:].replace('Slice', '')

    if eval('opt.'+s) is True:
        enabledSig = 'TriggerFlags.'+signature+'Slice.setAll()'
        opt.enabledSignatures.append( enabledSig )
    else:
        disabledSig = 'TriggerFlags.'+signature+'Slice.setAll()'
        opt.disabledSignatures.append( disabledSig )

#-------------------------------------------------------------
# Setting Global Flags
#-------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.BeamFlags import jobproperties
import TriggerJobOpts.Modifiers

# Auto-configuration for athena
if len(athenaCommonFlags.FilesInput())>0:
    ConfigFlags.Input.Files = athenaCommonFlags.FilesInput()
    import PyUtils.AthFile as athFile
    af = athFile.fopen(athenaCommonFlags.FilesInput()[0])
    globalflags.InputFormat = 'bytestream' if af.fileinfos['file_type']=='bs' else 'pool'
    globalflags.DataSource = 'data' if af.fileinfos['evt_type'][0]=='IS_DATA' else 'geant4'
    ConfigFlags.Input.isMC = False if globalflags.DataSource=='data' else True
    # Set isOnline=False for MC inputs unless specified in the options
    if globalflags.DataSource() != 'data' and 'isOnline' not in globals():
        log.info("Setting isOnline = False for MC input")
        opt.isOnline = False
    # Set geometry and conditions tags
    if opt.setDetDescr is None:
        opt.setDetDescr = af.fileinfos.get('geometry',None)
    if opt.setGlobalTag is None:
        opt.setGlobalTag = af.fileinfos.get('conditions_tag',None) or \
            (TriggerFlags.OnlineCondTag() if opt.isOnline else 'CONDBR2-BLKPA-2018-13')
    TriggerJobOpts.Modifiers._run_number = af.fileinfos['run_number'][0]

else:   # athenaHLT
    globalflags.InputFormat = 'bytestream'
    globalflags.DataSource = 'data' if not opt.setupForMC else 'data'
    ConfigFlags.Input.isMC = False
    if '_run_number' not in dir():
        import PyUtils.AthFile as athFile
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        af = athFile.fopen(athenaCommonFlags.BSRDOInput()[0])
        _run_number = af.run_number[0]

    TriggerJobOpts.Modifiers._run_number = _run_number   # noqa, set by athenaHLT

    from RecExConfig.RecFlags import rec
    rec.RunNumber =_run_number
    del _run_number

ConfigFlags.Input.Format = 'BS' if globalflags.InputFormat=='bytestream' else 'POOL'


# Set final Cond/Geo tag based on input file, command line or default
globalflags.DetDescrVersion = opt.setDetDescr or TriggerFlags.OnlineGeoTag()
ConfigFlags.GeoModel.AtlasVersion = globalflags.DetDescrVersion()
globalflags.ConditionsTag = opt.setGlobalTag or TriggerFlags.OnlineCondTag()
ConfigFlags.IOVDb.GlobalTag = globalflags.ConditionsTag()

# Other defaults
jobproperties.Beam.beamType = 'collisions'
ConfigFlags.Beam.Type = jobproperties.Beam.beamType()
jobproperties.Beam.bunchSpacing = 25
globalflags.DatabaseInstance='CONDBR2' if opt.useCONDBR2 else 'COMP200'
ConfigFlags.IOVDb.DatabaseInstance=globalflags.DatabaseInstance()
athenaCommonFlags.isOnline.set_Value_and_Lock(opt.isOnline)

log.info('Configured the following global flags:')
globalflags.print_JobProperties()

# Set default doL1Sim option depending on input type (if not set explicitly)
if 'doL1Sim' not in globals():
    opt.doL1Sim = globalflags.DataSource != 'data'
    log.info('Setting default doL1Sim=%s because globalflags.DataSource=%s', opt.doL1Sim, globalflags.DataSource())

# Translate opts to flags for LVL1
ConfigFlags.Trigger.doLVL1 = opt.doL1Sim
ConfigFlags.Trigger.enableL1Phase1 = opt.enableL1Phase1
ConfigFlags.Trigger.enableL1CaloLegacy = opt.enableL1CaloLegacy

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
                'enableCostMonitoring',
                #'enableCoherentPS',
                'useOracle',
                'enableHotIDMasking',
]

if globalflags.DataSource.is_geant4():  # MC modifiers
    setModifiers += ['BFieldFromDCS']
else:           # More data modifiers
    setModifiers += ['allowCOOLUpdates',
                     'BFieldAutoConfig',
                     'useDynamicAlignFolders',
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

TriggerFlags.doID = ConfigFlags.Trigger.doID = opt.doID
TriggerFlags.doMuon = ConfigFlags.Trigger.doMuon = opt.doMuon
TriggerFlags.doCalo = ConfigFlags.Trigger.doCalo = opt.doCalo

#-------------------------------------------------------------
# Modifiers
#-------------------------------------------------------------
modifierList=[]
from TrigConfigSvc.TrigConfMetaData import TrigConfMetaData
meta = TrigConfMetaData()

for mod in dir(TriggerJobOpts.Modifiers):
    if not hasattr(getattr(TriggerJobOpts.Modifiers,mod),'preSetup'):
        continue
    if mod in dir():  #allow turning on and off modifiers by variable of same name
        if globals()[mod]:
            if mod not in setModifiers:
                setModifiers+=[mod]
        elif mod in setModifiers:
                setModifiers.remove(mod)
    if mod in setModifiers:
        modifierList+=[getattr(TriggerJobOpts.Modifiers,mod)()]
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
    from TrigT2CaloCommon.CaloDef import setMinimalCaloSetup
    setMinimalCaloSetup()
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

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Increase scheduler checks and verbosity
#--------------------------------------------------------------
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.CheckDependencies( True )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.EnableVerboseViews( True )

#--------------------------------------------------------------
# Set the FailIfNoProxy property of SGInputLoader
#--------------------------------------------------------------
if not hasattr(topSequence,"SGInputLoader"):
    log.error('Cannot set FailIfNoProxy property because SGInputLoader not found in topSequence')
    theApp.exit(1)
topSequence.SGInputLoader.FailIfNoProxy = opt.failIfNoProxy


#--------------------------------------------------------------
# Event Info setup
#--------------------------------------------------------------
# If no xAOD::EventInfo is found in a POOL file, schedule conversion from old EventInfo
if globalflags.InputFormat.is_pool():
    from RecExConfig.ObjKeyStore import objKeyStore
    from PyUtils.MetaReaderPeeker import convert_itemList
    objKeyStore.addManyTypesInputFile(convert_itemList(layout='#join'))
    if objKeyStore.isInInput("xAOD::EventInfo"):
        topSequence.SGInputLoader.Load += [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
    else:
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, "xAODMaker::EventInfoCnvAlg"):
            from xAODEventInfoCnv.xAODEventInfoCnvAlgDefault import xAODEventInfoCnvAlgDefault
            xAODEventInfoCnvAlgDefault(sequence=condSeq)
else:
    topSequence.SGInputLoader.Load += [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]

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
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.doPrintConfigurables = log.getEffectiveLevel() <= logging.DEBUG
    include( "InDetRecExample/InDetRecCabling.py" )

if TriggerFlags.doCalo():
    from TrigT2CaloCommon.TrigT2CaloCommonConfig import TrigDataAccess
    svcMgr.ToolSvc += TrigDataAccess()
    if globalflags.InputFormat.is_pool():
        TriggerFlags.doTransientByteStream = True # enable transient BS if TrigDataAccess is used with pool data

if TriggerFlags.doMuon():
    TriggerFlags.MuonSlice.doTrigMuonConfig=True
    import MuonCnvExample.MuonCablingConfig  # noqa: F401
    import MuonRecExample.MuonReadCalib      # noqa: F401

    include ("MuonRecExample/MuonRecLoadTools.py")

# ---------------------------------------------------------------
# ID conditions
# ---------------------------------------------------------------
if TriggerFlags.doID:
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    InDetTrigFlags.doPixelClusterSplitting = False

    # PixelLorentzAngleSvc and SCTLorentzAngleSvc
    from AthenaCommon.Include import include
    include("InDetRecExample/InDetRecConditionsAccess.py")

# ----------------------------------------------------------------
# Pool input
# ----------------------------------------------------------------
if globalflags.InputFormat.is_pool():
    import AthenaPoolCnvSvc.ReadAthenaPool   # noqa
    svcMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]
    svcMgr.PoolSvc.AttemptCatalogPatch=True
    # enable transient BS
    if TriggerFlags.doTransientByteStream():
        log.info("setting up transient BS")
        include( "TriggerJobOpts/jobOfragment_TransBS_standalone.py" )


# ----------------------------------------------------------------
# ByteStream input
# ----------------------------------------------------------------
elif globalflags.InputFormat.is_bytestream() and not ConfigFlags.Trigger.Online.isPartition:
    if hasattr(svcMgr, "MetaDataSvc"):
        # Need to set this property to ensure correct merging with MetaDataSvc from AthenaPoolCnvSvc/AthenaPool.py
        # May be removed when the merging is fixed (or AthenaPool.py sets this property)
        svcMgr.MetaDataSvc.MetaDataContainer = "MetaDataHdr"
    # Set up ByteStream reading services
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    CAtoGlobalWrapper(ByteStreamReadCfg, ConfigFlags)

# ---------------------------------------------------------------
# Trigger config
# ---------------------------------------------------------------
ConfigFlags.Trigger.triggerMenuSetup = TriggerFlags.triggerMenuSetup = opt.setMenu
TriggerFlags.readLVL1configFromXML = True
TriggerFlags.outputLVL1configFile = None

from TrigConfigSvc.TrigConfigSvcCfg import generateL1Menu, createL1PrescalesFileFromMenu
generateL1Menu(ConfigFlags)
createL1PrescalesFileFromMenu(ConfigFlags)

from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
CAtoGlobalWrapper(L1ConfigSvcCfg,ConfigFlags)

# ---------------------------------------------------------------
# HLT prep: RoIBResult and L1Decoder
# ---------------------------------------------------------------

# main HLT top sequence
from AthenaCommon.CFElements import seqOR,parOR
hltTop = seqOR("HLTTop")
hltBeginSeq = parOR("HLTBeginSeq")
hltTop += hltBeginSeq
topSequence += hltTop

if opt.doL1Unpacking:
    if globalflags.InputFormat.is_bytestream() or opt.doL1Sim:
        ConfigFlags.Trigger.L1Decoder.forceEnableAllChains = opt.forceEnableAllChains
        from L1Decoder.L1DecoderConfig import L1DecoderCfg
        CAtoGlobalWrapper(L1DecoderCfg, ConfigFlags, seqName="HLTBeginSeq")
    else:
        from TrigUpgradeTest.TestUtils import L1EmulationTest
        hltBeginSeq += L1EmulationTest()

# ---------------------------------------------------------------
# Level 1 simulation
# ---------------------------------------------------------------
if opt.doL1Sim:
    from TriggerJobOpts.Lvl1SimulationConfig import Lvl1SimulationSequence
    hltBeginSeq += Lvl1SimulationSequence(ConfigFlags)


# ---------------------------------------------------------------
# HLT generation
# ---------------------------------------------------------------
if not opt.createHLTMenuExternally:

    from TriggerMenuMT.HLTMenuConfig.Menu.GenerateMenuMT import GenerateMenuMT
    menu = GenerateMenuMT()

    # define the function that enable the signatures
    def signaturesToGenerate():
        TriggerFlags.Slices_all_setOff()
        for sig in opt.enabledSignatures:
            eval(sig)

    menu.overwriteSignaturesWith(signaturesToGenerate)

    if (opt.selectChains):
        menu.selectChainsForTesting = opt.selectChains

    # generating the HLT structure requires
    # the L1Decoder to be defined in the topSequence
    menu.generateMT()

    if opt.endJobAfterGenerate:
        import sys
        sys.exit(0)



#Needed to get full output from TrigSignatureMoniMT with a large menu: see ATR-21487
#Can be removed once chainDump.py is used instead of log file parsing
svcMgr.MessageSvc.infoLimit=10000



from TrigConfigSvc.TrigConfigSvcCfg import getHLTConfigSvc
svcMgr += conf2toConfigurable( getHLTConfigSvc(ConfigFlags) )

if not opt.createHLTMenuExternally:
    # the generation of the prescale set file from the menu (with all prescales set to 1)
    # is not really needed. If no file is provided all chains are either enabled or disabled,
    # depending on the property L1Decoder.PrescalingTool.KeepUnknownChains being True or False
    from TrigConfigSvc.TrigConfigSvcCfg import createHLTPrescalesFileFromMenu
    createHLTPrescalesFileFromMenu(ConfigFlags)



# ---------------------------------------------------------------
# Monitoring
# ---------------------------------------------------------------
if not hasattr(svcMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr += THistSvc()
if hasattr(svcMgr.THistSvc, "Output"):
    from TriggerJobOpts.HLTTriggerGetter import setTHistSvcOutput
    setTHistSvcOutput(svcMgr.THistSvc.Output)

#-------------------------------------------------------------
# Conditions overrides
#-------------------------------------------------------------
if len(opt.condOverride)>0:
    for folder,tag in opt.condOverride.iteritems():
        log.warning('Overriding folder %s with tag %s', folder, tag)
        conddb.addOverride(folder,tag)

if svcMgr.MessageSvc.OutputLevel < Constants.INFO:
    from AthenaCommon.JobProperties import jobproperties
    jobproperties.print_JobProperties('tree&value')
    print(svcMgr)

#-------------------------------------------------------------
# Output flags
#-------------------------------------------------------------
if opt.doWriteRDOTrigger:
    if ConfigFlags.Trigger.Online.isPartition:
        log.error('Cannot use doWriteRDOTrigger in athenaHLT or partition')
        theApp.exit(1)
    rec.doWriteRDO = False  # RecExCommon flag
    ConfigFlags.Output.doWriteRDO = True  # new JO flag
    if not ConfigFlags.Output.RDOFileName:
        ConfigFlags.Output.RDOFileName = 'RDO_TRIG.pool.root'  # new JO flag
if opt.doWriteBS:
    rec.doWriteBS = True  # RecExCommon flag
    TriggerFlags.writeBS = True  # RecExCommon flag
    ConfigFlags.Output.doWriteBS = True  # new JO flag
    ConfigFlags.Trigger.writeBS = True  # new JO flag

#-------------------------------------------------------------
# ID Cache Creators
#-------------------------------------------------------------
ConfigFlags.lock()
from TriggerJobOpts.TriggerConfig import triggerIDCCacheCreatorsCfg
CAtoGlobalWrapper(triggerIDCCacheCreatorsCfg, ConfigFlags, seqName="HLTBeginSeq")


# B-jet output
if opt.doBjetSlice or opt.forceEnableAllChains:
    from JetTagCalibration.JetTagCalibConfig import JetTagCalibCfg
    alias = ["HLT_b->HLT_b,AntiKt4EMTopo"] #"HLT_bJets" is the name of the b-jet JetContainer
    topSequence += JetTagCalibCfg(ConfigFlags, scheme="Trig", TaggerList=ConfigFlags.BTagging.TrigTaggersList, NewChannel = alias)

#-------------------------------------------------------------
# Output configuration
#-------------------------------------------------------------
if opt.doWriteBS or opt.doWriteRDOTrigger:
    from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects, collectHypoDecisionObjects, triggerOutputCfg
    from AthenaCommon.CFElements import findAlgorithm,findSubSequence
    hypos = collectHypos(findSubSequence(topSequence, "HLTAllSteps"))
    filters = collectFilters(findSubSequence(topSequence, "HLTAllSteps"))

    summaryMakerAlg = findAlgorithm(topSequence, "DecisionSummaryMakerAlg")
    l1decoder = findAlgorithm(topSequence, "L1Decoder")

    if l1decoder and summaryMakerAlg:
        decObj = collectDecisionObjects( hypos, filters, l1decoder, summaryMakerAlg )
        decObjHypoOut = collectHypoDecisionObjects(hypos, inputs=False, outputs=True)
        log.debug("Decision Objects to write to output [hack method - should be replaced with triggerRunCfg()]")
        log.debug(decObj)
    else:
        log.error("Failed to find L1Decoder or DecisionSummaryMakerAlg, cannot determine Decision names for output configuration")
        decObj = []
        decObjHypoOut = []
    CAtoGlobalWrapper( triggerOutputCfg, ConfigFlags, decObj=decObj, decObjHypoOut=decObjHypoOut, summaryAlg=summaryMakerAlg)

#-------------------------------------------------------------
# Non-ComponentAccumulator Cost Monitoring
#-------------------------------------------------------------
include("TrigCostMonitorMT/TrigCostMonitorMT_jobOptions.py")

#-------------------------------------------------------------
# Debugging for view cross-dependencies
#-------------------------------------------------------------
if opt.reverseViews or opt.filterViews:
    from TriggerJobOpts.TriggerConfig import collectViewMakers
    viewMakers = collectViewMakers( topSequence )
    theFilter = []
    if opt.filterViews:
        theFilter = [ "Cache", "EventInfo", "HLT_IDVertex_FS" ]
    for alg in viewMakers:
        alg.ReverseViewsDebug = opt.reverseViews
        alg.FallThroughFilter = theFilter

#-------------------------------------------------------------
# Disable overly verbose and problematic ChronoStatSvc print-out
#-------------------------------------------------------------
include("TriggerTest/disableChronoStatSvcPrintout.py")

#-------------------------------------------------------------
# Disable spurious warnings from HepMcParticleLink, ATR-21838
#-------------------------------------------------------------
if ConfigFlags.Input.isMC:
    svcMgr.MessageSvc.setError += ['HepMcParticleLink']

#-------------------------------------------------------------
# Enable xAOD::EventInfo decorations for pileup values
#-------------------------------------------------------------
include ("LumiBlockComps/LumiBlockMuWriter_jobOptions.py")

#-------------------------------------------------------------
# Apply modifiers
#-------------------------------------------------------------
for mod in modifierList:
    mod.postSetup()

#-------------------------------------------------------------
# Print top sequence
#-------------------------------------------------------------
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)
