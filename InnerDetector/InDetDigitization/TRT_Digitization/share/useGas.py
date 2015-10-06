############################################################
# postInclude choice of gas mixture used in TRT straws.    #
# The default behaviour (0) is to use TRT/Cond/StatusHT;   #
# otherwise set the whole detector to (1)Xe, (2)Kr, (3)Ar. #
# Requires:                                                #  
#   TRT_Digitization-01-01-00+  TRT_PAI_Process-00-01-00+  #
############################################################
from TRT_Digitization.TRT_DigitizationConf import TRTDigitizationTool
TRTDigitizationTool.UseGasMix = 0
