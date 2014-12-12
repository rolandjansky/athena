###########################################################################
#
# TopOptions to copy a complete set of conditions for a given run to 
# one POOL file and one sqlite file
#
# Walter.Lampl@cern.ch
#
###########################################################################


import shutil, os
from LArCalibProcessing.extractFolderInfo import *

if 'dbname' not in dir():
  dbname="CONDBR2"

if 'sqliteIn' not in dir():
  sqliteIn="freshConstants_SV.db"

if 'sqliteOut' not in dir():
  sqliteOut=sqliteIn+".out"

inputDB="sqlite://;schema="+sqliteIn+";dbname="+dbname
outputDB="sqlite://;schema="+sqliteOut+";dbname="+dbname

folderInfo=extractFolderInfo(inputDB)


shutil.copyfile(sqliteIn,sqliteOut)


mergeFoldersOFC=["/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples3bins17phases",
                 "/LAR/ElecCalibOfl/Shape/RTM/5samples3bins17phases"
                 ]


import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

include("LArCalibProcessing/LArCalib_MinimalSetup.py")

theApp.EvtMax = 1
conddb.setGlobalTag("COMCOND-ES1PST-004-01") #For id mapping

svcMgr.PoolSvc.CheckDictionary=False
svcMgr.PoolSvc.SortReplicas=False


outObjectsOFC=[]
outTagsOFC=[]

runNoForFileName=0xFFFFFFFF

for f in folderInfo:
  fn=f[0]
  type=f[1]
  key=f[2]
  tag=f[3]
  since=f[4]
  until=f[5]
  if since>0 and since<runNoForFileName:
    runNoForFileName=since
  if fn in mergeFoldersOFC and 'mu_20' in tag:
    outObjectsOFC+=["#".join((type,key,fn))]
    outTagsOFC+=[tag]
    conddb.addFolder("",fn+"<tag>"+tag+"</tag><db>"+inputDB+"</db>")

svcMgr.EventSelector.RunNumber = 2147483647

OutputFileOFC="LArConditionsOFC_"+str(runNoForFileName)+".pool.root"
if os.access(OutputFileOFC,os.F_OK):
  print "File",OutputFileOFC,"exists already, removing ...."
  os.remove(OutputFileOFC)


print svcMgr.IOVDbSvc.Folders

print "============ Single bin OFC ============="
print outObjectsOFC
print outTagsOFC

from LArCalibTools.LArCalibToolsConf import LArBlockCorrections
topSequence+=LArBlockCorrections()

from LArCalibTools.LArCalibToolsConf import ForceLoadCondObj
topSequence+=ForceLoadCondObj(ObjectList=outObjectsOFC)

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
theOutputConditionsAlgOFC=OutputConditionsAlg("OutputConditionsAlgOFC",OutputFileOFC,
                                                outObjectsOFC,outTagsOFC,True)

svcMgr.IOVDbSvc.dbConnection  = outputDB
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.DetectorStore.Dump=True


if 'poolcat' in dir():
  svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:"+poolcat
