# for reading BS events using TAG_COMM 
# need to have the BS file in your file catalogue
# either by using: catalogBytestreamFiles.sh
# or by using dq2-ls -P

from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput=['tagcomm.root']
athenaCommonFlags.PoolInputQuery.set_Value_and_Lock("EventNumber==2317247")

from RecExConfig.RecFlags import rec
rec.readRDO.set_Value_and_Lock(True)
rec.doAOD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
include ("RecExCommon/RecExCommon_topOptions.py")
