#from LArCellRec.LArNoisyROFlags import larNoisyROFlags

# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0


#Run is Online or Offline:
OnlineMode=athenaCommonFlags.isOnline()


#check if there are FEBs to monitor
if 'febs_for_coherent_noise_mon' not in dir():
    febs_for_coherent_noise_mon=[]



###### LArNoiseCorrelationMon Configuration ###############
from LArMonTools.LArMonToolsConf import LArNoiseCorrelationMon
theLArNoiseCorrelationMon = LArNoiseCorrelationMon(name="LArNoiseCorrelationMon",
                             LArDigitContainerKey  = LArMonFlags.LArDigitKey(),
                             IsOnline              = OnlineMode,
                             IgnoreBadChannels     = True,
                             LArBadChannelMask     = theLArBadChannelsMasker,
                             ProcessNEvents        = EventBlockSize,
                             TriggerChain          = "HLT_noalg_zb_L1ZB",
                             FEBsToMonitor         = febs_for_coherent_noise_mon
                             )

from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Global.DataSource.get_Value() == 'data':
    theLArNoiseCorrelationMon.LArPedestalKey='Pedestal'
else:
    theLArNoiseCorrelationMon.LArPedestalKey='LArPedestal'
                

ToolSvc += theLArNoiseCorrelationMon
LArMon.AthenaMonTools+=[ theLArNoiseCorrelationMon ] 


