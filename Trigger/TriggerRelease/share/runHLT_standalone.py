##############################################################
# TriggerRelease/runHLT_standalone.py
#
#   jobOption file to run HLT standalone (independant of RecExCommon)
#
# Input could be Pool/RDO or BS
#
# Input file: (athena only)
#   Pool: set by "PoolRDOInput"
#   BS:   set by "BSRDOInput"
#
# L1Topo/L1 configuration are fixed
#   set by "setLVL1XML" 
#
# HLT configuration uses Python configuration
#   set by "setMenu" (can also include prescales)
#   if used for MC, one needs to set "setupForMC" to True
#   Individual slices can be run by doing "test<sliceName>=True"
#   Individual signatures can be removed/added putting them in
#     "removeSignatures" and "addSignatures"
#     e.g. "addSignatures={'MuonSlice': ['mu6_ef']}"
#
# HLT configuration from existing XML file
#   set by "setHLTXML"
#
# Geometry and conditions
#   CommisDB         set by "useCOMCONDDB"
#   Geometry         set by "setDetDescr"
#   Conditions       set by "setGlobalTag"
#   Folder overrides set by condOverride='{"Folder1":"Tag1", "Folder2":"Tag2"}'
#
# Special modifiers
#   Currently certain options needs to be modified depending on the type of data
#   Good defaults are provided for running on physics MC and cosmics Data
#   Change by specifying list in "setModifiers" or turn modifiers on/off
#    by doing "myModifier=True/False"
#   Existing modifiers can be found in "TriggerRelease/python/Modifiers.py"
#
# Predefined setups
#   certain menu configurations can be run just by doing
#     testPhysicsV6=True or
#     testMCV6=True or
#     testPhysicsV6MC=True
#   Note that for the addition of "MC" option. This is used for getting the MC setup.
#
# Usage:
#  
#   run with athenaHLT:
#      athenaHLT -f input.data -c testPhysicsV6=True -l DEBUG -n 25 TriggerRelease/runHLT_standalone.py
#      or with multiple files
#      athenaHLT -f "['input1.data','input2.data']" -c testPhysicsV6=True -n 25 TriggerRelease/runHLT_standalone.py
#      or, for writing BS output:
#      athenaHLT -f input.data -c testPhysicsV6=True -n 25 -o outBS TriggerRelease/runHLT_standalone.py
#      or, with online THistSvc
#      athenaHLT -M -l DEBUG -c testPhysicsV6=True -n 25 TriggerRelease/runHLT_standalone.py
#
#   run with athena:
#      BS input: athena.py -c "testPhysicsV6=True;BSRDOInput=['raw.data']" TriggerRelease/runHLT_standalone.py
#      RDO input: athena.py -c "testPhysicsV6=True;PoolRDOInput=['file.pool.root']" TriggerRelease/runHLT_standalone.py
#
# Select slice(s) to test:
#   set one or more of the following flags to True in the jo:
#      testEgamma, testMuon, testTau, testJet, testBjet, testMET, testBphysics
#    e.g.:
#      athenaHLT -f input.data -l DEBUG -n 25 -c "testTau=True;testPhysicsV6=True" TriggerRelease/runHLT_standalone.py
#
#===========================================================================================
from AthenaCommon.Logging import logging
log = logging.getLogger('runHLT_standalone.py')

