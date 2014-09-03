# steering file for RDO->ESD step
# see myTopOptions.py for more info
#rec.doCBNT=False

# jp.AthenaCommonFlags.PoolRDOInput=["therdofile"]
# jp.AthenaCommonFlags.PoolESDOutput="theoutput.ESD.pool.root"

from RecExConfig.RecFlags import rec
rec.Production=True

if not 'rec.doTrigger' in dir():
    rec.doTrigger=True

# main jobOption
if rec.doTrigger:
    include ("RecExCommon/rdotoesd.py")
else:
    include ("RecExCommon/rdotoesdnotrigger.py")
