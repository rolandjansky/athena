#Skeleton joboption for a simple analysis job



include("runs.py")

# if athenaCommonFlags.FilesInput()==[]:
# FilesInput=[
#  "root://eosatlas//eos/atlas/atlastier0/rucio/data16_13TeV/physics_Main/00304128/data16_13TeV.00304128.physics_Main.merge.AOD.f716_m1620/data16_13TeV.00304128.physics_Main.merge.AOD.f716_m1620._lb1996._0001.1"
#  ]
  
FilesInput = files


theApp.EvtMax=100000                                       #says how many events to run over. Set to -1 for all events



import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of POOL files (e.g. xAODs)
# svcMgr.EventSelector.InputCollections=[os.environ['ASG_TEST_FILE_DATA']] #replace with input file
svcMgr.EventSelector.InputCollections=FilesInput

algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
# algseq += CfgMgr.WillAlg()                                 #adds an instance of your alg to it

#only specifying here so that has the standard 'TrigDecisionTool' name
ToolSvc += CfgMgr.Trig__TrigDecisionTool("TrigDecisionTool")

from TrigEDMConfig.TriggerEDM import EDMLibraries
ToolSvc.TrigDecisionTool.Navigation.Dlls = EDMLibraries

# IOVDbSvc.DBInstance=""


##### Histogram File Part #################
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
algseq += CfgMgr.AthenaMonManager( "HLTMonManager")
HLTMonManager = algseq.HLTMonManager


############ TrigIDtrkMonitoring part ################################

from AthenaCommon.AppMgr import ToolSvc

if ( False ) : 
  from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestBase

  tidabjet = TrigTestBase(name = "IDBjetTool",
                        histoPathBase = "/Trigger/HLT")
  tidabjet.AnalysisConfig = "Tier0"
  tidabjet.SliceTag = "HLT/TRIDT/Bjet/Expert"
  tidabjet.ntupleChainNames += [
    "Offline",
    "HLT_j.*bperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi"
  ]

  ToolSvc += tidabjet;

  HLTMonManager.AthenaMonTools += [ "TrigTestBase/IDBjetTool" ]

  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()
  ServiceMgr.THistSvc.Output = ["AANT DATAFILE='TIDA_T0.root' OPT='RECREATE'"]
  HLTMonManager.FileKey = "AANT"



if ( True ) :

  from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDtrkMonitoringTool

  montools = TrigIDtrkMonitoringTool()

  print montools

  HLTMonManager.AthenaMonTools += montools


  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()
  ServiceMgr.THistSvc.Output = ["AANT DATAFILE='TIDA_T0.root' OPT='RECREATE'"]
  HLTMonManager.FileKey = "AANT"




############ TrigInDetAnalysis part ################################

if ( False ) : 
  from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestMonToolAC
  TestMonTool = TrigTestMonToolAC( name="TestMonToolAC")
  TestMonTool.buildNtuple = True
  TestMonTool.AnalysisConfig = "nTuple" #Change to Tier0 for T0 Analysis
  TestMonTool.EnableLumi = False
  # TestMonTool.RequireDecision = False
  TestMonTool.RequireDecision = True
  TestMonTool.mcTruth = False
  TestMonTool.ntupleChainNames = ['']

  TestMonTool.KeepAllEvents = False
  # TestMonTool.TrigConfigTool = "TrigConf::xAODConfigTool"
  TestMonTool.ntupleChainNames += [
    "Offline",
    "Vertex",
    "Muons",
  
    # "HLT_j.*bperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx", 
    # "HLT_j.*bperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi",
    # "HLT_j.*bperf_split:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig",
    # "HLT_j.*bperf_split:key=InDetTrigTrackingxAODCnv_Bjet_FTF",
    
    "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx", 
    "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx", 
    # "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx", 
    # "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx", 
    # "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi",
#    "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig",
    "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_Bjet_FTF"  
    
    #EFHistoPrmVtx
    # "Electrons",                                                                                                                                    
                           
    # "Electrons_MediumCB",
    # "Electrons_TightCB",
    # "Electrons_MediumLH",
    # "Electrons_TightLH",
    # "Taus",
    
    # "Taus_1Prong",
    # "Taus_Loose_1Prong",
    # "Taus_Medium_1Prong",
    # "Taus_Tight_1Prong",
    # "Taus_3Prong",
    # "Taus_Loose_3Prong",
    # "Taus_Medium_3Prong",
    # "Taus_Tight_3Prong",
    
    # "HLT_e.*idperf.*:InDetTrigTrackingxAODCnv_Electron_FTF",
    # "HLT_e.*idperf.*:InDetTrigTrackingxAODCnv_Electron_IDTrig",
    "HLT_mu.*_idperf.*:InDetTrigTrackingxAODCnv_Muon_FTF",
    "HLT_mu.*_idperf.*:InDetTrigTrackingxAODCnv_Muon_IDTrig",
    "HLT_mu.*_idperf.*:InDetTrigTrackingxAODCnv_Muon_FTF;DTE",
#    "HLT_mu.*_idperf.*:InDetTrigTrackingxAODCnv_Muon_IDTrig;DTE",
    # "HLT_mu.*idperf.*:InDetTrigTrackingxAODCnv_Muon_IDTrig",
    # "HLT_tau.*idperf.*track:key=InDetTrigTrackingxAODCnv_Tau_IDTrig",
    # "HLT_tau.*idperf.*track:key=InDetTrigTrackingxAODCnv_Tau_FTF",
    # "HLT_tau.*idperf.*tracktwo:key=InDetTrigTrackingxAODCnv_TauCore_FTF:roi=forID1",
    # "HLT_tau.*idperf.*tracktwo:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3",
    # "HLT_tau.*idperf.*tracktwo:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3",
    
    # "HLT_tau.*idperf.*tracktwo:key=InDetTrigTrackingxAODCnv_TauCore_FTF:roi=forID1",
    # "HLT_tau.*idperf.*tracktwo:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3",
    # "HLT_tau.*idperf.*tracktwo:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3",
    ]
  
  from AthenaCommon.AppMgr import release_metadata
  d = release_metadata()
  TestMonTool.releaseMetaData = d['nightly name'] + " " + d['nightly release'] + " " + d['date'] + " " + d['platform'] + " " + d['release']
  TestMonTool.outputFileName="TrkNtuple.root"
  ToolSvc += TestMonTool
  HLTMonManager.AthenaMonTools += [ "TrigTestMonToolAC/TestMonToolAC" ]
  print TestMonTool




# include("AthAnalysisBaseComps/SuppressLogging.py")              #Optional include to suppress as much athena output as possible. Keep at bottom of joboptions so that it doesn't suppress the logging of the things you have configured above

