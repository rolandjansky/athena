DetDescrVersion="ATLAS-CSC-02-00-00"
readTAG=True
jp.AthenaCommonFlags.EvtMax=-1
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolTAGInput=["TAG.pool.root"]
athenaCommonFlags.PoolInputQuery="NLooseMuon>=4"

from RecExConfig.RecFlags import rec
rec.TAGFromRDO=False 

rec.doCBNT=False
rec.doHist=False
rec.readESD=True
doJiveXML=True

rec.doWriteAOD=False 
rec.doAOD=False 
doESD=False
rec.doWriteESD=False
rec.doWriteTAG=False 

include ("RecExCommon/RecExCommon_topOptions.py")

