#Skeleton joboption for a simple analysis job


FilesInput = [ "AOD.pool.root" ]


theApp.EvtMax=-1                                       #says how many events to run over. Set to -1 for all events

from AthenaConfiguration.AllConfigFlags import ConfigFlags

import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of POOL files (e.g. xAODs)
# svcMgr.EventSelector.InputCollections=[os.environ['ASG_TEST_FILE_DATA']] #replace with input file
svcMgr.EventSelector.InputCollections=FilesInput

algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
# algseq += CfgMgr.WillAlg()                                 #adds an instance of your alg to it

#only specifying here so that has the standard 'TrigDecisionTool' name

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += CfgMgr.Trig__TrigDecisionTool("TrigDecisionTool")

from TrigEDMConfig.TriggerEDM import EDMLibraries
ToolSvc.TrigDecisionTool.Navigation.Dlls = EDMLibraries

from AthenaCommon.AppMgr import topSequence


from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigTool

cfgtool = TrigConf__xAODConfigTool('xAODConfigTool')
ToolSvc += cfgtool

tdt = Trig__TrigDecisionTool('TrigDecisionTool')
tdt.ConfigTool = cfgtool
tdt.NavigationFormat = "TrigComposite"

tdt.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]


jps.AthenaCommonFlags.FilesInput = FilesInput


##### Histogram File Part #################
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

HLTMonManager = CfgMgr.AthenaMonManager( "HLTMonManager") 
algseq += HLTMonManager
# HLTMonManager = algseq.HLTMonManager



############ TrigIDtrkMonitoring part ################################

from AthenaCommon.AppMgr import ToolSvc


from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestBase


if ( True ) :

  from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDtrkMonitoringTool

  montools = TrigIDtrkMonitoringTool()

  #  print "\n\nMonTools\n" 
  #  print montools

  HLTMonManager.AthenaMonTools += montools

  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()
  ServiceMgr.THistSvc.Output = ["AANT DATAFILE='TIDA_T0.root' OPT='RECREATE'"]
  HLTMonManager.FileKey = "AANT"
  




############ TrigInDetAnalysis part ################################