#predefined menu setups accessible using 'test<NAME>[MC]=True' commandline
menuMap={
         #2017 menus:       menu name                   L1 xml file
         'PhysicsV7':       ('Physics_pp_v7',           'TriggerMenuXML/LVL1config_Physics_pp_v7.xml'),
         'MCV7':            ('MC_pp_v7',                'TriggerMenuXML/LVL1config_MC_pp_v7.xml'),

         #2016 menus:       menu name                   L1 xml file
         'PhysicsV6':       ('Physics_pp_v6',           'TriggerMenuXML/LVL1config_Physics_pp_v6.xml'),
         'MCV6':            ('MC_pp_v6',                'TriggerMenuXML/LVL1config_MC_pp_v6.xml'),
         'HIV4':            ('Physics_HI_v4',           'TriggerMenuXML/LVL1config_Physics_HI_v4.xml'),
         'MCHIV4':          ('MC_HI_v4',                'TriggerMenuXML/LVL1config_Physics_HI_v4.xml'),

         #2015 menus:       menu name                   L1 xml file
         'PhysicsV5':       ('Physics_pp_v5',           'TriggerMenuXML/LVL1config_Physics_pp_v5.xml'),
         'MCV5':            ('MC_pp_v5',                'TriggerMenuXML/LVL1config_MC_pp_v5.xml'),
         'LS1V1':           ('LS1_v1',                  'TriggerMenuXML/LVL1config_Physics_pp_v4.xml'),
         'DC14':            ('DC14',                    ''),
         'HIV3':            ('Physics_HI_v3',           'TriggerMenuXML/LVL1config_Physics_HI_v3.xml'),
         'MCHIV3':          ('MC_HI_v3',                'TriggerMenuXML/LVL1config_Physics_HI_v3.xml'),

         #2012 menus:       menu name                   L1 xml file
         'PhysicsV4':       ('Physics_pp_v4',           'TriggerMenuXML/LVL1config_Physics_pp_v4.xml'),
         'MCV4':            ('MC_pp_v4',                'TriggerMenuXML/LVL1config_Physics_pp_v4.xml'),
         'L1V4':            ('L1_pp_v4',                'TriggerMenuXML/LVL1config_L1_pp_v4.xml'),
         'ALFAV2':          ('L1_alfa_v2',              'TriggerMenuXML/LVL1config_L1_alfa_v2.xml'),
         'HIV2':            ('Physics_HI_v2',           'TriggerMenuXML/LVL1config_Physics_HI_v2.xml'),
         'MCHIV2':          ('MC_HI_v2',                'TriggerMenuXML/LVL1config_Physics_HI_v2.xml'),

         #2011 menus:          menu name                      L1 xml file
         'CosmicPPV2':      ('Physics_pp_v2_cosmics_prescale',           'TriggerMenuXML/LVL1config_Physics_pp_v2.xml'),
         'PhysicsV2':       ('Physics_pp_v2',           'TriggerMenuXML/LVL1config_Physics_pp_v2.xml'),
         'MCV2':            ('MC_pp_v2',                'TriggerMenuXML/LVL1config_MC_pp_v2.xml'),
         'L1V2':            ('L1_pp_v2',                'TriggerMenuXML/LVL1config_L1_pp_v2.xml'),
         'HIV1':            ('Physics_HI_v1',           'TriggerMenuXML/LVL1config_Physics_HI_v1.xml'),
         'MCHIV1':          ('MC_HI_v1',                'TriggerMenuXML/LVL1config_Physics_HI_v1.xml'),
         'PhysicsV3':       ('Physics_pp_v3',           'TriggerMenuXML/LVL1config_Physics_pp_v3.xml'),
         'MCV3':            ('MC_pp_v3',                'TriggerMenuXML/LVL1config_Physics_pp_v3.xml'),
         'L1V3':            ('L1_pp_v3',                'TriggerMenuXML/LVL1config_L1_pp_v3.xml'),
         'ALFAV1':          ('L1_alfa_v1',              'TriggerMenuXML/LVL1config_L1_alfa_v1.xml'),
         
         #2010 menus:          menu name                      L1 xml file
         'InitialBeamV2':   ('InitialBeam_v2',          'TriggerMenuXML/LVL1config_InitialBeam_v2.xml'),
         'MCInitialBeamV2': ('MC_InitialBeam_v2',       'TriggerMenuXML/LVL1config_InitialBeam_v2.xml'),
         'CosmicV2':        ('Cosmic_v2',               'TriggerMenuXML/LVL1config_InitialBeam_v2.xml'),

         'InitialBeamV3':   ('InitialBeam_v3',          'TriggerMenuXML/LVL1config_InitialBeam_v3.xml'),
         'MCInitialBeamV3': ('MC_InitialBeam_v3',       'TriggerMenuXML/LVL1config_InitialBeam_v3.xml'),
         'CosmicV3':        ('Cosmic_v3',               'TriggerMenuXML/LVL1config_InitialBeam_v3.xml'),

         'PhysicsV1':       ('Physics_pp_v1',           'TriggerMenuXML/LVL1config_Physics_pp_v1.xml'),
         'MCV1':            ('MC_pp_v1',                'TriggerMenuXML/LVL1config_Physics_pp_v1.xml'),
         'CosmicPPV1':      ('Physics_pp_v1_cosmics_prescale','TriggerMenuXML/LVL1config_Physics_pp_v1.xml'),

         #Upgrade menus:       menu name                   L1 xml file
         'MCPhaseII':       ('MC_PhaseII',           'TriggerMenuXML/LVL1config_MC_PhaseII.xml'),
         }

