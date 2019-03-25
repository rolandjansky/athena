# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0

###### LAr HV Correction Tool Configuration ###############
from LArMonTools.LArMonToolsConf import LArHVCorrectionMonTool
theLArHVCorrectionMonTool = LArHVCorrectionMonTool(name="LArHVCorrectionMonTool",
                                                   NumberOfLBs = 1000
                                                   )
# include LAr HV Correction cond alg
from LArConditionsCommon import LArHVDB

#ToolSvc += theLArHVCorrectionMonTool
LArMon.AthenaMonTools+=[ theLArHVCorrectionMonTool ]

