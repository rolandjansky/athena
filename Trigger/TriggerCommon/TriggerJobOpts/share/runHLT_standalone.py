
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
    setMenu          = None           # option to overwrite flags.Trigger.triggerMenuSetup
    setDetDescr      = None           # force geometry tag
    setGlobalTag     = None           # force global conditions tag
    useCONDBR2       = True           # if False, use run-1 conditions DB
    condOverride     = {}             # overwrite conditions folder tags e.g. '{"Folder1":"Tag1", "Folder2":"Tag2"}'
    doHLT            = True           # run HLT?
    doID             = True           # ConfigFlags.Trigger.doID
    doCalo           = True           # ConfigFlags.Trigger.doCalo
    doMuon           = True           # ConfigFlags.Trigger.doMuon
    doWriteRDOTrigger = False         # Write out RDOTrigger?
    doWriteBS        = True           # Write out BS?
    doL1Unpacking    = True           # decode L1 data in input file if True, else setup emulation
    doL1Sim          = False          # (re)run L1 simulation
    isOnline         = True           # isOnline flag
    doEmptyMenu      = False          # Disable all chains, except those re-enabled by specific slices
    createHLTMenuExternally = False   # Set to True if the menu is build manually outside runHLT_standalone.py
    endJobAfterGenerate = False       # Finish job after menu generation
    strictDependencies = False        # Sets SGInputLoader.FailIfNoProxy=True and AlgScheduler.DataLoaderAlg=""
    forceEnableAllChains = False      # if True, all HLT chains will run even if the L1 item is false
    enableL1MuonPhase1   = False          # Enable Run-3 LVL1 muon simulation and/or decoding
    enableL1CaloPhase1   = False          # Enable Run-3 LVL1 calo simulation and/or decoding
    enableL1CaloLegacy = True         # Enable Run-2 L1Calo simulation and/or decoding (possible even if enablePhase1 is True)
    enableL1TopoDump = False          # Enable L1Topo simulation to write inputs to txt
    enableL1NSWEmulation = False      # Enable TGC-NSW coincidence emulator : ConfigFlags.Trigger.L1MuonSim.EmulateNSW
    enableL1NSWVetoMode = False       # Enable TGC-NSW coincidence veto mode: ConfigFlags.Trigger.L1MuonSim.NSWVetoMode
    enableL1NSWMMTrigger = False      # Enable MM trigger for TGC-NSW coincidence : ConfigFlags.Trigger.L1MuonSim.doMMTrigger
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
    doUnconventionalTrackingSlice   = True
    reverseViews      = False
    filterViews       = False
    enabledSignatures = []
    disabledSignatures = []
    selectChains      = []
    disableChains     = []

################################################################################
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.AccumulatorCache import AccumulatorDecorator
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

# This is temporary and will be re-worked for after M3.5
for s in slices:
    signature = s[2:].replace('Slice', '')

    if eval('opt.'+s) is True:
        opt.enabledSignatures.append( signature )
    else:
        opt.disabledSignatures.append( signature )

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
        if globalflags.DataSource=='data':
            opt.setGlobalTag = ConfigFlags.Trigger.OnlineCondTag if opt.isOnline else 'CONDBR2-BLKPA-2018-13'
        else:
            opt.setGlobalTag = 'OFLCOND-MC16-SDR-RUN2-08-02'
    TriggerJobOpts.Modifiers._run_number = ConfigFlags.Input.RunNumber[0]
    TriggerJobOpts.Modifiers._lb_number = ConfigFlags.Input.LumiBlockNumber[0]

else:   # athenaHLT
    globalflags.InputFormat = 'bytestream'
    globalflags.DataSource = 'data' if not opt.setupForMC else 'data'
    ConfigFlags.Input.isMC = False
    ConfigFlags.Input.Files = []
    TriggerJobOpts.Modifiers._run_number = globals().get('_run_number')  # set by athenaHLT
    TriggerJobOpts.Modifiers._lb_number = globals().get('_lb_number')  # set by athenaHLT
    if '_run_number' in globals():
        del _run_number  # noqa, set by athenaHLT
    if '_lb_number' in globals():
        del _lb_number  # noqa, set by athenaHLT