# Useful in job options beyond our control to always run the latest menu via 'testCurrentMenu=True'
menuMap['CurrentMenu'] = menuMap['PhysicsV7']

newMenuSetup=0
for name in menuMap:
    for key in ['','MC']:
        if ('test'+name+key in dir()):
            newMenuSetup+=1
            setMenu=menuMap[name][0]
            setLVL1XML=menuMap[name][1]
            if len(menuMap[name])<=2:
                # topo menu xml derived from l1 menu name
                setL1TopoXML=menuMap[name][1].replace("/LVL1","/L1Topo")
            else:
                # topo menu xml explicitly given
                setL1TopoXML=menuMap[name][2]
                
            if 'setupForMC' not in dir():
                setupForMC=(key=='MC')
                print 'Setting setupForMC = ',setupForMC

if 'setupForMC' not in dir():
    setupForMC=False

if newMenuSetup>1:
    log.fatal('More than one menu requested')
    import sys
    sys.exit(1)
if newMenuSetup==0 and not ('setMenu' in dir()):
    log.fatal('No trigger menu specified, use e.g. testPhysicsV6=True')
    import sys
    sys.exit(1)

#setup options for standalone configurations

defaultOptions={ 
    'setMenu'          : setMenu,
    'setupForMC'       : setupForMC,
    'setLVL1XML'       : 'TriggerMenuXML',
    'setL1TopoXML'     : 'TriggerMenuXML',
    'setHLTXML'        : None,
    'removeSignatures' : {},
    'addSignatures'    : {},
    'setDetDescr'      : None,
    'setGlobalTag'     : None,
    'useCONDBR2'       : True,
    'condOverride'     : {},
    'emptyMenu'        : False,
    'PoolRDOInput'     : None,
    'BSRDOInput'       : None,
    'OutputLevel'      : INFO,
    'HLTOutputLevel'   : INFO,
    'EvtMax'           : -1,
    'SkipEvents'       :  0,
}


#-------------------------------------------------------------
# Transfer flags into TriggerFlags
#-------------------------------------------------------------
from TriggerJobOpts.TriggerFlags import TriggerFlags

# To turn off HLT for athena running
TriggerFlags.doHLT = True
if 'doHLT' in dir():
    TriggerFlags.doHLT = bool(doHLT)
    del doHLT
    
# To extract the Trigger configuration
if "doDBConfig" in dir():
    TriggerFlags.Online.doDBConfig = bool(doDBConfig)
    del doDBConfig
if "trigBase" in dir():
    TriggerFlags.Online.doDBConfigBaseName = trigBase
    del trigBase


#set geometry and conditions
if setupForMC:   # for MC
    defaultOptions['useCOMCONDDB']=False
else:            # for data
    defaultOptions['useCOMCONDDB']=True
    defaultOptions['setDetDescr']=TriggerFlags.OnlineGeoTag()
    defaultOptions['setGlobalTag']=TriggerFlags.OnlineCondTag()

