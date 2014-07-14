# TrigT1Calo object containers to bytestream conversion
include.block("TrigT1CaloByteStream/WriteLVL1CaloBS_jobOptions.py")

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
    include ("TrigT1CaloByteStream/WriteLVL1CaloBSRun1_jobOptions.py")
else:
    include ("TrigT1CaloByteStream/WriteLVL1CaloBSRun2_jobOptions.py")
    #include ("TrigT1CaloByteStream/WriteLVL1CaloBSRun1Run2_jobOptions.py")
