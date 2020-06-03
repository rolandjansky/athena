# Define ONE LArBadChannelMask for all Monitoring tools
conddb.addFolder("LAR_ONL","/LAR/BadChannels/BadChannels")
conddb.addFolder("LAR_ONL","/LAR/BadChannels/MissingFEBs")
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArBadChannelsMasker=LArBadChannelMasker("LArBadChannelsMasker")
theLArBadChannelsMasker.DoMasking=True
theLArBadChannelsMasker.ProblemsToMask=[
    "deadReadout","deadPhys","short","almostDead",
    "highNoiseHG","highNoiseMG","highNoiseLG","sporadicBurstNoise"
    ]
ToolSvc+=theLArBadChannelsMasker

#include("LArMonTools/LArFEBMon_jobOptions.py") # Data integrity checks - Includes LArFEBMon tool and LArEventInfo
if Type == 'Pedestal':
#    include("LArMonTools/LArDigitNoiseMonTool_jobOptions.py")       # Monitoring of digits (noise, pedestal, correlation...)
#    include("LArMonTools/LArOddCellsMonTool_jobOptions.py")         # Monitoring of digits compared with DB pedestal / noise
     from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
     theLArCondSvc=LArFlatConditionSvc()
     svcMgr+=theLArCondSvc
     svcMgr.ProxyProviderSvc.ProviderNames += [ "LArFlatConditionSvc" ]  
     conddb.addFolder("LAR_ONL","/LAR/ElecCalibFlat/Pedestal")
     theLArCondSvc.PedestalInput="/LAR/ElecCalibFlat/Pedestal"
     include("LArMonTools/LArNoiseCorrelationMon_jobOptions.py")    # Coherent noise plots
     ToolSvc.LArNoiseCorrelationMon.IsCalibrationRun=True
     ToolSvc.LArNoiseCorrelationMon.LArDigitContainerKey=Gain
     ToolSvc.LArNoiseCorrelationMon.TriggerChain=""
     ToolSvc.LArNoiseCorrelationMon.TrigDecisionTool=""
#    include("LArMonTools/LArRawChannelNoiseMonTool_jobOptions.py") # Monitoring of raw channels (noise, pedestal, correlation...)
#    include("LArMonTools/LArScaNoiseMonTool_jobOptions.py")        # Monitoring of fixed pattern noise
#    include("LArMonTools/LArFebNoiseMonTool_jobOptions.py")        # Monitoring of noise shape (using FEB as a scope)
#    include("LArMonTools/LArFebNoiseMonToolAlt_jobOptions.py")     # Monitoring of noise shape (using FEB as a scope)
elif Type == 'Delay':
    include("LArMonTools/LArDelayMon_jobOptions.py") # includes LArWaveMon tool and LArJitterMon
#    include("LArMonTools/LArXtalk_jobOptions.py")    # includes LArWaveXtalk tool
elif Type == 'Ramp':
    include("LArMonTools/LArRampMon_jobOptions.py")
elif Type == 'Cosmic':
    include("LArMonTools/LArDigitMon_jobOptions.py")
else:
    print "Unknown Type !!!"
