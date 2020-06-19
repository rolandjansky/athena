#
# common JO for almos all slices
#
#  flags for command-line input
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
### usually ATN tests runs with following RDO input:
#PoolRDOInput=["/afs/cern.ch/atlas/offline/ReleaseData/v3/testfile/valid1.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s488_d151_tid039414_RDO.039414._00001_extract_10evt.pool.root"]
#set add your own here: 
#from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
#acf.FilesInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/validation-data/mc10_7TeV.105001.pythia_minbias.digit.RDO.e574_s932_s946_d369/RDO.197112._000001.pool.root.1"]

if not acf.EvtMax.is_locked():
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO
#scan for RTT files (only if dsName and fileRange set)
include("TriggerTest/TrigScanFiles.py")

if ('enableCostMonitoring' in dir()):
    enableCostMonitoring = True
else: 
    enableCostMonitoring = False

#RTT runs with costMonitoring on - test whether it is available in a given release
#test whether a package is useable in this release
import imp
try:
    imp.find_module('TrigCostD3PDMaker')
except:
    printfunc ('CostMonitoring packages not available, setting  enableCostMonitoring=False')
    enableCostMonitoring=False

# flags for RecExCommon
#thanks to Olya for the "magic" AOD combination - see: https://its.cern.ch/jira/browse/ATR-11211
doTrigger=True

import os
if os.environ['AtlasProject'] == 'AtlasP1HLT' or os.environ['AtlasProject'] == 'AtlasCAFHLT':
    log.info("WARNING working within AtlasProject %s, will not write out the real AOD/xAOD as would be produced by the actual Tier0/AtlasProdcution release" % os.environ['AtlasProject'])
    #    log.info("WARNING working within AtlasProject %s, will not write AOD/xAOD" % os.environ['AtlasProject'])
    #    rec.doWriteAOD.set_Value_and_Lock(False)
    #    rec.doWritexAOD.set_Value_and_Lock(False)
    #    rec.doAOD.set_Value_and_Lock(False)
    #    jp.Rec.doWriteAOD=False
    #else:
rec.doWriteAOD.set_Value_and_Lock(True)
rec.doWritexAOD.set_Value_and_Lock(True)
rec.doAOD.set_Value_and_Lock(True)
jp.Rec.doWriteAOD=True
    
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doESD.set_Value_and_Lock(False) 
doTAG=False
rec.doCBNT=False 
#rec.doTruth=True
rec.doTruth.set_Value_and_Lock(False)

#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

# set up trigger monitoring                                                                                                                                                        
if not ('RunningRTT' in dir()):
    TriggerFlags.enableMonitoring = [ 'Validation', 'Time' , 'Log' ]
else:
    TriggerFlags.enableMonitoring = [ 'Validation', 'Time'  ]

# set trigger menu
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False
#(should be set correctly for default) TriggerFlags.useL1CaloCalibration = False
#TriggerFlags.triggerMenuSetup = 'default' 
if  ('menu' in dir()):
    TriggerFlags.triggerMenuSetup=menu 

TriggerFlags.doHLT=True
#TriggerFlags.doL1Topo=True 

def egammaOnly():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.EgammaSlice.setAll()

def muonOnly():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.MuonSlice.setAll()

def jetOnly():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.JetSlice.setAll()

def bjetOnly():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.BjetSlice.setAll()

def bphysicsOnly():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.BphysicsSlice.setAll()

def metOnly():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.METSlice.setAll()

def tauOnly():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.TauSlice.setAll() 

def mubphysics():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.MuonSlice.setAll()
    TriggerFlags.BphysicsSlice.setAll()

def minbiasOnly():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.MinBiasSlice.setAll() 

