
from AthenaCommon import CfgGetter
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
#
# this will create one digitization with all default properties
#
job += CfgGetter.getAlgorithm("MDT_Digitizer/MDT_Digitizer",tryDefaultConfigurable=True)
#
from MDT_Digitization.MDT_DigitizationConf import MDT_Response_DigiTool
# number of ionisation clusters per mm. defaul=8.5
MDT_Response_DigiTool.ClusterDensity = 8.5
# calculation of energy deposition for Qballs with account of gamma. 
MDT_Response_DigiTool.DoQballGamma = FALSE
#
from MDT_Digitization.MDT_DigitizationConf import MdtDigitizationTool
# calculation of energy deposition for Qballs with account of electric charge. 
MdtDigitizationTool.DoQballCharge = FALSE
