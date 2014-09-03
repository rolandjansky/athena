# steering file for TAG->ESD step


#lines to specify that the reading is TAG driven
readTAG=True
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolTAGInput=["SimpleTAG.pool.root"]
# logical selection
#athenaCommonFlags.PoolInputQuery="EventNumber >= 1 && RunNumber!=1234 "
# if reading from RDO
from RecExConfig.RecFlags import rec
rec.TAGFromRDO=True 


include("RecExCommon/rdotoesd.py")

