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
#ToolSvc += theLArCoverage
LArMon.AthenaMonTools+=[ theLArCoverage ]

# CaloNoiseTool configuration
from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theLArCoverageCaloNoiseTool=CaloNoiseToolDefault()
ToolSvc+=theLArCoverageCaloNoiseTool
LArCoverage.LArCaloNoiseTool=theLArCoverageCaloNoiseTool

