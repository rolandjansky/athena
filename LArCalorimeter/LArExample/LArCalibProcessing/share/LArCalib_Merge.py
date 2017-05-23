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
  sqliteIn="freshConstants.db"

if 'sqliteOut' not in dir():
  sqliteOut=sqliteIn+".out"

inputDB="sqlite://;schema="+sqliteIn+";dbname="+dbname
outputDB="sqlite://;schema="+sqliteOut+";dbname="+dbname

folderInfo=extractFolderInfo(inputDB)

#ccs="<channelSelection>3:238</channelSelection>"

if 'mergeFolders' not in dir():
  mergeFolders=["/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples",
                "/LAR/ElecCalibOfl/Shape/RTM/5samples"
                ]

if 'fileName' not in dir():
  fileName="LArConditions"

shutil.copyfile(sqliteIn,sqliteOut)



import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector

#FIXME: Remove the following 2 lines as soon as AP has migrated to CONDBR2
from AthenaCommon.GlobalFlags import globalflags
globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')
include("LArCalibProcessing/LArCalib_MinimalSetup.py")

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

theApp.EvtMax = 1
conddb.setGlobalTag("LARCALIB-RUN2-00") #For id mapping
svcMgr.IOVDbSvc.DBInstance=""

svcMgr.PoolSvc.CheckDictionary=False
svcMgr.PoolSvc.SortReplicas=False

if 'IOVBegin' in dir():
   runNoForFileName=IOVBegin
else:   
   runNoForFileName=2147483647

outObjects=[]
outTags=[]

if 'keySuffix' not in dir():
  keySuffix="_1ns"

for f in folderInfo:
  fn=f[0]
  type=f[1]
  key=f[2]
  tag=f[3]
  since=f[4]
  until=f[5]
  print "Working on folder",fn,"IOV:",since,"-",until
  #if since == 0:
  #  print "ERROR: Found IOV begin = 0"
    #sys.exit(-1)
  if 'OFC' in fn and not 'mu_20' in tag:
     continue
  if fn in mergeFolders:
    if since>0 and since<runNoForFileName and 'IOVBegin' not in dir():
      runNoForFileName=since
    if len(keySuffix)>0 and key.endswith(keySuffix):
      key=key[:-len(keySuffix)]
    outObjects+=["#".join((type,key,fn))]
    outTags+=[tag]
    conddb.addFolder("",fn+"<tag>"+tag+"</tag><db>"+inputDB+"</db><key>"+key+"</key>")

svcMgr.EventSelector.RunNumber = 2147483647

OutputFileRec=fileName+"_"+str(runNoForFileName)+".pool.root"
if os.access(OutputFileRec,os.F_OK):
  print "File",OutputFileRec,"exists already, removing ...."
  os.remove(OutputFileRec)

print svcMgr.IOVDbSvc.Folders

print "============ Output objects ============="
print outObjects
print outTags

from LArCalibTools.LArCalibToolsConf import LArBlockCorrections
topSequence+=LArBlockCorrections()

from LArCalibTools.LArCalibToolsConf import ForceLoadCondObj
topSequence+=ForceLoadCondObj(ObjectList=outObjects)

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
theOutputConditionsAlgRec=OutputConditionsAlg("OutputConditionsAlgRec",OutputFileRec,
                                              outObjects,outTags,True)


svcMgr.IOVDbSvc.dbConnection  = outputDB
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.RecreateFolders = True
if 'IOVBegin' in dir():
   svcMgr.IOVRegistrationSvc.BeginRun=IOVBegin

svcMgr.DetectorStore.Dump=True


if 'poolcat' in dir():
  svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:"+poolcat
