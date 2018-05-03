#from LArCellRec.LArNoisyROFlags import larNoisyROFlags

# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0


#Run is Online or Offline:
OnlineMode=athenaCommonFlags.isOnline()


###### LArNoiseCorrelationMon Configuration ###############
from LArMonTools.LArMonToolsConf import LArNoiseCorrelationMon
theLArNoiseCorrelationMon = LArNoiseCorrelationMon(name="LArNoiseCorrelationMon",
                             LArDigitContainerKey  = LArMonFlags.LArDigitKey(),
                             IsOnline              = OnlineMode,
                             IgnoreBadChannels     = True,
                             LArBadChannelMask     = theLArBadChannelsMasker,
                             ProcessNEvents        = EventBlockSize,
                             TriggerChain          = "HLT_noalg_zb_L1ZB, HLT_noalg_cosmiccalo_L1RD1_EMPTY",
                             TrigDecisionTool      = "Trig::TrigDecisionTool/TrigDecisionTool",
                             FEBsToMonitor         = ["barrelaft09slot01","barrelaft10slot13","endcapCFT17slot04","EndCapAFT06Slot09"],
                             IsCalibrationRun      = False
                             )

from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Global.DataSource.get_Value() == 'data':
    theLArNoiseCorrelationMon.LArPedestalKey='Pedestal'
else:
    theLArNoiseCorrelationMon.LArPedestalKey='LArPedestal'
                

ToolSvc += theLArNoiseCorrelationMon
LArMon.AthenaMonTools+=[ theLArNoiseCorrelationMon ] 


