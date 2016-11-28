## @file TrigT1CaloMonitoring_forRecExCommission.py
#
# Standard monitoring jobOptions - runs on Tier0 (Reco_tf.py) or online.
#
# @authors Johanna Fleckner, Andrea Neusiedl, Peter Faulkner, Vasiliki Kouskoura
#
include.block("TrigT1Monitoring/TrigT1Monitoring_forRecExCommission.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags       import globalflags
from RecExConfig.AutoConfiguration  import GetRunNumber

if athenaCommonFlags.isOnline:
    run1 = False
elif globalflags.DataSource() == "data":
    run1 = (GetRunNumber() < 230000)
else: #MC
    run1 = False 
if run1:
    include ("TrigT1Monitoring/TrigT1Monitoring_forRecExCommission_Run1.py")
else:
    include ("TrigT1Monitoring/TrigT1Monitoring_forRecExCommission_Run2.py")
