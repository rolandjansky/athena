#Skeleton joboption for a simple analysis job

theApp.EvtMax=10                                      #says how many events to run over. Set to -1 for all events

debug = False
# debug = True

#Input file
from PyUtils import AthFile
import AthenaPoolCnvSvc.ReadAthenaPool                 #sets up reading of POOL files (e.g. xAODs)
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# need both of these set for some reason, just one won't work
svcMgr.EventSelector.InputCollections=["/eos/user/c/ckaldero/JetTrigger_shared/AOD/L1Calo/mc16_13TeV.308276.PowhegPy8EG_NNPDF30_AZNLOCTEQ6L1_VBFH125_ZZ4nu_MET125.recon.AOD.e6126_e5984_s3126_r10684_tid14732657_00/AOD.14732657._000011.pool.root.1"]
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections 

#Output AOD 
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamXAOD", "xAOD.out.root" )
# uncommenting the lines below gives "StreamXAOD          ERROR  INCORRECT Object/count: xAOD::EnergySumRoIAuxInfo_jFexMETAux., 1 should be: 2"
# but the JFex jet and MET containers are not written to the output xAOD...
xaodStream.AddItem( ["xAOD::JetRoIContainer#*"] )
xaodStream.AddItem( ["xAOD::JetRoIAuxContainer#*"] )
xaodStream.AddItem( ["xAOD::EnergySumRoI#*"] )
xaodStream.AddItem( ["xAOD::EnergySumRoIAuxInfo#*"] )
xaodStream.Stream.TakeItemsFromInput = True



from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff() 
DetFlags.detdescr.Calo_setOn()
include("RecExCond/AllDet_detDescr.py")

include( "LArConditionsCommon/LArIdMap_MC_jobOptions.py" )
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
if debug:
    svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
else:
    svcMgr.IOVRegistrationSvc.OutputLevel = INFO
svcMgr.IOVRegistrationSvc.RecreateFolders = True
svcMgr.IOVRegistrationSvc.SVFolder = False
svcMgr.IOVRegistrationSvc.userTags = False


#svcMgr.IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;ATLAS_COOLPROD:OFLP130:ATLAS_COOL_LAR_W:"
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=gJTowerMap.db;dbname=OFLP200"


af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
isMC = 'IS_SIMULATION' in af.fileinfos['evt_type']

from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
if isMC: ToolSvc += BunchCrossingTool( "MC" )
else: ToolSvc += BunchCrossingTool( "LHC" )



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

algseq = CfgMgr.AthSequencer("AthAlgSeq")        # gets the main AthSequencer

algseq += CfgMgr.JGTowerReader(                  # adds an instance of your alg to it
    outputNoise = False,
    debugJetAlg = debug,
    noise_file = "noise_r9813.root", # "/eos/user/c/cylin/public/L1Calo/noise_r10684.root", - this is not generally accessible (I can see it but not open it)
    makeSquareJets = True,
    jSeed_size = 0.2,
    jJet_r = 0.4,
    jMax_r = 0.4,
    jJet_threshold = 3, 
    makeRoundJets = True,
    jJetSeed_size=0.2, # wants to be 0.3 I think
    jJet_max_r=0.4,     # not really sure, maybe 0.6
    jJet_jet_r=0.4,
    jJet_jet_threshold = 3, 
    plotSeeds = True,
    gJet_threshold = 3,
    gSeed_size=0.4,
    gJet_r=1.0
    )

if debug:
    algseq.JGTowerReader.OutputLevel=DEBUG
else:
    algseq.JGTowerReader.OutputLevel=INFO

include("AthAnalysisBaseComps/SuppressLogging.py")       #Optional include to suppress as much athena output as possible
