include.block ("CSC_Digitization/CscDigitizationTool_jobOptions.py")
print "DEPRECATION WARNING: please replace include(\"CSC_Digitization/CscDigitizationTool_jobOptions.py\")\nwith:\nfrom AthenaCommon import CfgGetter\nCfgGetter.getPublicTool(\"CscDigitizationTool\")"

from AthenaCommon.CfgGetter import getPublicTool
CscDigitizationTool = getPublicTool("CscDigitizationTool")