from AthenaConfiguration.Enums import Format
ConfigFlags.Input.Format = Format.BS if globalflags.InputFormat == 'bytestream' else Format.POOL

# Load input collection list from POOL metadata
from RecExConfig.ObjKeyStore import objKeyStore
if ConfigFlags.Input.Format is Format.POOL:
    from PyUtils.MetaReaderPeeker import convert_itemList
    objKeyStore.addManyTypesInputFile(convert_itemList(layout='#join'))

# Run-3 Trigger produces Run-3 EDM
ConfigFlags.Trigger.EDMVersion = 3

# Set final Cond/Geo tag based on input file, command line or default
globalflags.DetDescrVersion = opt.setDetDescr or ConfigFlags.Trigger.OnlineGeoTag
ConfigFlags.GeoModel.AtlasVersion = globalflags.DetDescrVersion()
globalflags.ConditionsTag = opt.setGlobalTag or ConfigFlags.Trigger.OnlineCondTag
ConfigFlags.IOVDb.GlobalTag = globalflags.ConditionsTag()

# Other defaults
ConfigFlags.Beam.Type = jobproperties.Beam.beamType = 'collisions'
jobproperties.Beam.bunchSpacing = 25
if not ConfigFlags.Input.isMC:
    globalflags.DatabaseInstance='CONDBR2' if opt.useCONDBR2 else 'COMP200'
    ConfigFlags.IOVDb.DatabaseInstance=globalflags.DatabaseInstance()
athenaCommonFlags.isOnline.set_Value_and_Lock(opt.isOnline)
ConfigFlags.Common.isOnline = athenaCommonFlags.isOnline()

log.info('Configured the following global flags:')
globalflags.print_JobProperties()

# Enable strict dependency checking for data by default
if 'strictDependencies' not in globals():
    opt.strictDependencies = not ConfigFlags.Input.isMC

# Set default doL1Sim option depending on input type (if not set explicitly)
if 'doL1Sim' not in globals():
    opt.doL1Sim = ConfigFlags.Input.isMC
    log.info('Setting default doL1Sim=%s because ConfigFlags.Input.isMC=%s', opt.doL1Sim, ConfigFlags.Input.isMC)

# Set default enableL1CaloPhase1 option to True if running L1Sim on data or MC with SuperCells (ATR-23703)
if 'enableL1CaloPhase1' not in globals():
    if ConfigFlags.Input.Format is Format.BS:
        opt.enableL1CaloPhase1 = opt.doL1Sim
        log.info('Setting default enableL1CaloPhase1=%s because ConfigFlags.Input.Format=%s and doL1Sim=%s',
                 opt.enableL1CaloPhase1, ConfigFlags.Input.Format, opt.doL1Sim)
    else:
        scell_available = objKeyStore.isInInput('CaloCellContainer','SCell')
        opt.enableL1CaloPhase1 = scell_available
        log.info('Setting default enableL1CaloPhase1=%s because ConfigFlags.Input.Format=%s and SuperCells '
                 'are%s available in the input file',
                 opt.enableL1CaloPhase1, ConfigFlags.Input.Format, ('' if scell_available else ' not'))

# Set default enableL1MuonPhase1 option to True if running L1Sim (ATR-23973)
if 'enableL1MuonPhase1' not in globals():
    opt.enableL1MuonPhase1 = opt.doL1Sim
    log.info('Setting default enableL1MuonPhase1=%s because doL1Sim=%s', opt.enableL1MuonPhase1, opt.doL1Sim)

if ConfigFlags.Input.Format is Format.BS or opt.doL1Sim:
    ConfigFlags.Trigger.HLTSeeding.forceEnableAllChains = opt.forceEnableAllChains

# Translate a few other flags
ConfigFlags.Trigger.doLVL1 = opt.doL1Sim
ConfigFlags.Trigger.enableL1MuonPhase1 = opt.enableL1MuonPhase1
ConfigFlags.Trigger.enableL1CaloPhase1 = opt.enableL1CaloPhase1
ConfigFlags.Trigger.enableL1CaloLegacy = opt.enableL1CaloLegacy
ConfigFlags.Trigger.enableL1TopoDump = opt.enableL1TopoDump

