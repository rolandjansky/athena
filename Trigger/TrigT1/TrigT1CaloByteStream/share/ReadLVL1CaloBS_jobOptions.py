# Bytestream to TrigT1Calo objects conversions
include.block("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags       import globalflags
from RecExConfig.AutoConfiguration  import GetRunNumber
# Need something better eventually
if athenaCommonFlags.isOnline:
    run1 = False
elif globalflags.DataSource() == "data":
    run1 = (GetRunNumber() < 230000)
else:  # MC
    run1 = False
if run1:
    include ("TrigT1CaloByteStream/ReadLVL1CaloBSRun1_jobOptions.py")
else:
    include ("TrigT1CaloByteStream/ReadLVL1CaloBSRun2_jobOptions.py")