# Setup list of modifiers
defaultOptions['setModifiers']=[#Common modifiers for MC and data
                                'noLArCalibFolders',
                                'ForceMuonDataType',
                                'useNewRPCCabling',
                                'enableCostMonitoring', 
                                'enableCoherentPS',
                                'useOracle',
                                'detailedErrorStreams',
                                'optimizeChainOrder',    
                                'enableHotIDMasking',
                                'openThresholdRPCCabling',
                                #special streaming setup
                                'enable7BitL1TTStreaming',
]

if setupForMC:  # MC modifiers
    defaultOptions['setModifiers']+=['BFieldFromDCS']
else:           # More data modifiers
    defaultOptions['setModifiers']+=[
                                'allowCOOLUpdates',
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
                                'muCTPicheck',
                                #Monitoring L1Topo at ROB level
                                'L1TopoCheck',
                                'forceTileRODMap',
                                #for tests with data:
                                #'ignoreErrorStream', #nothing goes to debug stream
                                #'inclusiveErrorStream', #errors go to both debug and physics streams
                                #'ignoreL1Vetos',  #also run L2 prescaled and disabled L1 items
                                #'disablePixels',
                                #'disableSCTBarrel',
                                'useDynamicAlignFolders',
    ]

#make some more common trig cost operations easier to setup
if 'enableCostD3PD' in dir() or 'enableRateD3PD' in dir():
    enableCostMonitoring = True # This goes without saying!
    enableCostForCAF = True # This sets the cost mon to monitor every event and always export the data.

#-------------------------------------------------------------
# Setup options
#-------------------------------------------------------------
log.info('Setup options:')
for option in defaultOptions:
    if option in dir():
        print ' %20s = %s' % (option,str(globals()[option]))
    else:
        globals()[option]=defaultOptions[option]
        print ' %20s = (Default) %s' % (option,str(globals()[option]))

#we don't want anything further on to depend on these flags
del newMenuSetup
del setupForMC

#-------------------------------------------------------------
# Modifiers
#-------------------------------------------------------------

modifierList=[]
import TriggerRelease.Modifiers
if '_run_number' in dir(): TriggerRelease.Modifiers._run_number = _run_number

try:  # Temporary backwards compatible hack
    from TrigConfigSvc.TrigConfMetaData import TrigConfMetaData
    meta = TrigConfMetaData()
except:
    log.warning("TrigConfigSvc.TrigConfMetaData not available in this release")
    meta = None
    
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
        if meta: meta.Modifiers += [mod]    # store in trig conf meta data
        setModifiers.remove(mod)

if setModifiers:
    log.error('Unknown modifier(s): '+str(setModifiers))


#-------------------------------------------------------------
# Setting TriggerFlags
#-------------------------------------------------------------
include.block("RecExCond/RecExCommon_flags.py")
log = logging.getLogger('runHLT_standalone.py')

TriggerFlags.doHLTpersistency=False
TriggerFlags.writeBS=True
TriggerFlags.abortOnConfigurationError=True

TriggerFlags.triggerMenuSetup=setMenu

def stripPrescales(menu):
    import re
    m = re.match('(.*v\d).*', menu)
    return m.groups()[0] if m else menu

# L1 Topo
if setL1TopoXML=='TriggerMenuXML':    
    setL1TopoXML='TriggerMenuXML/L1Topoconfig_'+stripPrescales(setMenu)+'.xml'
TriggerFlags.inputL1TopoConfigFile   = setL1TopoXML
TriggerFlags.readL1TopoConfigFromXML = True
TriggerFlags.outputL1TopoConfigFile  = None

# LVL1 config
if setLVL1XML=='TriggerMenuXML':
    setLVL1XML='TriggerMenuXML/LVL1config_'+stripPrescales(setMenu)+'.xml'
TriggerFlags.inputLVL1configFile=setLVL1XML
TriggerFlags.readLVL1configFromXML=True
TriggerFlags.outputLVL1configFile=None

# HLT config
TriggerFlags.enableMonitoring = ['Online']
TriggerFlags.readHLTconfigFromXML=False
if setHLTXML!=None:
    TriggerFlags.readHLTconfigFromXML=True
    TriggerFlags.inputHLTconfigFile = setHLTXML

