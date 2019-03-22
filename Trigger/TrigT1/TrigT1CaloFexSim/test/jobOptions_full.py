theApp.EvtMax=1                                      #says how many events to run over. Set to -1 for all events

debug = False
# debug = True

#Input file
from PyUtils import AthFile
import AthenaPoolCnvSvc.ReadAthenaPool                 #sets up reading of POOL files (e.g. xAODs)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# need both of these set for some reason, just one won't work
svcMgr.EventSelector.InputCollections=["/eos/atlas/user/b/bcarlson/mc16/mc16_13TeV.361020.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ0W.recon.AOD.e3569_s3126_r10684/AOD.15453868._000192.pool.root.1"]
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections 

from AthenaCommon.AlgSequence import AlgSequence
topSequence=AlgSequence()


#Some database settings, needed for ESD  

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff() 
DetFlags.detdescr.Calo_setOn()
include("RecExCond/AllDet_detDescr.py")

#What does this do 
include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
#What does this do 
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
if debug:
    svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
else:
    svcMgr.IOVRegistrationSvc.OutputLevel = INFO
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder = False
svcMgr.IOVRegistrationSvc.userTags = False

#These lines are for configuring the bunch crossing tool, useful for some cases

af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
isMC = 'IS_SIMULATION' in af.fileinfos['evt_type']

from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
if isMC: ToolSvc += BunchCrossingTool( "MC" )
else: ToolSvc += BunchCrossingTool( "LHC" )

#Create an output root file 

svcMgr.MessageSvc.defaultLimit = 999
svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["OUTPUT DATAFILE='myOutputFile.root' OPT='RECREATE'"]

#Output AOD 

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
StreamAOD_Augmented = MSMgr.NewPoolRootStream( "StreamXAOD", "xAOD.out.root" )


#Enable jFEX and gFEX algorithms 

include("TrigT1CaloFexSim/createL1SimulationSequence.py")

StreamAOD_Augmented.AddItem ( ["xAOD::JetRoIContainer#*","xAOD::JetRoIAuxContainer#*","xAOD::EnergySumRoI#*","xAOD::EnergySumRoIAuxInfo#*"])