ConfigFlags.Trigger.L1MuonSim.EmulateNSW  = opt.enableL1NSWEmulation
ConfigFlags.Trigger.L1MuonSim.NSWVetoMode = opt.enableL1NSWVetoMode
ConfigFlags.Trigger.L1MuonSim.doMMTrigger = opt.enableL1NSWMMTrigger

ConfigFlags.Trigger.doHLT = bool(opt.doHLT)
ConfigFlags.Trigger.doID = opt.doID
ConfigFlags.Trigger.doMuon = opt.doMuon
ConfigFlags.Trigger.doCalo = opt.doCalo

if opt.setMenu:
    ConfigFlags.Trigger.triggerMenuSetup = opt.setMenu

# Setup list of modifiers
# Common modifiers for MC and data
setModifiers = ['noLArCalibFolders',
                'ForceMuonDataType',
                'useNewRPCCabling',
                'enableCostMonitoring',
                'useOracle',
]

if ConfigFlags.Input.isMC:  # MC modifiers
    setModifiers += ['BFieldFromDCS']
else:           # More data modifiers
    setModifiers += ['BFieldAutoConfig',
                     'useDynamicAlignFolders',
                     'useHLTMuonAlign',
                     #Check for beamspot quality flag
                     'useOnlineLumi',
                     #for running with real data
                     'DisableMdtT0Fit',
                     #Set muComb/muIso Backextrapolator tuned for real data
                     #Monitoring for L1 muon group
                     #Monitoring L1Topo at ROB level
                     'forceTileRODMap',
                     'enableSchedulerMon'
    ]

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
    log.error('Unknown modifier(s): %s', setModifiers)

#-------------------------------------------------------------
# Output flags
#-------------------------------------------------------------
from RecExConfig.RecFlags import rec
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
    ConfigFlags.Output.doWriteBS = True  # new JO flag
    ConfigFlags.Trigger.writeBS = True  # new JO flag

# never include this
include.block("RecExCond/RecExCommon_flags.py")

# ---------------------------------------------------------------
# Create main sequences
# ---------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.CFElements import seqOR,parOR
hltTop = seqOR("HLTTop")
hltBeginSeq = parOR("HLTBeginSeq")
hltTop += hltBeginSeq
topSequence += hltTop

#-------------------------------------------------------------
# Setting DetFlags
#-------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags
if ConfigFlags.Input.Format is not Format.BS:
    DetFlags.detdescr.all_setOn()
    #if not ConfigFlags.Input.isMC or ConfigFlags.Common.isOnline:
    #    DetFlags.detdescr.ALFA_setOff()

if ConfigFlags.Trigger.doID:
    DetFlags.detdescr.ID_setOn()
    DetFlags.makeRIO.ID_setOn()
else:
    DetFlags.ID_setOff()

if ConfigFlags.Trigger.doMuon:
    DetFlags.detdescr.Muon_setOn()
    DetFlags.makeRIO.all_setOn()
    # Setup muon reconstruction for trigger
    ConfigFlags.Muon.MuonTrigger=True
else:
    DetFlags.Muon_setOff()

if ConfigFlags.Trigger.doCalo:
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

#-------------------------------------------------------------
# Apply modifiers
#-------------------------------------------------------------
for mod in modifierList:
    mod.preSetup()

#--------------------------------------------------------------
# Increase scheduler checks and verbosity
#--------------------------------------------------------------
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.CheckDependencies( True )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )
AlgScheduler.EnableVerboseViews( True )

#--------------------------------------------------------------
# Enable strict enforcing of data dependencies
#--------------------------------------------------------------
if opt.strictDependencies:
    AlgScheduler.setDataLoaderAlg("")
    if not hasattr(topSequence,"SGInputLoader"):
        log.error('Cannot set FailIfNoProxy property because SGInputLoader not found in topSequence')
        theApp.exit(1)
    topSequence.SGInputLoader.FailIfNoProxy = True

