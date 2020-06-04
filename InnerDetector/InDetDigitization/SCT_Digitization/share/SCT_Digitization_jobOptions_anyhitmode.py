# JobOption fragment to activate any hit mode

# Get the SCT Digitization Alg
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
theSCT_Digitization = topSequence.SCT_Digitization

# Get the SCT FrontEnd Tool
from AthenaCommon.AppMgr import ToolSvc
from SCT_Digitization.SCT_DigitizationConf import SCT_FrontEnd
theSCT_FrontEnd = SCT_FrontEnd("SCT_FrontEnd")

# DataCompressionMode: 1 is level mode X1X (default), 2 is edge mode 01X, 3 is any hit mode (1XX|X1X|XX1)
theSCT_FrontEnd.DataCompressionMode = 3

ToolSvc += theSCT_FrontEnd