def minbiasEnhanced():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.MinBiasSlice.setAll()
    mbL1Items = ['L1_BCM_Wide', 'L1_LUCID', 'L1_MBTS_2', 'L1_MBTS_1_1', 'L1_TE20', 'L1_TE30', 'L1_TE40', 'L1_RD0_FILLED', 'L1_MBTS_2_UNPAIRED_ISO', 'L1_MBTS_4_4',]
    for L1item in mbL1Items:        
        if not L1item in TriggerFlags.Lvl1.items():
            TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() + [L1item]
    mbHLTItems = [
                  # MBTS at L2
                  #['mb_mbts_L1MBTS_2', 'L1_MBTS_2', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
                  #['mb_mbts_L1MBTS_2_UNPAIRED_ISO', 'L1_MBTS_2_UNPAIRED_ISO', [], ['MinBias'],["BW:Unpaired_Minbias", "RATE:MinBias"], 1],
                  #['mb_mbts_L1MBTS_4_4',  'L1_MBTS_4_4', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1], 	
                  # Other spTrk
                  #['mb_sptrk_noisesup', 'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                  ['mb_sptrk_L1MBTS_1',  'L1_MBTS_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
                  ['mb_sptrk_L1MBTS_2',  'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
                  ['mb_sptrk_noisesup_L1MBTS_1',  'L1_MBTS_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
                  ['mb_sptrk_noisesup_L1MBTS_2',  'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
                  ['mb_sp2000_trk70_hmt_L1MBTS_1',  'L1_MBTS_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],                  
                  ['mb_sp2000_trk70_hmt_L1MBTS_2',  'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
                  ['mb_sp2000_pusup600_trk70_hmt_L1MBTS_1', 'L1_MBTS_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
                  ['mb_sptrk_costr_L1MBTS_1', 'L1_MBTS_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], -1],
                  ]
    TriggerFlags.MinBiasSlice.signatures = mbHLTItems
    for HLTitem in mbHLTItems:        
        if not HLTitem in TriggerFlags.MinBiasSlice.signatures():
            TriggerFlags.MinBiasSlice.signatures = TriggerFlags.MinBiasSlice.signatures() + [HLTitem]

# Override list of signatures in e/gamma slice
try:
    from TriggerMenu import useNewTriggerMenu
    useNewTM = useNewTriggerMenu()
    log.info("Using new TriggerMenu: %r" % useNewTM)
except:
    useNewTM = False
    log.info("Using old TriggerMenuPython since TriggerMenu.useNewTriggerMenu can't be imported")

if useNewTM:
    from TriggerMenu.menu.GenerateMenu import GenerateMenu
else:
    from TriggerMenuPython.GenerateMenu import GenerateMenu


# flags for command-line input
if  ('sliceName' in dir()):
    if  sliceName == 'egamma':
        GenerateMenu.overwriteSignaturesWith(egammaOnly)
    elif sliceName == 'muon':
        GenerateMenu.overwriteSignaturesWith(muonOnly)
    elif sliceName == 'jet':
        GenerateMenu.overwriteSignaturesWith(jetOnly)    
    elif sliceName == 'bjet':
        GenerateMenu.overwriteSignaturesWith(bjetOnly)
    elif sliceName == 'bphysics':
        GenerateMenu.overwriteSignaturesWith(bphysicsOnly)    
    #    elif sliceName == 'bphysicskstar':
    #        GenerateMenu.overwriteSignaturesWith(bphysicskstarOnly)
    elif sliceName == 'met':
        GenerateMenu.overwriteSignaturesWith(metOnly)
    elif sliceName == 'tau':
        GenerateMenu.overwriteSignaturesWith(tauOnly)
    elif sliceName == 'minbias':
        GenerateMenu.overwriteSignaturesWith(minbiasOnly)    
    elif sliceName == 'mubphysics':
        GenerateMenu.overwriteSignaturesWith(mubphysics)
    elif sliceName == 'minbiasEnhanced':
        GenerateMenu.overwriteSignaturesWith(minbiasEnhanced)    
    else:
        log.info("WARNING no sliceName given! pleace check this testconfiguration")


# pre set up trigger monitoring
if 'enableCostMonitoring' in dir() and bool(enableCostMonitoring) == True:
    import TriggerJobOpts.Modifiers
    getattr(TriggerJobOpts.Modifiers,'enableCostMonitoring')().preSetup()
    getattr(TriggerJobOpts.Modifiers,'enableCostForCAF')().preSetup()
#

#------------ This is a temporary fix ---------------
#DetFlags.simulateLVL1.Calo_setOn()
#-------------end of temporary fix-------------------

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------
from LumiBlockComps.LumiBlockMuWriterDefault import LumiBlockMuWriterDefault
LumiBlockMuWriterDefault()

# post set up trigger monitoring
if 'enableCostMonitoring' in dir() and bool(enableCostMonitoring) == True:
    import TriggerJobOpts.Modifiers
    getattr(TriggerJobOpts.Modifiers,'enableCostMonitoring')().postSetup()
    getattr(TriggerJobOpts.Modifiers,'enableCostForCAF')().postSetup()
    getattr(TriggerJobOpts.Modifiers,'enableCostD3PD')().postSetup()
    # Check if we are debugging the cost mon output - false by default
    if 'enableCostDebug' in dir() and bool(enableCostDebug) == True:
        getattr(TriggerJobOpts.Modifiers,'enableCostDebug')().postSetup()
#
#-----------------------------------------------------------
include("TriggerTest/TriggerTestCommon.py")
#-----------------------------------------------------------

#L1 DEBUG for Minbias jobs 
if 'sliceName' in dir() and 'minbias' in sliceName and hasattr(topSequence, "LVL1::TrigT1MBTS"):
    alg = getattr(topSequence, "LVL1::TrigT1MBTS")
    alg.OutputLevel = DEBUG

#from TrigSteerMonitor.TrigSteerMonitorConf import TrigChainMoni
#chainMoni = TrigChainMoni()
#chainMoni.OutputLevel=VERBOSE
#topSequence.TrigSteer_HLT.MonTools += [chainMoni]
#
#import AthenaCommon.Configurable as Configurable
#Configurable.log.setLevel( INFO )
#print (topSequence.getChildren())

#-------------------------------------------------------------
# Disable overly verbose and problematic ChronoStatSvc print-out
#-------------------------------------------------------------
include("TriggerTest/disableChronoStatSvcPrintout.py")