# ----------------------------------------------------------------
# Detector geometry
# ----------------------------------------------------------------
# Always enable magnetic field
from AthenaCommon.DetFlags import DetFlags
DetFlags.BField_setOn()
include ("RecExCond/AllDet_detDescr.py")

if ConfigFlags.Trigger.doID:
    include("InDetTrigRecExample/InDetTrigRec_jobOptions.py")
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    InDetTrigFlags.doPrintConfigurables = log.getEffectiveLevel() <= logging.DEBUG
    from InDetRecExample.InDetJobProperties import InDetFlags
    InDetFlags.doPrintConfigurables = log.getEffectiveLevel() <= logging.DEBUG
    include("InDetRecExample/InDetRecConditionsAccess.py")

#-------------------------------------------------------------
# Lock flags !
#
# This is the earliest we can lock since InDetJobProperties.py
# above still modifies the ConfigFlags.
ConfigFlags.lock()

# Only import this here to avoid we accidentally use CAs before locking
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
#-------------------------------------------------------------

from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg
CAtoGlobalWrapper(IOVDbSvcCfg, ConfigFlags)

if ConfigFlags.Trigger.doCalo:
    from TrigT2CaloCommon.TrigCaloDataAccessConfig import trigCaloDataAccessSvcCfg
    CAtoGlobalWrapper(trigCaloDataAccessSvcCfg, ConfigFlags)
    if ConfigFlags.Trigger.doTransientByteStream:
        from TriggerJobOpts.TriggerTransBSConfig import triggerTransBSCfg_Calo
        CAtoGlobalWrapper(triggerTransBSCfg_Calo, ConfigFlags, seqName="HLTBeginSeq")


if ConfigFlags.Trigger.doMuon:
    import MuonCnvExample.MuonCablingConfig  # noqa: F401
    import MuonRecExample.MuonReadCalib      # noqa: F401

    include ("MuonRecExample/MuonRecLoadTools.py")

# restore logger after above includes
log = logging.getLogger('runHLT_standalone.py')

# ----------------------------------------------------------------
# Pool input
# ----------------------------------------------------------------
print("ConfigFlags.Input.Format", ConfigFlags.Input.Format)
print("ConfigFlags.Trigger.Online.isPartition", ConfigFlags.Trigger.Online.isPartition)
if ConfigFlags.Input.Format is Format.POOL:
    import AthenaPoolCnvSvc.ReadAthenaPool   # noqa
    svcMgr.AthenaPoolCnvSvc.PoolAttributes = [ "DEFAULT_BUFFERSIZE = '2048'" ]
    svcMgr.PoolSvc.AttemptCatalogPatch=True

# ----------------------------------------------------------------
# ByteStream input
# ----------------------------------------------------------------
elif ConfigFlags.Input.Format is Format.BS and not ConfigFlags.Trigger.Online.isPartition:
    # Set up ByteStream reading services
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    CAtoGlobalWrapper(ByteStreamReadCfg, ConfigFlags)

# ---------------------------------------------------------------
# Trigger config
# ---------------------------------------------------------------
from TrigConfigSvc.TrigConfigSvcCfg import generateL1Menu, createL1PrescalesFileFromMenu
generateL1Menu(ConfigFlags)
createL1PrescalesFileFromMenu(ConfigFlags)

from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
CAtoGlobalWrapper(L1ConfigSvcCfg,ConfigFlags)

# ---------------------------------------------------------------
# Event Info setup
# ---------------------------------------------------------------
# If no xAOD::EventInfo is found in a POOL file, schedule conversion from old EventInfo
if ConfigFlags.Input.Format is Format.POOL:
    if objKeyStore.isInInput("xAOD::EventInfo"):
        topSequence.SGInputLoader.Load += [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]
    else:
        if not hasattr(hltBeginSeq, "xAODMaker::EventInfoCnvAlg"):
            from xAODEventInfoCnv.xAODEventInfoCnvAlgDefault import xAODEventInfoCnvAlgDefault
            xAODEventInfoCnvAlgDefault(sequence=hltBeginSeq)