if ( True ) : 
  from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestMonToolAC
  TestMonTool = TrigTestMonToolAC( name="TestMonToolAC")
  TestMonTool.buildNtuple = True
  TestMonTool.AnalysisConfig = "nTuple" #Change to Tier0 for T0 Analysis
  TestMonTool.EnableLumi = False
  TestMonTool.RequireDecision = False
  # TestMonTool.RequireDecision = True
  TestMonTool.mcTruth = True
  TestMonTool.ntupleChainNames = ['']

  TestMonTool.KeepAllEvents = False
  # TestMonTool.TrigConfigTool = "TrigConf::xAODConfigTool"
  TestMonTool.ntupleChainNames += [
    "Offline",
    "Truth",
    "Vertex",
    # "Muons",
    # "Muons:Tight",
    # "Muons:Medium",
  
    # "HLT_j.*bperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx", 
    # "HLT_j.*bperf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi",
    # "HLT_j.*bperf_split:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig",
    # "HLT_j.*bperf_split:key=InDetTrigTrackingxAODCnv_Bjet_FTF",
    
    #    "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx", 
    #    "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx", 
    # "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=xPrimVx", 
    # "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi:vtx=EFHistoPrmVtx", 
    # "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF:roi=SuperRoi",
    #    "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_Bjet_IDTrig",
    #    "HLT_j.*b.*perf_split:key=InDetTrigTrackingxAODCnv_Bjet_FTF",  
    
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
    # "HLT_mu.*_idperf.*:InDetTrigTrackingxAODCnv_Muon_IDTrig;DTE",
    # "HLT_mu.*idperf.*:InDetTrigTrackingxAODCnv_Muon_IDTrig",
    # "HLT_tau.*idperf.*track:key=InDetTrigTrackingxAODCnv_Tau_IDTrig",
    # "HLT_tau.*idperf.*track:key=InDetTrigTrackingxAODCnv_Tau_FTF",
    # "HLT_tau.*idperf.*tracktwo:key=InDetTrigTrackingxAODCnv_TauCore_FTF:roi=forID1",
    # "HLT_tau.*idperf.*tracktwo:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3",
    # "HLT_tau.*idperf.*tracktwo:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3",
    
    # "HLT_tau.*idperf.*tracktwo:key=InDetTrigTrackingxAODCnv_TauCore_FTF:roi=forID1",
    # "HLT_tau.*idperf.*tracktwo:key=InDetTrigTrackingxAODCnv_TauIso_FTF:roi=forID3",
    # "HLT_tau.*idperf.*tracktwo:key=InDetTrigTrackingxAODCnv_Tau_IDTrig:roi=forID3",


    #    ":HLT_IDTrack_FS_FTF",
    #    ":HLT_IDTrack_FS_FTF:roi=HLT_FSRoI:vtx=HLT_IDVertex_FS",

    "HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20:HLT_IDTrack_FS_FTF:roi=HLT_FSRoI:vtx=HLT_IDVertex_FS",
    "HLT_j45_ftf_L1J15:key=HLT_IDTrack_FS_FTF:roi=HLT_FSRoI:vtx=HLT_IDVertex_FS",

    "HLT_mu.*_idperf.*:HLT_IDTrack_Muon_FTF",
    "HLT_mu.*_idperf.*:HLT_IDTrack_Muon_FTF:roi=HLT_Roi_L2SAMuon",
    "HLT_mu.*_idperf.*:HLT_IDTrack_Muon_IDTrig",
    "HLT_mu.*_idperf.*:HLT_IDTrack_Muon_IDTrig:roi=HLT_Roi_L2SAMuonForEF",
    "HLT_mu.*i.*:HLT_IDTrack_MuonIso_FTF:roi=HLT_Roi_MuonIso",
    "HLT_mu.*i.*:HLT_IDTrack_MuonIso_IDTrig:roi=HLT_Roi_MuonIso",

    "HLT_b.*perf.*:HLT_IDTrack_Bjet_FTF",
    "HLT_b.*perf.*:HLT_IDTrack_Bjet_IDTrig",
    "HLT_j.*perf.*:HLT_IDTrack_Bjet_FTF",
    "HLT_j.*perf.*:HLT_IDTrack_Bjet_IDTrig",

#    "HLT_e.*_etcut.*:HLT_IDTrack_Electron_FTF",
#    "HLT_e.*_etcut.*:HLT_IDTrack_Electron_IDTrig",
    "HLT_e.*:HLT_IDTrack_Electron_FTF",
    "HLT_e.*:HLT_IDTrack_Electron_IDTrig",

    "HLT_tau.*_idperf.*:HLT_IDTrack_Tau_FTF",
    "HLT_tau.*_idperf.*:HLT_IDTrack_Tau_FTF:roi=HLT_TAURoI"
    "HLT_tau.*_idperf.*:HLT_IDTrack_TauCore_FTF",
    "HLT_tau.*_idperf.*:HLT_IDTrack_TauIso_FTF",
    "HLT_tau.*_idperf.*:HLT_IDTrack_TauCore_FTF:roi=HLT_Roi_TauCore",
    "HLT_tau.*_idperf.*:HLT_IDTrack_TauIso_FTF:roi=HLT_Roi_TauIso",
    "HLT_tau.*_idperf.*:HLT_IDTrack_Tau_IDTrig",
    "HLT_tau.*_idperf.*:HLT_IDTrack_Tau_IDTrig:roi=HLT_TAURoI",
    "HLT_tau.*_idperf.*:HLT_IDTrack_Tau_IDTrig:roi=HLT_Roi_TauIso",

    "HLT_mb.*:HLT_IDTrack_MinBias_FTF",
    

    ]
  
  from AthenaCommon.AppMgr import release_metadata
  d = release_metadata()
  TestMonTool.releaseMetaData = d['nightly name'] + " " + d['nightly release'] + " " + d['date'] + " " + d['platform'] + " " + d['release']
  TestMonTool.outputFileName="TrkNtuple.root"
  HLTMonManager.AthenaMonTools += [ TestMonTool ]
  print (TestMonTool)


print ("configured everything")


# include("AthAnalysisBaseComps/SuppressLogging.py")              #Optional include to suppress as much athena output as possible. Keep at bottom of joboptions so that it doesn't suppress the logging of the things you have configured above


MessageSvc.infoLimit  = 100000000
