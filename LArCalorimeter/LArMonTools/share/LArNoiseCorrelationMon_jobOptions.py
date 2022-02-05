#from LArCellRec.LArNoisyROFlags import larNoisyROFlags

# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0
if 'doNoiseCorr' not in dir():
    doNoiseCorr = True
if 'doCNF' not in dir():
    doCNF = True


#Run is Online or Offline:
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
OnlineMode=athenaCommonFlags.isOnline()


#some FEBs to be monitored (updated 04 may 2018)
MiniNoiseBurstFlagging_FEBs = ["barrelAft09slot02","barrelAft09slot14","barrelAft10slot10","barrelAft10slot14","barrelAft31slot14","barrelCft08slot14","barrelCft22slot07","barrelCft27slot05","barrelCft27slot14"]
NewlyNoisy_FEBs = ["barrelCft02slot14","barrelCft01slot14","barrelCft07slot09","barrelCft07slot11","barrelCft06slot14"]
NoisyRegionsInOuterWheel_FEBs = ["endcapCft21slot04","endcapCft21slot02","endcapCft02slot04","endcapCft02slot02","endcapCft09slot02","endcapCft09slot03","endcapAft21slot04","endcapAft21slot02","endcapAft21slot03"]
NotNoisyRegionForComparison_FEBs = ["endcapAft02slot04"]
FEBs_from_DQ_run_350440 = ["endcapAft19slot12","endcapAft19slot09","endcapAft20slot09"]

defaultFEBs=NotNoisyRegionForComparison_FEBs+NoisyRegionsInOuterWheel_FEBs+NewlyNoisy_FEBs+MiniNoiseBurstFlagging_FEBs+FEBs_from_DQ_run_350440
if 'coherent_noise_febs' in dir():
    defaultFEBs=coherent_noise_febs
    pass

if doNoiseCorr:
   ###### LArNoiseCorrelationMon Configuration ###############
   from LArMonTools.LArMonToolsConf import LArNoiseCorrelationMon
   theLArNoiseCorrelationMon = LArNoiseCorrelationMon(name="LArNoiseCorrelationMon",
                             LArDigitContainerKey  = LArMonFlags.LArDigitKey(),
                             IsOnline              = OnlineMode,
                             IgnoreBadChannels     = True,
                             ProblemsToMask        = ProblemsToMask,
                             ProcessNEvents        = EventBlockSize,
                             TriggerChain          = "HLT_noalg_zb_L1ZB, HLT_noalg_cosmiccalo_L1RD1_EMPTY",
                             FEBsToMonitor         = defaultFEBs,
                             IsCalibrationRun      = False,
                             )




   if 'coherent_noise_calibration_run' in dir():
       if coherent_noise_calibration_run:
          theLArNoiseCorrelationMon.IsCalibrationRun=True
          theLArNoiseCorrelationMon.LArDigitContainerKey=Gain
          theLArNoiseCorrelationMon.TriggerChain=""
          theLArNoiseCorrelationMon.TrigDecisionTool=""
          pass
       pass                

   if 'coherent_noise_PublishPartialSums' in dir():
       theLArNoiseCorrelationMon.PublishPartialSums = coherent_noise_PublishPartialSums
       pass

   LArMon.AthenaMonTools+=[ theLArNoiseCorrelationMon ] 

if doCNF:
   ###### LArCoherentNoisefractionMon Configuration ###############
   from LArMonTools.LArMonToolsConf import LArCoherentNoisefractionMon
   theLArCNFMon = LArCoherentNoisefractionMon(name="LArCoherentNoisefractionMon",
                             LArDigitContainerKey  = LArMonFlags.LArDigitKey(),
                             IsOnline              = OnlineMode,
                             ProcessNEvents        = EventBlockSize,
                             TriggerChain          = "HLT_noalg_zb_L1ZB, HLT_noalg_cosmiccalo_L1RD1_EMPTY",
                             FEBsToMonitor         = defaultFEBs,
                             IsCalibrationRun      = False,
                             OutputLevel = WARNING
                             )

   if 'coherent_noise_calibration_run' in dir():
       if coherent_noise_calibration_run:
          theLArCNFMon.IsCalibrationRun=True
          theLArCNFMon.LArDigitContainerKey=Gain
          theLArCNFMon.TriggerChain=""
          theLArCNFMon.TrigDecisionTool=""
          pass
       pass

   LArMon.AthenaMonTools+=[ theLArCNFMon ]

