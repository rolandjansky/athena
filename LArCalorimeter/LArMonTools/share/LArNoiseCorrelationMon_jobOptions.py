#from LArCellRec.LArNoisyROFlags import larNoisyROFlags

# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0


#Run is Online or Offline:
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
OnlineMode=athenaCommonFlags.isOnline()


#some FEBs to be monitored (updated 04 may 2018)
MiniNoiseBurstFlagging_FEBs = ["barrelAft09slot02","barrelAft09slot14","barrelAft10slot10","barrelAft10slot14","barrelAft31slot14","barrelCft08slot14","barrelCft22slot07","barrelCft27slot05","barrelCft27slot14"]
NewlyNoisy_FEBs = ["barrelCft02slot14","barrelCft01slot14","barrelCft07slot09","barrelCft07slot11","barrelCft06slot14"]
NoisyRegionsInOuterWheel_FEBs = ["endcapCft21slot04","endcapCft21slot02","endcapCft02slot04","endcapCft02slot02","endcapCft09slot02","endcapCft09slot03","endcapAft21slot04","endcapAft21slot02","endcapAft21slot03"]
NotNoisyRegionForComparison_FEBs = ["endcapAft02slot04"]
FEBs_from_DQ_run_350440 = ["endcapAft19slot12","endcapAft19slot09","endcapAft20slot09"]


###### LArNoiseCorrelationMon Configuration ###############
from LArMonTools.LArMonToolsConf import LArNoiseCorrelationMon
theLArNoiseCorrelationMon = LArNoiseCorrelationMon(name="LArNoiseCorrelationMon",
                             LArDigitContainerKey  = LArMonFlags.LArDigitKey(),
                             IsOnline              = OnlineMode,
                             IgnoreBadChannels     = True,
                             LArBadChannelMask     = theLArBadChannelsMasker,
                             ProcessNEvents        = EventBlockSize,
                             TriggerChain          = "HLT_noalg_zb_L1ZB, HLT_noalg_cosmiccalo_L1RD1_EMPTY",
                             FEBsToMonitor         = NotNoisyRegionForComparison_FEBs+NoisyRegionsInOuterWheel_FEBs+NewlyNoisy_FEBs+MiniNoiseBurstFlagging_FEBs+FEBs_from_DQ_run_350440,
                             IsCalibrationRun      = False,
                             )



from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Global.DataSource.get_Value() == 'data':
    theLArNoiseCorrelationMon.LArPedestalKey='Pedestal'
else:
    theLArNoiseCorrelationMon.LArPedestalKey='LArPedestal'
                

ToolSvc += theLArNoiseCorrelationMon
LArMon.AthenaMonTools+=[ theLArNoiseCorrelationMon ] 


