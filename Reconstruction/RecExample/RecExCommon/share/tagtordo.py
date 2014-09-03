DetDescrVersion="ATLAS-CSC-02-00-00"
jp.AthenaCommonFlags.EvtMax=-1
readTAG=True
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolTAGInput=["TAG.pool.root"]
athenaCommonFlags.PoolInputQuery="NLooseMuon>=4"

from RecExConfig.RecFlags import rec
rec.TAGFromRDO=False 

doWriteRDO=True
PoolRDOOutput = "Selected.RDO.pool.root"

rec.doCBNT=False
rec.doHist=False
rec.readESD=False

rec.doWriteAOD=False 
rec.doAOD=False 
doESD=False
rec.doWriteESD=False
rec.doWriteTAG=False 

include ("RecExCommon/RecExCommon_topOptions.py")

