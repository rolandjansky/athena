from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import sys
#
# run MT and PT without prescales
#
# apparently these options need to be set here and not 
# in config file
test1E31MC=True 
setPrescale= ''
setLVL1XML='../AthenaP1RDO/outputLVL1config.xml'
include("TriggerRelease/runHLT_standalone.py")
