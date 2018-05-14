#Skeleton joboption for a simple analysis job

theApp.EvtMax=-1                                      #says how many events to run over. Set to -1 for all events

from AthenaCommon.BeamFlags import jobproperties;
mu=200
jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(mu);
from LArROD.LArRODFlags import larRODFlags; 
larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True); 
larRODFlags.NumberOfCollisions.set_Value_and_Lock(mu); 
from AthenaCommon.BeamFlags import jobproperties; 
jobproperties.Beam.bunchSpacing=25; 

#Tag Override may not be needed for newer releases(>=20.20.7.4)
tagToOverride="mV-LSB";

from IOVDbSvc.CondDB import conddb
conddb.addOverride("/LAR/ElecCalibMCSC/Noise","LARElecCalibMCSCNoise-"+tagToOverride);
conddb.addOverride("/LAR/ElecCalibMCSC/Ramp", "LARElecCalibMCSRamp-"+tagToOverride)

#Input file
from PyUtils import AthFile
import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of POOL files (e.g. xAODs)
svcMgr.EventSelector.InputCollections=[""]               #left blank for no proper sample
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff() 
DetFlags.detdescr.Calo_setOn()
include("RecExCond/AllDet_detDescr.py")

include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder = False
svcMgr.IOVRegistrationSvc.userTags = False


#svcMgr.IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;ATLAS_COOLPROD:OFLP130:ATLAS_COOL_LAR_W:"
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=gJTowerMap.db;dbname=OFLP200"




folderlist=("/LAR/Identifier/GTowerSCMap",
            "/LAR/Identifier/JTowerSCMap"
            )

folderspec=[]
tagspec=[]
for f in folderlist:
    folderspec.append("GTowerSCMap#GTowerSCMapAtlas#"+f)
    tagspec.append("".join(f.split("/"))+"-boohoo")
    pass



svcMgr.MessageSvc.defaultLimit = 999
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["OUTPUT DATAFILE='myOutputFile.root' OPT='RECREATE'"]

athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections
algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
algseq += CfgMgr.JGTowerReader(
outputNoise=True,
noise_file="../../Trigger/TrigT1/TrigT1CaloFexSim/data/noise_r9813.root",
jSeed_size=0.2,
jJet_r=0.4,
gSeed_size=0.4,
gJet_r=1.0,
seed_thr=1.5,
seed_tower_thr=1.0
)                                 #adds an instance of your alg to it
algseq.JGTowerReader.OutputLevel=DEBUG

include("AthAnalysisBaseComps/SuppressLogging.py")       #Optional include to suppress as much athena output as possible
