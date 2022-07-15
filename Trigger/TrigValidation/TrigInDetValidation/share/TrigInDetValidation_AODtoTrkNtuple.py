#Skeleton joboption for a simple analysis job


FilesInput = [ "AOD.pool.root" ]
if len(jps.AthenaCommonFlags.FilesInput.get_Value())>0:
   FilesInput = jps.AthenaCommonFlags.FilesInput.get_Value()

theApp.EvtMax=-1                                       #says how many events to run over. Set to -1 for all events

from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.Input.Files = FilesInput

import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of POOL files (e.g. xAODs)
# svcMgr.EventSelector.InputCollections=[os.environ['ASG_TEST_FILE_DATA']] #replace with input file
svcMgr.EventSelector.InputCollections=FilesInput

algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer

#only specifying here so that has the standard 'TrigDecisionTool' name

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from TrigDecisionTool.TrigDecisionToolConfig import TrigDecisionToolCfg
CAtoGlobalWrapper(TrigDecisionToolCfg, ConfigFlags)

from AthenaCommon.AppMgr import topSequence

from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigTool

cfgtool = TrigConf__xAODConfigTool('xAODConfigTool')
ToolSvc += cfgtool


jps.AthenaCommonFlags.FilesInput = FilesInput


##### Histogram File Part #################
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

HLTMonManager = CfgMgr.AthenaMonManager( "HLTMonManager")
algseq += HLTMonManager
# HLTMonManager = algseq.HLTMonManager

doTier0Mon = False

if 'doTIDATier0' in locals():
  doTier0Mon = doTIDATier0
                
doNewTier0Mon = False

if 'doNewTIDATier0' in locals():
  doNewTier0Mon = doNewTIDATier0


############ TrigInDetMonitoring part ################################

from AthenaCommon.AppMgr import ToolSvc


from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestBase


if doTier0Mon and not doNewTier0Mon:

  from TrigIDtrkMonitoring.TrigIDtrkMonitoringConfig import TrigIDtrkMonitoringTool

  montools = TrigIDtrkMonitoringTool()

  HLTMonManager.AthenaMonTools += montools

  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()
  ServiceMgr.THistSvc.Output = ["AANT DATAFILE='data-hists-tier0.root' OPT='RECREATE'"]
  HLTMonManager.FileKey = "AANT"


if doNewTier0Mon :   

  # this is the new location ...
  from TrigInDetMonitoring.TIDAMonitoring import TIDAMonitoring
  for git in TIDAMonitoring( None, "idtrigger" ):
    algseq += git

  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()
  ServiceMgr.THistSvc.Output = ["EXPERT DATAFILE='data-hists-tier0.root' OPT='RECREATE'"]



############ TrigInDetAnalysis part ################################

if ( True ) :
  from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigR3Mon
  TestMonTool = TrigR3Mon( name="TrigR3Mon")
  TestMonTool.buildNtuple = True
  TestMonTool.AnalysisConfig = "nTuple" #Change to Tier0 for T0 Analysis
  TestMonTool.EnableLumi = False
