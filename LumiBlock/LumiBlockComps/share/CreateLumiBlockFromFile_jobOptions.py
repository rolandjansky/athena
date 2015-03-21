# Create LumiBlock meta data containers *before* creating the output StreamESD/AOD
from LumiBlockComps.LumiBlockCompsConf import CreateLumiBlockCollectionFromFile
createlb = CreateLumiBlockCollectionFromFile()
topSequence += createlb
if globalflags.DataSource()=='data':
    from IOVDbSvc.CondDB import conddb
    countfolder="/GLOBAL/FILECOUNT/PROMPT <tag>GlobalFileCountPrompt-Tier0</tag>"
    conddb.addFolder('GLOBAL_OFL',countfolder)
    trstreamName=str(rec.triggerStream())
    if(trstreamName != "") :
       createlb.streamName = "physics_"+trstreamName
