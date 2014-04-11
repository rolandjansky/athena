# CaloCell Monitoring for Collisions,
# Evan Wulf, 12/10/09 - earwulf@nevis.columbia.edu
#
#                           D Urbaniec       15/02/09
# LArCellVecMon_jobOpt.py   Francesco Spano  07/15/07
# LArCellVecMon_jobOpt.py   R. Kehoe         07/03/06

from CaloMonitoring.CaloMonitoringConf import CaloCellVecMon
from CaloRec.CaloTopoClusterFlags import jobproperties

if not 'rec' in dir():
   from RecExConfig.RecFlags import rec

from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.GlobalFlags  import globalflags

from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
include ("AthenaMonitoring/AtlasReadyFilterTool_jobOptions.py")

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool=CaloNoiseToolDefault()
ToolSvc+=theCaloNoiseTool

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArChanMasker=LArBadChannelMasker("LArChanMasker")
theLArChanMasker.DoMasking=True
theLArChanMasker.ProblemsToMask=[
     "deadReadout","deadPhys","almostDead","short",
     "sporadicBurstNoise",
     "unstableNoiseLG","unstableNoiseMG","unstableNoiseHG",
     "highNoiseHG","highNoiseMG","highNoiseLG"
]
ToolSvc+=theLArChanMasker

from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
ToolSvc+=theLArBadChannelTool

if (DQMonFlags.monManEnvironment == 'online'):
  tmp_isOnline=TRUE
else:
  tmp_isOnline=FALSE

if (DQMonFlags.monManEnvironment == 'online' or globalflags.DataSource.get_Value() == 'geant4' or globalflags.DataSource.get_Value() == 'geant3'):
  tmp_useBadLBTool=FALSE
else:
  tmp_useBadLBTool=TRUE

if DQMonFlags.monManEnvironment() == 'online':
  tmp_useReadyFilterTool=FALSE
else:
#  tmp_useReadyFilterTool=FALSE
  tmp_useReadyFilterTool=TRUE

if DQMonFlags.monManEnvironment() == 'online':
   tmp_useLArNoisyAlg = FALSE
else:
   tmp_useLArNoisyAlg = TRUE 

tmp_useElectronicNoiseOnly = FALSE
tmp_useTwoGaus = TRUE 

if DQMonFlags.monManEnvironment() == 'online':
   tmp_useBeamBackgroundRemoval = FALSE
else:
   tmp_useBeamBackgroundRemoval = TRUE

if not (rec.triggerStream()=='CosmicCalo'):
  tmp_useBeamBackgroundRemoval = FALSE 
  print "not CosmicCalo stream" 

print "tmp_useBeamBackgroundRemoval=", tmp_useBeamBackgroundRemoval


tmp_useTrigger = TRUE  
if 'DQMonFlags' in dir():
   if not DQMonFlags.useTrigger:
      tmp_useTrigger = FALSE 

if DQMonFlags.monManEnvironment() == 'online':
   tmp_sporadicSwitch = FALSE 
else:
   tmp_sporadicSwitch = TRUE