else:
    topSequence.SGInputLoader.Load += [( 'xAOD::EventInfo' , 'StoreGateSvc+EventInfo' )]

# ---------------------------------------------------------------
# Add LumiBlockMuWriter creating xAOD::EventInfo decorations for pileup values
# ---------------------------------------------------------------
from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
CAtoGlobalWrapper(LumiBlockMuWriterCfg, ConfigFlags, seqName="HLTBeginSeq")


# ---------------------------------------------------------------
# Level 1 simulation
# ---------------------------------------------------------------
if opt.doL1Sim:
    from TriggerJobOpts.Lvl1SimulationConfig import Lvl1SimulationCfg
    CAtoGlobalWrapper(Lvl1SimulationCfg, ConfigFlags, seqName="HLTBeginSeq")


# ---------------------------------------------------------------
# Add HLTSeeding providing inputs to HLT
# ---------------------------------------------------------------
if opt.doL1Unpacking:
    if ConfigFlags.Input.Format is Format.BS or opt.doL1Sim:
        from HLTSeeding.HLTSeedingConfig import HLTSeedingCfg
        CAtoGlobalWrapper(HLTSeedingCfg, ConfigFlags, seqName="HLTBeginSeq")
    else:
        from DecisionHandling.TestUtils import L1EmulationTest
        hltBeginSeq += L1EmulationTest()

# ---------------------------------------------------------------
# HLT generation
# ---------------------------------------------------------------
if not opt.createHLTMenuExternally:

    from TriggerMenuMT.HLT.Menu.GenerateMenuMT import GenerateMenuMT
    menu = GenerateMenuMT()

    def chainsToGenerate(signame, chain):
        return ((signame in opt.enabledSignatures and signame not in opt.disabledSignatures) and
                (not opt.selectChains or chain in opt.selectChains) and chain not in opt.disableChains)

    menu.setChainFilter(chainsToGenerate)

    # generating the HLT structure requires
    # the HLTSeeding to be defined in the topSequence
    menu.generateMT(ConfigFlags)
    # Note this will also create the requested HLTPrescale JSON
    # - the default file (with all prescales set to 1) is not really needed.
    # - If no file is provided all chains are either enabled or disabled,
    #   depending on the property HLTSeeding.PrescalingTool.KeepUnknownChains being True or False


    if opt.endJobAfterGenerate:
        import sys
        sys.exit(0)



from TrigConfigSvc.TrigConfigSvcCfg import HLTConfigSvcCfg
CAtoGlobalWrapper(HLTConfigSvcCfg,ConfigFlags)

# ---------------------------------------------------------------
# Tell the SGInputLoader about L1 and HLT menu in the DetectorStore
# ---------------------------------------------------------------
if hasattr(topSequence,"SGInputLoader"):
    topSequence.SGInputLoader.Load += [
        ('TrigConf::L1BunchGroupSet','DetectorStore+L1BunchGroup'),
        ('TrigConf::L1Menu','DetectorStore+L1TriggerMenu'),
        ('TrigConf::HLTMenu','DetectorStore+HLTTriggerMenu')]