# TestMonTool.RequireDecision = False
  TestMonTool.mcTruth = True
  TestMonTool.pTCutOffline = 1000
  TestMonTool.ntupleChainNames = []

  if ( 'LRT' in dir() ) :
    if LRT == True :
      TestMonTool.FiducialRadius = 500.

  if 'ptmin' in dir():
     TestMonTool.pTCutOffline = ptmin


  if 'pdgid' in dir():
     TestMonTool.SelectTruthPdgId=pdgid
 
  if 'parentpdgid' in dir():
     TestMonTool.SelectParentTruthPdgId=parentpdgid


  TestMonTool.KeepAllEvents = False
  # TestMonTool.TrigConfigTool = "TrigConf::xAODConfigTool"
  TestMonTool.ntupleChainNames += [
    "Offline",
    "Truth",
    "Vertex",
    "Vertex:BTagging_AntiKt4EMPFlowSecVtx",
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

    "Electrons",

    "Electrons:MediumCB",
    "Electrons:TightCB",
    "Electrons:MediumLH",
    "Electrons:TightLH",

    # "Taus",

    # "Taus_1Prong",
    # "Taus_Loose_1Prong",
    # "Taus_Medium_1Prong",
    # "Taus_Tight_1Prong",
    # "Taus_3Prong",
    # "Taus_Loose_3Prong",
    # "Taus_Medium_3Prong",
    # "Taus_Tight_3Prong",

    "Taus:Medium:1Prong",
    "Taus:Tight:1Prong",

    #    ":HLT_IDTrack_FS_FTF",
    #    ":HLT_IDTrack_FS_FTF:roi=HLT_FSRoI:vtx=HLT_IDVertex_FS",

    "HLT_j45_subjesgscIS_ftf_boffperf_split_L1J20:key=HLT_IDTrack_Bjet_FTF",
    "HLT_j45_subjesgscIS_ftf_boffperf_split_L1J20:key=HLT_IDTrack_Bjet_IDTrig",

    "HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20:key=HLT_IDTrack_FS_FTF:roi=HLT_FSRoI:vtx=HLT_IDVertex_FS",

    "HLT_j.*_presel_.*:key=HLT_IDTrack_JetSuper_FTF:roi=HLT_Roi_JetSuper",
    "HLT_j.*_presel_.*:key=HLT_IDTrack_JetSuper_FTF:roi=HLT_Roi_JetSuper:vtx=HLT_IDVertex_JetSuper",

    "HLT_j45_0eta290_020jvt_boffperf_pf_ftf_L1J20:key=HLT_IDTrack_Bjet_IDTrig:roi=HLT_Roi_Bjet:vtx=HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf_BTaggingSecVtx",
    # don't use FSJet any longer
    # "HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20:key=HLT_IDTrack_FS_FTF:roi=HLT_FSRoI:vtx=HLT_IDVertex_FSJet",
    "HLT_j45_pf_ftf_preselj20_L1J15:key=HLT_IDTrack_FS_FTF:roi=HLT_FSRoI:vtx=HLT_IDVertex_FS",
    # don't use FSJet any longer
    # "HLT_j45_pf_ftf_preselj20_L1J15:key=HLT_IDTrack_FS_FTF:roi=HLT_FSRoI:vtx=HLT_IDVertex_FSJet",
    "HLT_j45_0eta290_020jvt_boffperf_pf_ftf_L1J20:key=HLT_IDTrack_Bjet_IDTrig:roi=HLT_Roi_Bjet:vtx=HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf_BTaggingSecVtx",

    "HLT_fslrt.*:HLT_IDTrack_FSLRT_FTF;DTE",
    "HLT_fslrt.*:HLT_IDTrack_FS_FTF;DTE",
    "HLT_fslrt.*:HLT_IDTrack_FSLRT_IDTrig;DTE",

    "HLT_mu.*_idperf.*:HLT_IDTrack_Muon_FTF",
    "HLT_mu.*_idperf.*:HLT_IDTrack_Muon_FTF:roi=HLT_Roi_L2SAMuon",
    "HLT_mu.*_idperf.*:HLT_IDTrack_Muon_IDTrig",
    "HLT_mu.*_idperf.*:HLT_IDTrack_Muon_IDTrig:roi=HLT_Roi_L2SAMuon",
    "HLT_mu.*iv.*:HLT_IDTrack_MuonIso_FTF:roi=HLT_Roi_MuonIso",
    "HLT_mu.*iv.*:HLT_IDTrack_MuonIso_IDTrig:roi=HLT_Roi_MuonIso",

    "HLT_mu.*_LRT_idperf_.*:HLT_IDTrack_MuonLRT_FTF:HLT_Roi_L2SAMuon_LRT",
    "HLT_mu.*_LRT_idperf_.*:HLT_IDTrack_MuonLRT_IDTrig:HLT_Roi_L2SAMuon_LRT",
    "HLT_mu.*_LRT_idperf_.*:HLT_IDTrack_MuonLRT_FTF:HLT_Roi_L2SAMuon_LRT",

    "HLT_b.*perf.*:HLT_IDTrack_Bjet_FTF",
    "HLT_b.*perf.*:HLT_IDTrack_Bjet_IDTrig",
    "HLT_j.*perf.*:HLT_IDTrack_Bjet_FTF",
    "HLT_j.*perf.*:HLT_IDTrack_Bjet_IDTrig",

#    "HLT_e.*_etcut.*:HLT_IDTrack_Electron_FTF",
#    "HLT_e.*_etcut.*:HLT_IDTrack_Electron_IDTrig",
    "HLT_e.*:HLT_IDTrack_Electron_FTF:roi=HLT_Roi_FastElectron",
    "HLT_e.*:HLT_IDTrack_Electron_IDTrig",
    "HLT_e.*:HLT_IDTrack_Electron_GSF",

    "HLT_e20_idperf_loose_lrtloose_L1EM15VH:HLT_IDTrack_ElecLRT_FTF:HLT_Roi_FastElectron_LRT",
    "HLT_e30_idperf_loose_lrtloose_L1EM22VHI:HLT_IDTrack_ElecLRT_FTF:HLT_Roi_FastElectron_LRT",
    "HLT_e20_idperf_loose_lrtloose_L1EM15VH:HLT_IDTrack_ElecLRT_IDTrig:HLT_Roi_FastElectron_LRT",
    "HLT_e30_idperf_loose_lrtloose_L1EM22VHI:HLT_IDTrack_ElecLRT_IDTrig:HLT_Roi_FastElectron_LRT",

    # electron lrt e tag
    "HLT_e26_lhtight_ivarloose_e5_lhvloose_nopix_lrtloose_idperf_probe_L1EM22VHI:HLT_IDTrack_ElecLRT_FTF:roi=HLT_Roi_FastElectron_LRT:te=1",
    "HLT_e26_lhtight_ivarloose_e5_lhvloose_nopix_lrtloose_idperf_probe_L1EM22VHI:HLT_IDTrack_ElecLRT_IDTrig:roi=HLT_Roi_FastElectron_LRT:te=1",

    "HLT_e26_lhtight_ivarloose_e30_lhloose_nopix_lrtmedium_probe_L1EM22VHI:HLT_IDTrack_ElecLRT_FTF:roi=HLT_Roi_FastElectron_LRT:te=1",
    "HLT_e26_lhtight_ivarloose_e30_lhloose_nopix_lrtmedium_probe_L1EM22VHI:HLT_IDTrack_ElecLRT_IDTrig:roi=HLT_Roi_FastElectron_LRT:te=1",
    # electron lrt photon tag
    "HLT_e5_lhvloose_nopix_lrtloose_idperf_probe_g25_medium_L1EM20VH:HLT_IDTrack_ElecLRT_FTF:roi=HLT_Roi_FastElectron_LRT:te=0",
    "HLT_e5_lhvloose_nopix_lrtloose_idperf_probe_g25_medium_L1EM20VH:HLT_IDTrack_ElecLRT_IDTrig:roi=HLT_Roi_FastElectron_LRT:te=0",

    "HLT_e30_lhloose_nopix_lrtmedium_probe_g25_medium_L1EM20VH:HLT_IDTrack_ElecLRT_FTF:roi=HLT_Roi_FastElectron_LRT:te=0",
    "HLT_e30_lhloose_nopix_lrtmedium_probe_g25_medium_L1EM20VH:HLT_IDTrack_ElecLRT_IDTrig:roi=HLT_Roi_FastElectron_LRT:te=0",

    # double electron chains for tag and probe analysis
    "HLT_e26_lhtight_e14_etcut_idperf_nogsf_probe_50invmAB130_L1EM22VHI:key=HLT_IDTrack_Electron_FTF:extra=el_tag:roi=HLT_Roi_FastElectron:te=0",
    "HLT_e26_lhtight_e14_etcut_idperf_nogsf_probe_50invmAB130_L1EM22VHI:key=HLT_IDTrack_Electron_FTF:extra=el_probe:roi=HLT_Roi_FastElectron:te=1",

    "HLT_e26_lhtight_e14_etcut_idperf_nogsf_probe_50invmAB130_L1EM22VHI:key=HLT_IDTrack_Electron_IDTrig:extra=el_tag:te=0",
    "HLT_e26_lhtight_e14_etcut_idperf_nogsf_probe_50invmAB130_L1EM22VHI:key=HLT_IDTrack_Electron_IDTrig:extra=el_probe:te=1",

    "HLT_e26_lhtight_e14_etcut_idperf_probe_50invmAB130_L1EM22VHI:key=HLT_IDTrack_Electron_GSF:extra=el_tag:te=0",
    "HLT_e26_lhtight_e14_etcut_idperf_probe_50invmAB130_L1EM22VHI:key=HLT_IDTrack_Electron_GSF:extra=el_probe:te=1",

    # two stage tau FTF
    "HLT_tau.*_idperf.*tracktwo.*:HLT_IDTrack_TauCore_FTF:roi=HLT_Roi_TauCore",
    "HLT_tau.*_idperf.*tracktwo.*:HLT_IDTrack_TauIso_FTF:roi=HLT_Roi_TauIso",
    "HLT_tau.*_idperf.*tracktwo.*:HLT_IDTrack_TauIso_FTF:roi=HLT_Roi_TauIsoBDT",

    # two stage tau precision tracking - empty ???
    "HLT_tau.*_idperf.*tracktwo.*:HLT_IDTrack_Tau_IDTrig:roi=HLT_Roi_TauIso",
    "HLT_tau.*_idperf.*tracktwo.*:HLT_IDTrack_Tau_IDTrig:roi=HLT_Roi_TauIso:vtx=HLT_IDVertex_Tau",


    # should get single stage tau
    "HLT_tau.*_idperf.*_track_.*:HLT_IDTrack_Tau_FTF:roi=HLT_Roi_Tau",
    "HLT_tau.*_idperf.*_track_.*:HLT_IDTrack_Tau_IDTrig:roi=HLT_Roi_Tau",

    # LRT tau
    "HLT_tau.*trackLRT.*:HLT_IDTrack_TauLRT_FTF:roi=HLT_Roi_TauLRT",
    "HLT_tau.*trackLRT.*:HLT_IDTrack_TauLRT_IDTrig:roi=HLT_Roi_TauLRT",

    # none of these will work
    "HLT_tau.*_idperf.*:HLT_IDTrack_Tau_IDTrig",

    # should work for single stage tau ???
    # "HLT_tau.*_idperf.*:HLT_IDTrack_Tau_FTF",

    # should work for a two stage tau ??
    # "HLT_tau.*_idperf.*:HLT_IDTrack_TauIso_FTF",

    "HLT_mu4.*_cosmic.*:HLT_IDTrack_Cosmic_FTF",
    "HLT_mu4.*_cosmic.*:HLT_IDTrack_Cosmic_IDTrig",
    "HLT_mu4.*_cosmic.*:HLT_IDTrack_Cosmic_EFID",

    #"HLT_mb.*:HLT_IDTrack_Cosmic_EFID",
    #"HLT_mb.*:HLT_IDTrack_MinBias_FTF",  #There are no tracks here
    "HLT_mb.*:HLT_IDTrack_MinBias_IDTrig",
    #"HLT_mb.*:HLT_IDTrack_MinBias_EFID"  #There are no tracks here
    
    "HLT_2mu4_bBmumux_BsmumuPhi_L12MU3V:HLT_IDTrack_Bmumux_FTF",
    "HLT_2mu4_bBmumux_BsmumuPhi_L12MU3V:HLT_IDTrack_Bmumux_IDTrig"
    
    ]

  from AthenaCommon.AppMgr import release_metadata
  d = release_metadata()
  TestMonTool.releaseMetaData = d['nightly name'] + " " + d['nightly release'] + " " + d['date'] + " " + d['platform'] + " " + d['release']
  TestMonTool.outputFileName="TrkNtuple.root"
  algseq += [ TestMonTool ]
  print (TestMonTool)


print ("configured everything")


# include("AthAnalysisBaseComps/SuppressLogging.py")              #Optional include to suppress as much athena output as possible. Keep at bottom of joboptions so that it doesn't suppress the logging of the things you have configured above


MessageSvc.infoLimit  = 100000000
