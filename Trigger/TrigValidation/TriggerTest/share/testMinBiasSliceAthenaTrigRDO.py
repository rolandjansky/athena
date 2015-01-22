### this ATN test runs with following RDO input:

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

DetDescrVersion="ATLAS-GEO-16-00-00"
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/validation-data/mc10_7TeV.105001.pythia_minbias.digit.RDO.e574_s932_s946_d369/RDO.197112._000001.pool.root.1"]


#if not ('PoolRDOInput' in dir()):
#    PoolRDOInput=["root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid1.105003.pythia_sdiff.digit.RDO.e380_s511_d159_tid044456/RDO.044456._00001.pool.root.1"]
#
#if not ('DetDescrVersion' in dir()):
#    DetDescrVersion="ATLAS-GEO-06-00-00"

if not acf.EvtMax.is_locked():
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO
#scan for RTT files (only if dsName and fileRange set)
#include("TriggerTest/TrigScanFiles.py")

###############################
rec.doCBNT=False


doTrigger=True
rec.doESD.set_Value_and_Lock(False)

TriggerModernConfig=True

rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False 
rec.doESD.set_Value_and_Lock(False) 
doTAG=False

#rec.doTruth=True
rec.doTruth.set_Value_and_Lock(False)


#-----------------------------------------------------------
include("RecExCond/RecExCommon_flags.py")
#-----------------------------------------------------------
#TriggerFlags.triggerMenuSetup = 'MC_pp_v5'
if  ('menu' in dir()):
    TriggerFlags.triggerMenuSetup=menu 
else:
    TriggerFlags.triggerMenuSetup = 'Physics_pp_v5'

TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False
#TriggerFlags.useL1CaloCalibration = False
TriggerFlags.doHLT=True
#TriggerFlags.doL1Topo=True 


# set up trigger monitoring                                                                                                                                                        
if not ('RunningRTT' in dir()):
    TriggerFlags.enableMonitoring = [ 'Validation', 'Time' , 'Log' ]
else:
    TriggerFlags.enableMonitoring = [ 'Validation', 'Time'  ]


###### This is a temporary fix ################
#DetFlags.simulateLVL1.Calo_setOn()

def MinBiasOnly():
    TriggerFlags.Slices_all_setOff()
    TriggerFlags.MinBiasSlice.setAll()
    
    # 2014-06-26 - removing unused L1_ZDC sim
    mbL1Items = ['L1_BCM_Wide', 'L1_LUCID', 'L1_MBTS_2', 'L1_MBTS_1_1', 'L1_TE20', 'L1_RD0_FILLED', 'L1_MBTS_2_UNPAIRED_ISO', 'L1_MBTS_4_4',]
    for L1item in mbL1Items:        
        if not L1item in TriggerFlags.Lvl1.items():
            TriggerFlags.Lvl1.items = TriggerFlags.Lvl1.items() + [L1item]
    
    TriggerFlags.MinBiasSlice.signatures = [
                                            #MBTS at L2
                                            ['mb_mbts_L1MBTS_2', 'L1_MBTS_2', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
                                            #['mb_mbts_L1MBTS_2_UNPAIRED_ISO',   'L1_MBTS_2_UNPAIRED_ISO', [], ['MinBias'],["BW:Unpaired_Minbias", "RATE:MinBias"], 1],
                                            #['mb_mbts_L1MBTS_4_4',   'L1_MBTS_4_4', [], ['MinBias'],["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_perf_L1LUCID',           'L1_LUCID',        [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_perf_L1MBTS_1', 'L1_MBTS_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            #['mb_noalg_L1MBTS_2',         'L1_MBTS_2',        [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sptrk', 'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sptrk_noisesup',   'L1_RD0_FILLED', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sp2000_trk70_hmt',   'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sptrk_L1MBTS_1',  'L1_MBTS_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sptrk_noisesup_L1MBTS_1',  'L1_MBTS_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sp2000_trk70_hmt_L1MBTS_1', 'L1_MBTS_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],                  
                                            ['mb_sptrk_L1MBTS_2',   'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sptrk_noisesup_L1MBTS_2',   'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sp2000_trk70_hmt_L1MBTS_2',  'L1_MBTS_2', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sp2000_pusup600_trk70_hmt',  'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sp2000_pusup600_trk70_hmt_L1TE30',   'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sp2000_pusup600_trk70_hmt_L1TE40',  'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sp2000_pusup700_trk70_hmt_L1TE30',   'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sp2000_pusup700_trk70_hmt_L1TE40',   'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sp2000_pusup600_trk70_hmt_L1MBTS_1',   'L1_MBTS_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ['mb_sptrk_costr_L1MBTS_1',  'L1_MBTS_1', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            #['noalg_mb_L1TE20',  'L1_TE20', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            #['noalg_mb_L1TE30',  'L1_TE30', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            #['noalg_mb_L1TE40', 'L1_TE40', [], ['MinBias'], ["BW:MinBias", "RATE:MinBias"], 1],
                                            ]

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
GenerateMenu.overwriteSignaturesWith(MinBiasOnly)

###### This is a temporary fix ################
#DetFlags.simulateLVL1.Muon_setOn()

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

#------------------------------------------------------------
include("TriggerTest/TriggerTestCommon.py")
#------------------------------------------------------------

# Turn on debugging in TrigT1BCM, TrigT1Lucid, TrigT1MBTS
from AthenaCommon.AlgSequence import AlgSequence    
topSeq = AlgSequence()
if hasattr(topSeq, "LVL1::TrigT1BCM"):
    alg = getattr(topSeq, "LVL1::TrigT1BCM")
    alg.OutputLevel = DEBUG
if hasattr(topSeq, "LVL1::TrigT1Lucid"):
    alg = getattr(topSeq, "LVL1::TrigT1Lucid")
    alg.OutputLevel = DEBUG
if hasattr(topSeq, "LVL1::TrigT1MBTS"):
    alg = getattr(topSeq, "LVL1::TrigT1MBTS")
    alg.OutputLevel = DEBUG

#from TrigSteerMonitor.TrigSteerMonitorConf import TrigChainMoni
#chainMoni = TrigChainMoni()
#chainMoni.OutputLevel=VERBOSE
#topSequence.TrigSteer_HLT.MonTools += [chainMoni]
##topSequence.TrigSteer_HLT.OutputLevel=DEBUG

