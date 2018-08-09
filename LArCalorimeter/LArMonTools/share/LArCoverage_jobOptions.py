# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0

###### LAr Coverage Tool Configuration ###############
from LArMonTools.LArMonToolsConf import LArCoverage
theLArCoverage = LArCoverage(name="LArCoverage",
                             ProcessNEvents             = EventBlockSize,
                             LArDigitContainerKey       = LArMonFlags.LArDigitKey(),
                             LArBadChannelMask          = theLArBadChannelsMasker,
                             Nevents                    = 40
                         )
ToolSvc += theLArCoverage
LArMon.AthenaMonTools+=[ theLArCoverage ]

# BadChannelTool configuration
from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
theLArBadChannelTool.ReadFromASCII=False
ToolSvc+=theLArBadChannelTool
ToolSvc.LArCoverage.LArBadChannelTool=theLArBadChannelTool

# KnownMNBFEBsTool configuration
if 'COMP200' not in conddb.GetInstance():
    theMNBFebTool=LArBadChanTool("KnownMNBFEBsTool")
    theMNBFebTool.CoolMissingFEBsFolder="/LAR/BadChannels/KnownMNBFEBs"
    ToolSvc+=theMNBFebTool
    ToolSvc.LArCoverage.LArKnownMNBFEBsTool=theMNBFebTool


# CaloNoiseTool configuration
from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theLArCoverageCaloNoiseTool=CaloNoiseToolDefault()
ToolSvc+=theLArCoverageCaloNoiseTool
ToolSvc.LArCoverage.LArCaloNoiseTool=theLArCoverageCaloNoiseTool

