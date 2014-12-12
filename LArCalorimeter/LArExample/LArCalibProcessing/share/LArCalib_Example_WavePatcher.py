#InputDB="oracle://ATONR_COOL;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2;;password=COOLRED4PRO"
InputDB="COOLOFL_LAR/CONDBR2"
#Folder="/LAR/ElecCalibOfl/CaliWaves/CaliWave"
FolderTag="LARElecCalibCaliWavesCaliWave-comm-08"
Folder="/LAR/ElecCalibOfl/CaliWaves/CaliWaveXtalkCorr"
FolderTag="LARElecCalibCaliWavesCaliWaveXtalkCorr-comm-08"

RunNumber=52000

import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  



from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-Comm-00-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit 

#Get identifier mapping (needed by LArConditionsContainer)
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

#input-data:
#conddb.addFolder("",Folder+"<key>LArCaliWaveInput</key><tag>"+FolderTag+"</tag>"+InputDB)
conddb.addFolder("",Folder+"<tag>"+FolderTag+"</tag><db>"+InputDB+"</db>")
#conddb.addFolder("",Folder+"<tag>"+FolderTag+"</tag>"+InputDB)
conddb.addFolder("LAR","/LAR/BadChannelsOfl/BadChannels")
svcMgr.IOVDbSvc.GlobalTag="COMCOND-004-00"
try:
   svcMgr.IOVDbSvc.DBInstance=""
except: 
   pass

PoolSvc.ReadCatalog+=["prfile:poolcond/PoolCat_comcond_castor.xml"]

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = RunNumber 




from LArCalibUtils.LArCalibUtilsConf import LArCalibPatchingAlg_LArCaliWaveContainer_
theLArCaliWavePatcher=LArCalibPatchingAlg_LArCaliWaveContainer_("LArCaliWavePatcher")
theLArCaliWavePatcher.ContainerKey="LArCaliWave"
theLArCaliWavePatcher.NewContainerKey="LArCaliWaveOut"
theLArCaliWavePatcher.PatchMethod="PhiNeighbor"
theLArCaliWavePatcher.OutputLevel=DEBUG
topSequence+=theLArCaliWavePatcher

from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
theLArBadChannelTool.OutputLevel=DEBUG
ToolSvc+=theLArBadChannelTool

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArRCBMasker=LArBadChannelMasker("LArRCBMasker")
theLArRCBMasker.DoMasking=True
theLArRCBMasker.ProblemsToMask=[
    "deadCalib","deadReadout","deadPhys","almostDead","short",
    ]
ToolSvc+=theLArRCBMasker
theLArCaliWavePatcher.MaskingTool=theLArRCBMasker



OutputObjectSpecCaliWave = "LArCaliWaveContainere#LArCaliWaveOut#"+Folder

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","LArCaliWaveCorrections_"+str(RunNumber)+".pool.root",
                                           [OutputObjectSpecCaliWave],[FolderTag],True)
theOutputConditionsAlg.Run1 = RunNumber
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=LArCaliWaveCorrections_"+str(RunNumber)+".db;dbname=CONDBR2"

from AthenaCommon.AppMgr import ServiceMgr
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True


svcMgr.DetectorStore.Dump=True


ToolSvc.LArBadChanTool.OutputLevel=DEBUG
svcMgr.IOVDbSvc.OutputLevel=DEBUG
