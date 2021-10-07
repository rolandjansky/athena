# Trying to run standalone only Noisy monitoring on ESD
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
if 'inputFiles' in dir():
    athenaCommonFlags.FilesInput.set_Value_and_Lock(inputFiles.split(','))
    del inputFiles

if 'outHistFile' not in dir():
    outHistFile="LArMon.root"

if 'maxEvents' not in dir():
    maxEvents = -1

if 'MNBLoose' not in dir():
   MNBLoose=5

if 'MNBTight' not in dir():
   MNBTight=17

from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
#globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.InputFormat.set_Value_and_Lock('pool')
globalflags.DataSource.set_Value_and_Lock('data')
#globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')
# Not sure that this is correct
# In LArCAF, BSRDOInput was used but here; ESDInput does not work...

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from RecExConfig.AutoConfiguration import GetProjectName,ConfigureGeo
rec.projectName=GetProjectName()

ConfigureGeo()

rec.readRDO.set_Value_and_Lock(False)
rec.readTAG.set_Value_and_Lock(False)
rec.readESD.set_Value_and_Lock(True)
rec.readAOD.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doHist.set_Value_and_Lock(False)
rec.doCBNT.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)

rec.doPerfMon.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteAOD=False

from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
DetFlags.Print()


#from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

svcMgr.GeoModelSvc.AtlasVersion=globalflags.DetDescrVersion()

from LArConditionsCommon.LArCondFlags import larCondFlags
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")
include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()

if 'conditionsTag' not in dir():
   print("Resolving 'CURRENT' express conditions tag ...")
   sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
   from AtlCoolBKLib import resolveAlias
   resolver=resolveAlias()
   currentGlobalES=resolver.getCurrentES().replace("*","ST")
   print("Found ",currentGlobalES)
   svcMgr.IOVDbSvc.GlobalTag=currentGlobalES
else:
   svcMgr.IOVDbSvc.GlobalTag=conditionsTag

from RecExConfig.ObjKeyStore import objKeyStore, CfgKeyStore

# First block the reading of LArNoisyROSummary from ESD
#objKeyStore.readInputFile('RecExPers/OKS_streamESD.py')
#objKeyStore['inputFile'].removeItem(["LArNoisyROSummary#LArNoisyROSummary"]) 

# unlock EventInfo
from RecAlgs.RecAlgsConf import EventInfoUnlocker
topSequence+=EventInfoUnlocker("UnlockEventInfo")

# then create a new LArNoisyROSummary


from LArCellRec.LArNoisyROFlags import larNoisyROFlags
from LArCellRec.LArCellRecConf import LArNoisyROAlg,LArNoisyROTool
theLArNoisyROTool=LArNoisyROTool(PrintSummary=True,
                                 CellQualityCut=larNoisyROFlags.CellQualityCut(),
                                 BadChanPerFEB=larNoisyROFlags.BadChanPerFEB(),
                                 BadFEBCut=larNoisyROFlags.BadFEBCut(),
                                 MNBLooseCut=MNBLoose, MNBTightCut=MNBTight
                                 )
theLArNoisyROAlg=LArNoisyROAlg()
theLArNoisyROAlg.OutputKey="LArNoisyROSummary_reco"
theLArNoisyROAlg.Tool=theLArNoisyROTool
theLArNoisyROAlg.writeEventInfo=True

#theLArNoisyROAlg.BadFEBCut = 2000
#theLArNoisyROAlg.SaturatedCellQualityCut = 111165535
#from RecExConfig.ObjKeyStore import objKeyStore
#objKeyStore.addStreamESD("LArNoisyROSummary","LArNoisyROSummary")
topSequence += theLArNoisyROAlg

# and then run monitoring on it
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()

svcMgr.THistSvc.Output += ["LArMon DATAFILE='"+outHistFile+"' OPT='RECREATE'"]

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
larRawMon=False
larESDMon=True
from AthenaMonitoring.DQMonFlags import DQMonFlags
from LArMonTools.LArMonFlags import LArMonFlags
LArMonFlags.doLArDigitMon = False
LArMonFlags.EventBlockSize = 0

LArMon = AthenaMonManager(name="LArMonManager",
                          FileKey             = "LArMon",
                          Environment         = DQMonFlags.monManEnvironment(),
                          ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                          DataType            = DQMonFlags.monManDataType())
topSequence += LArMon

from LArMonTools.LArMonToolsConf import LArNoisyROMon

# variable for testing on ESD
try:
    LArNoisyROMonForceTrigger
except NameError:
    LArNoisyROMonForceTrigger = False

from LArCellRec.LArNoisyROFlags import larNoisyROFlags

theLArNoisyROMon = LArNoisyROMon(name="LArNoisyROMon")
theLArNoisyROMon.IsOnline = False
theLArNoisyROMon.inputKey = "LArNoisyROSummary_reco"
theLArNoisyROMon.ProcessNEvents= LArMonFlags.EventBlockSize()
theLArNoisyROMon.NoisyFEBDefStr =  '(>'+str(larNoisyROFlags.BadChanPerFEB())+' chan with Q>'+str(larNoisyROFlags.CellQualityCut())+')' #LArNoisyROCutHelper('BadChanPerFEB')
theLArNoisyROMon.BadFEBCut = larNoisyROFlags.BadFEBCut() #LArNoisyROFEBCutHelper()
theLArNoisyROMon.doTrigger = False
theLArNoisyROMon.doHisto = False

#ToolSvc += theLArNoisyROMon
LArMon.AthenaMonTools += [theLArNoisyROMon]

#########
from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
svcMgr += AthenaEventLoopMgr()
svcMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100

theApp.EvtMax=maxEvents

svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.defaultLimit = 9999999  # all messages
svcMgr.MessageSvc.useColors = False
svcMgr.MessageSvc.defaultLimit=1000000
#svcMgr.MessageSvc.OutputLevel=DEBUG

