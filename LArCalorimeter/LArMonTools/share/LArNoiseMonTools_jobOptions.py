# all the following stuff was requested by Gabe for the EndCap
ToolSvc.LArDigitNoiseMonTool.febNamesFor2dCorrCoef = ["ALL"]#added by Gabe 14 Feb 2007
ToolSvc.LArDigitNoiseMonTool.regionIDs = [0] # [0] to turn on all regions, [] to turn off
# ToolSvc.LArDigitNoiseMonTool.resetHistosAtCheckHists = True
    
from LArMonTools.LArMonToolsConf import LArFebNoiseMonTool
theLArFebNoiseMonTool = LArFebNoiseMonTool(name="LArFebNoiseMonTool",
                                           LArDigitContainerKey = LArMonFlags.LArDigitKey(),
                                           histoPathBase = "LAr/FebNoise",
                                           febIDs  = [ 0 ],
                                           TriggerSign = +1,
                                           AlternateSum = False,
                                           SingleTrigger = False,
                                           ## SingleTriggerSide = 1, # 0:C side 1:A side
                                           ## SingleTriggerFT = 1,
                                           ## SingleTriggerSlot = 1
                                           )
    
ToolSvc += theLArFebNoiseMonTool
LArMon.AthenaMonTools+=[ theLArFebNoiseMonTool ] 
    
# ToolSvc.LArFebNoiseMonTool.OutputLevel=DEBUG

from LArMonTools.LArMonToolsConf import LArFebNoiseMonTool
theLArFebNoiseMonToolAlt = LArFebNoiseMonTool(name="LArFebNoiseMonToolAlt",
                                              LArDigitContainerKey = LArMonFlags.LArDigitKey(),
                                              histoPathBase = "LAr/FebNoiseAlt",
                                              febIDs  = [ 0 ],
                                              TriggerSign = +1,
                                              AlternateSum = True,
                                              SingleTrigger = False,
                                              ## SingleTriggerSide = 1, # 0:C side 1:A side
                                              ## SingleTriggerFT = 1,
                                              ## SingleTriggerSlot = 1
                                              )
    
ToolSvc += theLArFebNoiseMonToolAlt
LArMon.AthenaMonTools+=[ theLArFebNoiseMonToolAlt ] 
    
# ToolSvc.LArFebNoiseMonToolAlt.OutputLevel=DEBUG