# ---------------------------------------------------------------
# Monitoring
# ---------------------------------------------------------------
if not hasattr(svcMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr += THistSvc()
if hasattr(svcMgr.THistSvc, "Output"):
    from TriggerJobOpts.TriggerHistSvcConfig import setTHistSvcOutput
    setTHistSvcOutput(svcMgr.THistSvc.Output)

#-------------------------------------------------------------
# Conditions overrides
#-------------------------------------------------------------
if len(opt.condOverride)>0:
    for folder,tag in iter(opt.condOverride.items()):
        log.warning('Overriding folder %s with tag %s', folder, tag)
        from IOVDbSvc.IOVDbSvcConfig import addOverride
        CAtoGlobalWrapper(addOverride, ConfigFlags,folder=folder,tag=tag)

if svcMgr.MessageSvc.OutputLevel < Constants.INFO:
    from AthenaCommon.JobProperties import jobproperties
    jobproperties.print_JobProperties('tree&value')
    print(svcMgr)

#-------------------------------------------------------------
# ID Cache Creators
#-------------------------------------------------------------
from TriggerJobOpts.TriggerConfig import triggerIDCCacheCreatorsCfg
CAtoGlobalWrapper(triggerIDCCacheCreatorsCfg, ConfigFlags, seqName="HLTBeginSeq")

#-------------------------------------------------------------
# Output configuration
#-------------------------------------------------------------
if opt.doWriteBS or opt.doWriteRDOTrigger:
    from TriggerJobOpts.TriggerConfig import collectHypos, collectFilters, collectDecisionObjects, collectHypoDecisionObjects, triggerOutputCfg
    from AthenaCommon.CFElements import findAlgorithm,findSubSequence
    hypos = collectHypos(findSubSequence(topSequence, "HLTAllSteps"))
    filters = collectFilters(findSubSequence(topSequence, "HLTAllSteps"))

    nfilters = sum(len(v) for v in filters.values())
    nhypos = sum(len(v) for v in hypos.values())    
    log.info( "Algorithms counting: Number of Filter algorithms: %d  -  Number of Hypo algoirthms: %d", nfilters , nhypos) 

    summaryMakerAlg = findAlgorithm(topSequence, "DecisionSummaryMakerAlg")
    hltSeeding = findAlgorithm(topSequence, "HLTSeeding")

    if hltSeeding and summaryMakerAlg:
        decObj = collectDecisionObjects( hypos, filters, hltSeeding, summaryMakerAlg )
        decObjHypoOut = collectHypoDecisionObjects(hypos, inputs=False, outputs=True)
        log.debug("Decision Objects to write to output [hack method - should be replaced with triggerRunCfg()]")
        log.debug(decObj)
    else:
        log.error("Failed to find HLTSeeding or DecisionSummaryMakerAlg, cannot determine Decision names for output configuration")
        decObj = []
        decObjHypoOut = []

    # Add HLT Navigation to EDM list
    from TrigEDMConfig import TriggerEDMRun3
    TriggerEDMRun3.addHLTNavigationToEDMList(TriggerEDMRun3.TriggerHLTListRun3, decObj, decObjHypoOut)

    # Configure output writing
    CAtoGlobalWrapper(triggerOutputCfg, ConfigFlags, hypos=hypos)

#-------------------------------------------------------------
# Cost Monitoring
#-------------------------------------------------------------

from TrigCostMonitor.TrigCostMonitorConfig import TrigCostMonitorCfg, TrigCostMonitorPostSetup
CAtoGlobalWrapper(TrigCostMonitorCfg, ConfigFlags)
# TODO - how can TrigCostMonitorPostSetup be component-accumulator-ised?
TrigCostMonitorPostSetup()

#-------------------------------------------------------------
# Debugging for view cross-dependencies
#-------------------------------------------------------------
if opt.reverseViews or opt.filterViews:
    from TriggerJobOpts.TriggerConfig import collectViewMakers
    viewMakers = collectViewMakers( topSequence )
    theFilter = []
    if opt.filterViews:
        # no idea why the FS vertex would be needed here, but I'll add the FSJet vertex also for good measure
        theFilter = [ "Cache", "EventInfo", "HLT_IDVertex_FS", "HLT_IDVertex_FSJet" ]
    for alg in viewMakers:
        alg.ReverseViewsDebug = opt.reverseViews
        alg.FallThroughFilter = theFilter

#-------------------------------------------------------------
# Disable overly verbose and problematic ChronoStatSvc print-out
#-------------------------------------------------------------
include("TriggerTest/disableChronoStatSvcPrintout.py")

#-------------------------------------------------------------
# MessageSvc
#-------------------------------------------------------------
svcMgr.MessageSvc.Format = "% F%40W%C%4W%R%e%s%8W%R%T %0W%M"
svcMgr.MessageSvc.enableSuppression = False

if ConfigFlags.Input.isMC:
    # Disable spurious warnings from HepMcParticleLink, ATR-21838
    svcMgr.MessageSvc.setError += ['HepMcParticleLink']

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

#-------------------------------------------------------------
# Print caching statistics
#-------------------------------------------------------------
AccumulatorDecorator.printStats()
