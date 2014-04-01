# This job options shows examples of how to override conditions database folders
# See https://twiki.cern.ch/twiki/bin/view/Atlas/CoolIOVDbSvcConfigurable

from IOVDbSvc.CondDB import conddb

# To block folders. This is needed if you use the CondProxyProvider or can also
# be used to disable the alignments. It is also needed to override with a
# local sqlite file (see below)
#conddb.blockFolder("/Indet/Align")
#conddb.blockFolder("/TRT/Align")

## Default alignments
## If GlobalTag is not set it will use the same tags as used in the simulation.
## If GlobalTag is set then it will use the set associated with that tag
## This is not needed as it is the default
#conddb.setGlobalTag("")

## CSC As-built misalignments
#conddb.setGlobalTag("OFLCOND-CSC-00-01-00")

## Null alignment set
#conddb.setGlobalTag("OFLCOND-CSC-00-00-00")

## To override the folder tags.
#conddb.addOverride('/Indet/Align','newfoldertag')
#conddb.addOverride('/TRT/Align','newfoldertag')

# Some example
#conddb.addOverride('/Indet/Align','InDetAlign_CSC_02')
#conddb.addOverride('/TRT/Align','TRTAlign_CSC_01')

#conddb.addOverride('/Indet/Align','InDetAlign_ResIniPix_00')
#conddb.addOverride('/Indet/Align','InDetAlign_CSC_02_ResIniPix_00')
#conddb.addOverride('/Indet/Align','InDetAlign_ResFinalPix_00')
#conddb.addOverride('/Indet/Align','InDetAlign_CSC_02_ResFinalPix_00')

## or from an sqlite file mycool.db (the force=True is need because of the blockFolder command)
#conddb.blockFolder("/XYZ/myfolder") 
#conddb.addFolderWithTag("LOCAL","/XYZ/myfolder","mytag",force=True)

## eg
#conddb.blockFolder("/Indet/Align")
#conddb.addFolderWithTag("LOCAL","/Indet/Align","mytag",force=True)
#conddb.blockFolder("/TRT/Align")
#conddb.addFolderWithTag("LOCAL","/TRT/Align","mytag",force=True)


## or if you need to specify another SQLITE file you can use
#conddb.blockFolder("/my/folder") 
#conddb.addFolder("","<dbConnection>sqlite://;schema=myfile.db;dbname=OFLP200</dbConnection> /my/folder" +
#                 "<tag>mytag</tag>", force=True)

## eg
#conddb.blockFolder("/Indet/Align")
#conddb.addFolder("","<dbConnection>qlite://;schema=myfile.db;dbname=OFLP200</dbConnection> /Indet/Align" +
#                 "<tag>mytag</tag>", force=True)
#conddb.blockFolder("/TRT/Align")
#conddb.addFolder("","<dbConnection>sqlite://;schema=myfile.db;dbname=OFLP200</dbConnection> /TRT/Align" +
#                 "<tag>mytag</tag>", force=True)