log.info("Trigger xml files L1Topo : in = %s , out = %s (read from XML = %s)" % (TriggerFlags.inputL1TopoConfigFile(), TriggerFlags.outputL1TopoConfigFile(), TriggerFlags.readL1TopoConfigFromXML() ) )
log.info("Trigger xml files LVL1   : in = %s , out = %s (read from XML = %s)" % (TriggerFlags.inputLVL1configFile(),   TriggerFlags.outputLVL1configFile(), TriggerFlags.readLVL1configFromXML() ) )
log.info("Trigger xml files HLT    : in = %s , out = %s (read from XML = %s)" % (TriggerFlags.inputHLTconfigFile(),    TriggerFlags.outputHLTconfigFile(), TriggerFlags.readHLTconfigFromXML() ) )



# define a unique HLT configuration xml when running in a partition
import os
if "TDAQ_PARTITION" in os.environ:
    from TriggerRelease import Utils
    ident = Utils.uniqueFileName()
    TriggerFlags.outputHLTconfigFile ="outputHLTconfig_%s.xml" % ident
    TriggerFlags.inputHLTconfigFile  ="outputHLTconfig_%s.xml" % ident

### detector flags - we always run with everything on now
TriggerFlags.doID=True
TriggerFlags.doMuon=True
TriggerFlags.doCalo=True

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.isOnline = True

#TriggerFlags.CosmicSlice.testCosmic=False #Makes cosmic slice more quiet by default

#-------------------------------------------------------------
# Setting Global Flags
#-------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.InputFormat='bytestream'  # default for athenaMT/PT

# Input format and file for athena running
if BSRDOInput!=None:
    globalflags.InputFormat='bytestream'
    if type(BSRDOInput)==type(''):
        athenaCommonFlags.BSRDOInput=[BSRDOInput]
    else:
        athenaCommonFlags.BSRDOInput=BSRDOInput
elif PoolRDOInput!=None:    
    globalflags.InputFormat='pool'
    if type(PoolRDOInput)==type(''):
        athenaCommonFlags.PoolRDOInput=[PoolRDOInput]
    else:
        athenaCommonFlags.PoolRDOInput=PoolRDOInput

# Conditions and geometry tag
if globalflags.InputFormat.is_pool() and (setDetDescr==None or setGlobalTag==None):
    import PyUtils.AthFile as athFile
    af = athFile.fopen(athenaCommonFlags.PoolRDOInput()[0])
    if setDetDescr==None:
        setDetDescr=af.fileinfos.get('geometry',None)
        log.info('Geometry tag not specified. Setting from file meta data: setDetDescr="%s"' % setDetDescr)
    if setGlobalTag==None:
        setGlobalTag=af.fileinfos.get('conditions_tag',None)
        log.info('Global conditions tag not specified. Setting from file meta data: setGlobalTag="%s"' % setGlobalTag)

if setDetDescr==None:
    raise RuntimeError('No geometry tag specified. Please use "setDetDescr" to set it.')
if setGlobalTag==None:
    raise RuntimeError('No global conditions tag specified. Please use "setGlobalTag" to set it.')

if useCOMCONDDB:
    globalflags.DetGeo='commis'
    globalflags.DataSource='data'
    if useCONDBR2:
        if hasattr(globalflags,'DatabaseInstance'):
            globalflags.DatabaseInstance='CONDBR2'
        else:
            log.error('This release does not support the use of CONDBR2')
    else:
        if hasattr(globalflags,'DatabaseInstance'):
            globalflags.DatabaseInstance='COMP200'
else:
    globalflags.DetGeo='atlas'
    globalflags.DataSource='geant4'
        
globalflags.DetDescrVersion=setDetDescr
globalflags.Luminosity='zero'
globalflags.ConditionsTag=setGlobalTag

from AthenaCommon.BeamFlags import jobproperties
if ('Cosmic' in setMenu) or ('LS1_v1' in setMenu):
    jobproperties.Beam.beamType = 'cosmics'
