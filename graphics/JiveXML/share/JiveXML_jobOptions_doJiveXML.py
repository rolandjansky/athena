### minimal jOs, equivalent to:
###    athena -c 'doJiveXML=True' myTopOptions.py
###
jp.Rec.doJiveXML.set_Value_and_Lock(True)
### 'pure' reco:
#include ("RecExCommon/RecExCommon_topOptions.py")
### for rel.19, use 'hacked' reco:
include ("RecExCommon/myTopOptions.py")