CaloCellMon = CaloCellVecMon(
    name = "CaloCellMon",
    CaloCellContainer = "AllCalo",

    TimeGran = "run",
    ProcessNEvents = 100,  #determines eventsblock time granularity, defined in ManagedMonitorToolBase.

    isOnline=tmp_isOnline,
    useBadLBTool=tmp_useBadLBTool,
    BadLBTool = GetLArBadLBFilterTool(),

    useReadyFilterTool = tmp_useReadyFilterTool,
    ReadyFilterTool = monAtlasReadyFilterTool,

    useLArNoisyAlg = tmp_useLArNoisyAlg,
    useElectronicNoiseOnly = tmp_useElectronicNoiseOnly,
    useTwoGaus = tmp_useTwoGaus,

    CaloNoiseTool=theCaloNoiseTool,
    useBeamBackgroundRemoval = tmp_useBeamBackgroundRemoval,

    useTrigger          = tmp_useTrigger,
    rndmTriggerNames    = "L1_RD0, L1_RD0_FILLED, L1_RD0_EMPTY, L1_RD1, L1_RD1_NOISE, L1_RD1_HIST, L1_RD1_BGRP4, L1_RD1_BGRP5",
    caloTriggerNames    = "L1_EM[0-9]+, L1_HA[0-9]+, L1_J[0-9]+.*, L1_JB[0-9]+, L1_JF[0-9]+, L1_TE[0-9]+, L1_JE[0-9]+, L1_XE[0-9]+, L1_2EM[0-9]+, L1_2FJ[0-9]+, L1_2J[0-9]+,L1_3J[0-9]+.*,L1_4J[0-9]+.*,L1_5J[0-9]+,L1_6J[0-9]+,L1_FJ[0-9]+.*",
    minBiasTriggerNames = "L1_RD0_FILLED, L1_MBTS_1, L1_MBTS_2, L1_MBTS_1_1",
    metTriggerNames     = "EF_xe[0-9]+.*",
    miscTriggerNames    = "",

    LArBadChannelMask=theLArChanMasker,
    LArBadChannelTool=theLArBadChannelTool,
    MaskBadChannels    = FALSE,
    MaskNoCondChannels = FALSE,
    doInverseMasking   = FALSE,
    doMaskingOnline    = FALSE,
    problemsToMaskOffline =["deadReadout","deadPhys","almostDead","short","sporadicBurstNoise","unstableNoiseLG",
                            "unstableNoiseMG","unstableNoiseHG","highNoiseHG","highNoiseMG","highNoiseLG"],

    # Database Record Plots (filled in first event)
    doDatabaseNoiseVsEtaPhi     = TRUE,
    doKnownBadChannelsVsEtaPhi  = TRUE,
    doDBNoiseNormalized1DEnergy = TRUE,
    doUnnormalized1DEnergy      = TRUE,
    useLogarithmicEnergyBinning = TRUE,

    # Use WeightedEff rather than Weighted Average Merging, where appropriate:
    #  (if merging / errors are all correct, this should have no effect, hence this is a good validation tool)
    useWeightedEffMerging = TRUE,

    Sporadic_switch = tmp_sporadicSwitch,
    Threshold_EM_S0S1= 5000,
    Threshold_HECFCALEMS2S3= 15000,
    EventNumber_thres = 10,
    NsporadicPlotLimit = 300,
    Sporadic_protection = 4000,

    DoEtaLumi = FALSE,
    DoPhiLumi = FALSE, 
    
# EtaPhi Cell Plots:
    
#   These histograms will be filled* for every event in which the energy threshold (either absolute or DB noise
#   dependent) and trigger selection is passed. Plots can be switched on for each threshold type. If a default
#   energy threshold (!=-99999999.) is specified for a given type, plots will be produced for all layers of the
#   LAr calorimeter (EMB, EMEC, HEC, FCAL), otherwise, if thresholds are given for only certain layers, only
#   those layers will have histograms produced for this threshold type.

#   Triggers correspond to those specified above under Trigger Awareness, the logic is that an event will be used
#   to fill plots of a given threshold type iff it passes any of the triggers in the TriggersToInclude trigger
#   chain group without passing any of the triggers in the TrigersToExclude chain group.

# Global Settings:

    ThresholdType          = ["default", "noEth", "noEth_rndm", "medEth", "5Sigma", "hiEth", "hiEth_noVeto", "met"  ],
    ThresholdDirection     = ["over"   , "none" , "none"      , "both"  , "both"  , "over" , "over"        , "over" ],
    TriggersToInclude      = ["all"    , "all"  , "all"       , "all"   , "all"   , "all"  , "all"         , "met"  ],
    TriggersToExclude      = ["none"   , "none" , "none"      ],

    DoPercentageOccupancy  = [ TRUE    , FALSE  , FALSE       , TRUE    , FALSE   , FALSE  , FALSE         , FALSE  ],
    DoEtaPhiOccupancy      = [ FALSE   , FALSE  , FALSE       , TRUE    , TRUE    , TRUE   , TRUE          , TRUE   ],
    DoEtaOccupancy         = [ FALSE   , FALSE  , FALSE       , TRUE    ],
    DoPhiOccupancy         = [ FALSE   , FALSE  , FALSE       , TRUE    ],
  
    DoEtaPhiAverageEnergy  = [ FALSE   , FALSE  , TRUE        , TRUE    , TRUE    ],
    DoEtaPhiTotalEnergy    = [ FALSE   , FALSE  , FALSE       , FALSE   , TRUE    ],
    DoEtaPhiEnergyRMS      = [ FALSE   , FALSE  , TRUE        ],
    DoEtaPhiRMSvsDBnoise   = [ FALSE   , FALSE  , TRUE        ],

    DoEtaPhiAverageQuality = [ FALSE   , FALSE  , FALSE       , FALSE   , TRUE    ],
    DoEtaPhiFractionOverQth= [ FALSE   , FALSE  , FALSE       , FALSE   , FALSE   , TRUE   , TRUE          ],
    QualityFactorThreshold = [ 4000.   ],

    DoEtaPhiAverageTime    = [ FALSE   , FALSE  , FALSE       , FALSE   , TRUE    ],
    DoEtaPhiFractionPastTth= [ FALSE   , FALSE  , FALSE       , FALSE   , TRUE    ],
    TimeThreshold          = [ 4.      ],

    MaskEmptyBins          = [ FALSE   , FALSE  , FALSE       , FALSE   , FALSE   , TRUE   , TRUE          , FALSE  ], #bins w/o calocell = -1  
    DoBeamBackgroundRemoval= [ TRUE    , TRUE   , TRUE        , TRUE    , TRUE    , TRUE   , FALSE         , TRUE   ],
    
    # Defaults: (Can be over ridden by layer specific values) ; plots will be made for all layers with DefaultThreshold != -9999999
    DefaultThresholdTitles= ["default" , 
                             "no Threshold", 
                             "no Threshold",
                             "|E_{cell}| > %0.f#sigma_{noise}^{database}",
                             "E_{cell} beyond %0.f#sigma_{noise}^{database}",
                             "E_{cell} > %0.f MeV",
                             "E_{cell} > %0.f MeV",
                             "E_{cell} > %0.f MeV, MET trigger"],

    DefaultThresholdNames = ["default" ,"noEth","noEth"], ## if none specified, use threshold type for name
    DefaultThresholds     = [-9999999.,-4000000.,-4000000.    , 3.      , 5.      , 500.   , 500.          , 1000.  ],
    DefaultUseNoiseTool   = [ FALSE    , FALSE  , FALSE       , TRUE    , TRUE    , FALSE  , FALSE         , FALSE  ],
 
    ThresholdColumnType   = ["hiEth", "hiEth_noVeto"],

    # EMB
    EMBP_Thresh           = [ 800.  , 800.          ],
    EMB1_Thresh           = [ 600.  , 600.          ],
    EMB2_Thresh           = [ 600.  , 600.          ],  
    EMB3_Thresh           = [ 600.  , 600.          ],  
    # EMEC
    EMECP_Thresh          = [ 1200. , 1200.         ],  
    EMEC1_Thresh          = [ 800.  , 800.          ],  
    EMEC2_Thresh          = [ 800.  , 800.          ],  
    EMEC3_Thresh          = [ 800.  , 800.          ],  
    # HEC
    HEC0_Thresh           = [ 4000. , 4000.         ],  
    HEC1_Thresh           = [ 4000. , 4000.         ],  
    HEC2_Thresh           = [ 4000. , 4000.         ],  
    HEC3_Thresh           = [ 4000. , 4000.         ],  
    # FCAL
    FCAL1_Thresh          = [ 6000. , 6000.         ],  
    FCAL2_Thresh          = [ 6000. , 6000.         ],  
    FCAL3_Thresh          = [ 6000. , 6000.         ],
)

#if 'DQMonFlags' in dir():
#    if not DQMonFlags.useTrigger:
#        CaloCellMon.useTrigger=False

ToolSvc+=CaloCellMon
CaloMon.AthenaMonTools += [CaloCellMon]
