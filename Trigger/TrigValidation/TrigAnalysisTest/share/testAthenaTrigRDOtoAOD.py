#still needs a lot of cleaning ... 

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not acf.EvtMax.is_locked():
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO
#scan for RTT files (only if dsName and fileRange set)
include("TriggerTest/TrigScanFiles.py")
###############################

doTrigger=True
TriggerModernConfig=True
rec.doWriteAOD=True
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=True
#rec.doESD.set_Value_and_Lock(False) 
rec.doESD=True
doTAG=False
rec.doCBNT=False 
#rec.doTruth=True

#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------

# set up trigger monitoring                                                                                                                                                        
if not ('RunningRTT' in dir()):
    TriggerFlags.enableMonitoring = [ 'Validation', 'Time' , 'Log' ]
else:
    TriggerFlags.enableMonitoring = [ 'Validation', 'Time' ]

#------------ This is for ATN/RTT tests only ---------
#TriggerFlags.triggerMenuSetup = 'default'
TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False
if  ('menu' in dir()):
    TriggerFlags.triggerMenuSetup=menu 

TriggerFlags.doHLT=True
TriggerFlags.L1PrescaleSet  = '' 
TriggerFlags.HLTPrescaleSet = '' 
TriggerFlags.AODEDMSet="AODFULL"




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


#-------------end of flag for tests-------------------

#------------ This is a temporary fix ---------------
#from RecExConfig.RecConfFlags import recConfFlags
#recConfFlags.AllowIgnoreConfigError=False
#athenaCommonFlags.AllowIgnoreConfigError=False
#-------------end of temporary fix-------------------

#from ParticleBuilderOptions.AODFlags import AODFlags 
#AODFlags.FastSimulation=False 
# see comments in https://savannah.cern.ch/bugs/?83735
#AODFlags.MuonTrackSlimmer=False
#print AODFlags.Print()

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

# abort when there is an unchecked status code
StatusCodeSvc.AbortOnError=False

include("TriggerTest/TriggerTestCommon.py")
