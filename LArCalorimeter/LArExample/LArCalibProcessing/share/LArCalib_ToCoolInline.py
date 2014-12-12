###########################################################################
#
# TopOptions to copy a complete set of conditions for a given run to 
# one POOL file and one sqlite file
#
# Walter.Lampl@cern.ch
#
###########################################################################


import os
#from LArCalibProcessing.extractFolderInfo import *

if 'dbname' not in dir():
  dbname="CONDBR2"

if 'sqliteIn' not in dir():
  sqliteIn="freshConstants.db"

if 'sqliteOut' not in dir():
  sqliteOut=sqliteIn+"Inline.out"

if "globalTag" not in dir():
  globalTag="CONDBR2-BLKPA-2014-02"

if "inputFolders" not in dir():
  inputFolders=[("/LAR/ElecCalibOnl/Pedestal",""),
                ("/LAR/ElecCalibOnl/Ramp",""),
                ("/LAR/ElecCalibOnl/MphysOverMcal",""),
                ("/LAR/ElecCalibOnl/OFC",""),
                ("/LAR/ElecCalibOnl/Shape","")
                ]





inputDB="sqlite://;schema="+sqliteIn+";dbname="+dbname
outputDB="sqlite://;schema="+sqliteOut+";dbname="+dbname

#folderInfo=extractFolderInfo(inputDB)

if os.access(sqliteOut,os.F_OK):
  print "File",sqliteOut,"exists already, removing ...."
  os.remove(sqliteOut)


import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector

from AthenaCommon.GlobalFlags import  globalflags
globalflags.DataSource="data"
globalflags.InputFormat="bytestream"
globalflags.DatabaseInstance="CONDBR2"

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

include("LArCalibProcessing/LArCalib_MinimalSetup.py")

theApp.EvtMax = 1
conddb.setGlobalTag(globalTag) #For id mapping

svcMgr.PoolSvc.CheckDictionary=False
svcMgr.PoolSvc.SortReplicas=False

svcMgr.EventSelector.RunNumber = 2147483647

for (fldr,tag) in inputFolders:
  if len(tag):
     conddb.addFolder("","<db>"+inputDB+"</db>"+fldr+"<tag>"+tag+"</tag>")
  else:
    conddb.addFolder("","<db>"+inputDB+"</db>"+fldr)
  pass




from LArCalibTools.LArCalibToolsConf import LArCompleteToFlat
theLArCompleteToFlat=LArCompleteToFlat()
theLArCompleteToFlat.PedestalInput="Pedestal"
theLArCompleteToFlat.RampInput="LArRamp"
theLArCompleteToFlat.MphysOverMcalInput="LArMphysOverMcal"
theLArCompleteToFlat.OFCInput="LArOFC"
theLArCompleteToFlat.ShapeInput="LArShape"
theLArCompleteToFlat.FakeEMBPSLowGain=True
topSequence+=theLArCompleteToFlat



outObjects=["CondAttrListCollection#/LAR/ElecCalibFlat/Pedestal",
            "CondAttrListCollection#/LAR/ElecCalibFlat/OFC",
            "CondAttrListCollection#/LAR/ElecCalibFlat/Shape",
            "CondAttrListCollection#/LAR/ElecCalibFlat/Ramp",
            "CondAttrListCollection#/LAR/ElecCalibFlat/MphysOverMcal"
            ]
outTags=[]

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
theOutputConditionsAlgRec=OutputConditionsAlg("OutputConditionsAlgInline","dummy.root",
                                              outObjects,outTags,True)


svcMgr.IOVDbSvc.dbConnection  = outputDB
svcMgr.IOVDbSvc.DBInstance=""

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder=True
svcMgr.IOVRegistrationSvc.OverrideNames += [ "Pedestal","PedestalRMS","OFCa"   ,"OFCb"    ,"TimeOffset","RampVec","Shape","ShapeDer","HVScaleCorr","MphysOverMcal","DAC2uA","uA2MeV"]
types=[]
for i in range(len(svcMgr.IOVRegistrationSvc.OverrideNames)):
    types.append("Blob16M");
svcMgr.IOVRegistrationSvc.OverrideTypes += types;

svcMgr.DetectorStore.Dump=True


if 'poolcat' in dir():
  svcMgr.PoolSvc.WriteCatalog="xmlcatalog_file:"+poolcat

svcMgr.MessageSvc.defaultLimit=9999999999
