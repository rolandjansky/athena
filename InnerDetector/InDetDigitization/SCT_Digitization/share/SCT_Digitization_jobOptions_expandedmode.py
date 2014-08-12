# JobOption fragment to activate expanded any hit mode

# Get the SCT Digitization Alg
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
theSCT_Digitization = topSequence.SCT_Digitization

# Get the SCT FrontEnd Tool
from AthenaCommon.AppMgr import ToolSvc
from SCT_Digitization.SCT_DigitizationConf import SCT_FrontEnd
theSCT_FrontEnd = SCT_FrontEnd("SCT_FrontEnd")

# DataCompressionMode: 1 is level mode(default), 2 is edge mode, 3 is expanded any hit mode
theSCT_FrontEnd.DataCompressionMode = 3

ToolSvc += theSCT_FrontEnd

