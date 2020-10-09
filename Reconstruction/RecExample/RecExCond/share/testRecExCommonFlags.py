# standalone python to test RecExCommon_flags.py
# copy ine one's directory
# python testRecExCommonFlags.py

PoolRDOInput=["LFN:streamInclEle_DC3-02_RDO.pool"]
DetDescrVersion="ATLAS-DC3-02"


doTrigger = True # for example do not run trigger simulation


include ("RecExCommon/RecExCommon_flags.py")

# test: 
from AthenaCommon.GlobalFlags  import globalflags
#if globalflags.InputFormat()=='bytestream': # jobproperties.GlobalFlags.InputFormat() is equivalent
#   dosomething
print(globalflags.DetDescrVersion())

if globalflags.DetDescrVersion()[0:3]=="DC2":
    print(" this is DC2 ")
else:
    print(" this is not DC2 ")

from RecExConfig.RecFlags import recAlgs
if recAlgs.doTrigger() ^ doTrigger:
    print("recAlgs.doTrigger() and doTrigger inconsistent")
else:
    print("recAlgs.doTrigger() and doTrigger consistent")


# stop the hard way
stop
