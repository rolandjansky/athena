# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

###########################################################################
#
# TopOptions to copy a complete set of conditions for a given run to 
# one POOL file and one sqlite file
#
# Walter.Lampl@cern.ch
#
###########################################################################


import os

if not "SuperCells" in dir():
   SuperCells=False



if not SuperCells: 
  include("LArCalibProcessing/LArCalib_Flags.py")
else:
  include("LArCalibProcessing/LArCalib_FlagsSC.py")

if 'dbname' not in dir():
  dbname="CONDBR2"
inputDB="sqlite://;schema="+sqliteIn+";dbname="+dbname
outputDB="sqlite://;schema="+sqliteOut+";dbname="+dbname

if 'foldertag' not in dir():
   if SuperCells:
      foldertag = '-UPD3-01'
   else:
      foldertag = '-UPD3-00'

if 'sqliteIn' not in dir():
  sqliteIn="freshConstants.db"

if 'sqliteOut' not in dir():
  sqliteOut=sqliteIn+"Inline.out"

if "globalTag" not in dir():
  globalTag = "CONDBR2-BLKPA-RUN2-03"


if "inputFolders" not in dir():
  inputFolders=[ (LArCalib_Flags.LArPedestalFolder, LArCalibFolderTag(LArCalib_Flags.LArPedestalFolder, foldertag), "LArPedestal"),
                 (LArCalib_Flags.LArRampFolder, LArCalibFolderTag(LArCalib_Flags.LArRampFolder, foldertag), "LArRamp"),
                 #(LArCalib_Flags.LArMphysOverMcalFolder, LArCalibFolderTag(LArCalib_Flags.LArMphysOverMcalFolder, foldertag.replace("-01","-00")), "LArMphysOverMcal"),
                 (LArCalib_Flags.LArOFCCaliFolder, LArCalibFolderTag(LArCalib_Flags.LArOFCCaliFolder, foldertag), "LArOFCCali"),
                 #(LArCalib_Flags.LArShapeRTMSamplesFolder, LArCalibFolderTag(LArCalib_Flags.LArShapeRTMSamplesFolder, foldertag.replace("-01","-00")), "LArShape"),
               ]


if not 'online' in dir():
   online = True #False

if os.access(sqliteOut,os.F_OK):
  printfunc ("File",sqliteOut,"exists already, removing ....")
  os.remove(sqliteOut)


import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector

from AthenaCommon.GlobalFlags import  globalflags
globalflags.DataSource="data"
globalflags.InputFormat="bytestream"
globalflags.DatabaseInstance=dbname

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

include ("LArConditionsCommon/LArMinimalSetup.py")
from LArCabling.LArCablingAccess import LArOnOffIdMapping
LArOnOffIdMapping()
if SuperCells:
  from LArCabling.LArCablingAccess import LArOnOffIdMappingSC,LArCalibIdMappingSC
  LArOnOffIdMappingSC()


theApp.EvtMax = 1
conddb.setGlobalTag(globalTag) #For id mapping

svcMgr.PoolSvc.SortReplicas=False

svcMgr.EventSelector.RunNumber = 2147483647

from LArCalibTools.LArCalibToolsConf import LArCompleteToFlat
theLArCompleteToFlat=LArCompleteToFlat()
theLArCompleteToFlat.FakeEMBPSLowGain=True
theLArCompleteToFlat.isSC = SuperCells

outTypes = []

for (fldr,tag,key,classtype) in inputFolders:
  if "Pedestal" in fldr:
    outTypes.append("Pedestal")
    theLArCompleteToFlat.PedestalInput=key
  elif "Ramp" in fldr:
    outTypes.append("Ramp")
    theLArCompleteToFlat.RampInput=key
  elif "OFC" in fldr:
    outTypes.append("OFC")
    theLArCompleteToFlat.OFCInput=key
  elif "MphysOverMcal" in fldr:
    outTypes.append("MphysOverMcal")
    theLArCompleteToFlat.MphysOverMcalInput=key
  elif "Shape" in fldr:
    outTypes.append("Shape")
    theLArCompleteToFlat.ShapeInput=key
  if len(tag):
     conddb.addFolder("","<db>"+inputDB+"</db>"+fldr+"<tag>"+tag+"</tag>",className=classtype)
  else:
    conddb.addFolder("","<db>"+inputDB+"</db>"+fldr,className=classtype)
  pass

topSequence+=theLArCompleteToFlat

flatName="ElecCalibFlat"
if SuperCells:
   flatName+="SC"
if "outObjects" not in dir():
  outObjects=["CondAttrListCollection#/LAR/"+flatName+"/"+ot for ot in outTypes]

outTags=[]

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
theOutputConditionsAlgRec=OutputConditionsAlg("OutputConditionsAlgInline","dummy.root", outObjects,outTags,True)


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

svcMgr.MessageSvc.defaultLimit=99999999
