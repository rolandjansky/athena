fill=True

import AthenaCommon.AtlasUnixGeneratorJob
## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')
#globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-18-01-01')
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-20-00-01')
globalflags.DetGeo.set_Value_and_Lock('commis')
globalflags.Luminosity.set_Value_and_Lock('zero')

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

svcMgr.IOVDbSvc.GlobalTag = "COMCOND-ES1PA-006-02"

#Get identifier mapping (needed by LArConditionsContainer)
include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

# for LArBadChannelTool, instead of conddb.AddFolder below
#include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py") 

theApp.EvtMax = 1
#conddb.setGlobalTag("COMCOND-006-01") #For id mapping

folder="/LAR/Configuration/DSPGeoWeightsFlat"

fileName="DSPGeoWeigths"

from CaloTools.CaloNoiseFlags import jobproperties
jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.45*30/8)

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArBadChannelMasker=LArBadChannelMasker("LArBadChannelMasker")
theLArBadChannelMasker.DoMasking=True
theLArBadChannelMasker.ProblemsToMask=[
    "highNoiseHG","highNoiseMG","highNoiseLG"
    ]
##    "deadReadout","deadPhys","deadCalib","short","almostDead",
ToolSvc+=theLArBadChannelMasker

from LArOnlDbPrep.LArOnlDbPrepConf import LArGeoWeightsFill
theLArGeoWeightsFill=LArGeoWeightsFill()
theLArGeoWeightsFill.OutFile="dumpFile.txt"
theLArGeoWeightsFill.Key=folder

theLArGeoWeightsFill.Fill=True
theLArGeoWeightsFill.Dump=True

topSequence+=theLArGeoWeightsFill


OutputList=[ "AthenaAttributeList#"+folder ]
OutputTagList=[]


from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg","LArGeoWeightsTemplates.pool.root",
                                           OutputList,OutputTagList,True)
    
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.RecreateFolders = False
svcMgr.IOVRegistrationSvc.SVFolder=True
svcMgr.IOVRegistrationSvc.OverrideNames += ["costheta","sinthetacosphi","sinthetasinphi","offlineTTid"]
svcMgr.IOVRegistrationSvc.OverrideTypes += ["Blob16M","Blob16M","Blob16M","Blob16M"]
svcMgr.IOVRegistrationSvc.OutputLevel=VERBOSE
    
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=DSPGeoWeights.db;dbname=CONDBR2"

svcMgr.DetectorStore.Dump=True
svcMgr.EventSelector.RunNumber=0xFFFFFF
