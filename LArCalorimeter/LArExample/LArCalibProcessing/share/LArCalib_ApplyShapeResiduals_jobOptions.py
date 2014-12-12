###########################################################################
#
# TopOptions to copy a complete set of conditions for a given run to 
# one POOL file and one sqlite file
#
# Walter.Lampl@cern.ch
#
###########################################################################


import os,sys
from LArCalibProcessing.extractFolderInfo import *

if 'dbname' not in dir():
  dbname="CONDBR2"

if 'sqliteIn' not in dir():
  sqliteIn="freshConstants.db"

if 'sqliteOut' not in dir():
  sqliteOut=sqliteIn+".out"

inputDB="sqlite://;schema="+sqliteIn+";dbname="+dbname
outputDB="sqlite://;schema="+sqliteOut+";dbname="+dbname


if os.environ.has_key("CALIBMERGE_PEEKRUN"):
  peekrun=int(os.environ["CALIBMERGE_PEEKRUN"])
else:
  peekrun=2147483647

print "Peeking input database at run",peekrun

if not "ShapeFolder" in dir():
  ShapeFolder="/LAR/ElecCalibOfl/Shape/RTM/5samples1phase"

if not 'ShapeResidualsFolder' in dir() :
  ShapeResidualsFolder  =  "/LAR/ElecCalibOfl/Shape/Residuals/5samples"


if 'ShapeResidualsTag' in dir() :
  shapeResidualTag="<tag>"+ShapeResidualsTag+"</tag>"
else:
  shapeResidualTag=""

folderInfo=extractFolderInfo(inputDB,checkFolders=[ShapeFolder,],runnumber=peekrun)
print folderInfo

import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

include("LArCalibProcessing/LArCalib_MinimalSetup.py")


theApp.EvtMax = 1
conddb.setGlobalTag("LARCALIB-000-02") #For id mapping and wave residuals

svcMgr.PoolSvc.CheckDictionary=True
svcMgr.PoolSvc.SortReplicas=False
svcMgr.PoolSvc.FileOpen="update"

runNoForFileName=2147483647

if len(folderInfo)==0:
  print "ERROR, No input shape found in input db",inputDB,"folder",ShapeFolder
  sys.exit(-1)
  pass

if len(folderInfo)>1:
  print "WARNING, More than one input shape object found in input db",inputDB,"folder",ShapeFolder
  pass

f=folderInfo[0]
fn=f[0]
type=f[1]
key=f[2]
tag=f[3]
since=f[4]
until=f[5]
print "Working on folder",fn,"IOV:",since,"-",until

tagTokens=tag.split('-')
tagTokens[0]+="corr"
outTag="-".join(tagTokens)

if since>0 and since<runNoForFileName:
  runNoForFileName=since
  pass

IOVBegin=since

outObjects=["#".join((type,key,fn)),]
outTags=[outTag,]
conddb.addFolder("",fn+"<tag>"+tag+"</tag><db>"+inputDB+"</db><key>"+key+"In</key>")

svcMgr.EventSelector.RunNumber = peekrun

if "OutputFile" not in dir():
  OutputFile="LArConditionsRec_"+str(runNoForFileName)+".pool.root"

print outObjects
print outTags

from LArCalibTools.LArCalibToolsConf import LArBlockCorrections
topSequence+=LArBlockCorrections()

from LArCalibUtils.LArCalibUtilsConf import LArShapeCorrector
theLArShapeCorrector=LArShapeCorrector(KeyShape=key+"In",KeyShape_newcorr=key,OutputLevel=DEBUG)
topSequence+=theLArShapeCorrector

conddb.addFolder("LAR_OFL",ShapeResidualsFolder+shapeResidualTag)


from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlgCorr",OutputFile,
                                           outObjects,outTags,True)

theOutputConditionsAlg.Run1 = IOVBegin

svcMgr.IOVDbSvc.dbConnection  = outputDB
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.RecreateFolders = False
svcMgr.IOVRegistrationSvc.OutputLevel=DEBUG
svcMgr.DetectorStore.Dump=True


if 'poolcat' in dir():
  svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:"+poolcat


svcMgr.IOVDbSvc.DBInstance=""
#svcMgr.IOVDbSvc.OutputLevel=DEBUG
from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += larCalibCatalogs