else:
    jobproperties.Beam.beamType = 'collisions'

if globalflags.InputFormat=='bytestream':
    if '_run_number' not in dir():
        import PyUtils.AthFile as athFile
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        af = athFile.fopen(athenaCommonFlags.BSRDOInput()[0])
        _run_number = af.run_number[0]
    if _run_number>=276073:       #start of periodD1, 25ns bunch spacing 
        jobproperties.Beam.bunchSpacing=25
        log.info('Bunch spacing set to %dns for a bytestream input. It can be overriden by BunchSpacing50ns=True'
                 % jobproperties.Beam.bunchSpacing())


# RecEx flags
from RecExConfig.RecFlags import rec
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
rec.doESD = False
rec.doAOD = False
rec.doTruth = False

#-------------------------------------------------------------
# Menu
#  - allow to run individual slices by simply doing testSliceName=True
#  - for more detailed control use "addSignatures" and "removeSignatures" directly
#-------------------------------------------------------------
sliceList=[]
slicesToRun=[]
for prop in dir(TriggerFlags):
    if prop[-5:]=='Slice':
        sliceName=prop
        sliceList+=[sliceName]
        if dir().count('test'+sliceName[:-5]) and globals()['test'+sliceName[:-5]]:
            slicesToRun+=[sliceName]

if slicesToRun or emptyMenu:
    slicesToDisable=set(sliceList).difference(slicesToRun)
    #make a list of slices to disable once the menu has been setup
    for slice in slicesToDisable:
        removeSignatures[slice]='ALL'
else:
    slicesToRun=['All']

#-------------------------------------------------------------
# Apply modifiers
#-------------------------------------------------------------
for mod in modifierList:
    mod.preSetup()


# should maybe implement the actual filter with regular expressions for added flexibility 
def enableDisableChains():
    # this functions is called in menu generations after the
    # default signatures have been setup according to choice of menu
    for s in sliceList:
        slice=getattr(TriggerFlags,s)
        if s in removeSignatures:
            remove=removeSignatures[s]
            log.warn('Disabling '+str(remove)+' of '+s)
            if remove=='ALL':
                signatures_all = slice.signatures.get_Value()  # Save the chain definitions
                slice.signatures.set_Value([])                 # Remove all chains
            else:
                slice.disableSignatures(remove)
        if s in addSignatures:
            add=addSignatures[s]            
            slice.enableSignatures([sig for sig in signatures_all if sig[0] in add])
            log.info('Enabled '+str(add)+' in '+s)


from TriggerMenu.menu.GenerateMenu import GenerateMenu

GenerateMenu.overwriteSignaturesWith(enableDisableChains)


log.info("=============== Running slices: ===============")
for s in slicesToRun:
    log.info(s)
log.info("===============================================")


#-------------------------------------------------------------
# Setup trigger
#-------------------------------------------------------------
include("TriggerRelease/Trigger_topOptions_standalone.py")
log = logging.getLogger('runHLT_standalone.py')

# ----------------------------------------------------------------
# Number of events to be processed - for athena
# ----------------------------------------------------------------
theApp.EvtMax = EvtMax
if hasattr(svcMgr,"EventSelector"):
    svcMgr.EventSelector.SkipEvents = SkipEvents 
    
#-------------------------------------------------------------
# Apply modifiers
#-------------------------------------------------------------
for mod in modifierList:
    mod.postSetup()

#-------------------------------------------------------------
# Conditions overrides
#-------------------------------------------------------------    
if len(condOverride)>0:
    from IOVDbSvc.CondDB import conddb    
    for folder,tag in condOverride.iteritems():
        log.warn('Overriding folder %s with tag %s' % (folder,tag))
        conddb.addOverride(folder,tag)
                
if not TriggerFlags.Online.doValidation():  # Suppress this printout in ATN tests
    from AthenaCommon.JobProperties import jobproperties
    jobproperties.print_JobProperties('tree&value')
    print AlgSequence
    print ServiceMgr
