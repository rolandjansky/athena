# Bytestream to TrigT1Calo objects conversions
include.block("TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags       import globalflags
from RecExConfig.AutoConfiguration  import GetRunNumber
# Need something better eventually
if athenaCommonFlags.isOnline:
    run = 2
elif globalflags.DataSource() == "data":
    run = 1 if (GetRunNumber() < 230000) else 2
else:  # MC
    run = 2

include ("TrigT1CaloCalibConditions/L1CaloCalibConditionsRun%d_jobOptions.py" % run)
