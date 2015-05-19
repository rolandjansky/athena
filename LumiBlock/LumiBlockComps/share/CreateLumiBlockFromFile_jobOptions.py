# Create LumiBlock meta data containers *before* creating the output StreamESD/AOD
from LumiBlockComps.LumiBlockCompsConf import CreateLumiBlockCollectionFromFile
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

createlb = CreateLumiBlockCollectionFromFile()
topSequence += createlb
if globalflags.DataSource()=='data' and athenaCommonFlags.isOnline==False:
    from IOVDbSvc.CondDB import conddb
    countfolder="/GLOBAL/FILECOUNT/PROMPT <tag>GlobalFileCountPrompt-Tier0</tag>"
    conddb.addFolder('GLOBAL_OFL',countfolder)
    trstreamName=str(rec.triggerStream())
    if(trstreamName != "") :
       createlb.streamName = trstreamName
