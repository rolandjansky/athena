# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def HLTEgammaMonitoringTool():

  from AthenaCommon.AppMgr import ToolSvc

  from TrigEgammaMonitoring.TrigEgammaMonitoringConf import HLTEgammaMonTool
  from TrigHLTMonitoring.HLTMonTriggerList import hltmonList  # access to central tool
  HLTEgammaMon = HLTEgammaMonTool(name = 'HLTEgammaMon', histoPathBase = "/Trigger/HLT")
  ToolSvc += HLTEgammaMon;

  from TrigEgammaMonitoring.TrigEgammaMonitoringConf import HLTEgammaNavMonTool
  HLTEgammaNavMon = HLTEgammaNavMonTool(name = 'HLTEgammaNavMon', histoPathBase = "/Trigger/HLT")
  HLTEgammaNavMon.signatures = hltmonList.monitoring_egamma
  HLTEgammaNavMon.categories = ['primary_single_ele' , 'primary_single_ele_cutbased' , 'primary_double_ele' , 'primary_double_ele_cutbased' , 'monitoring_ele_idperf' , 'monitoring_ele_idperf_cutbased' , 'monitoring_Zee' , 'monitoring_Jpsiee' , 'primary_single_pho' , 'primary_double_pho', 'primary_single_ele_iso', 'primary_single_ele_cutbased_iso']
  HLTEgammaNavMon.sigsPerCategory = [4, 4, 3, 3, 4, 4, 2, 2, 7, 3, 5, 5]
  #HLTEgammaNavMon.signatures = ['e5_NoCut','e5_medium1','e10_loose','e20_loose_IdScan']
  #HLTEgammaNavMon.signatures +=['e5_NoCut_cosmic','e10_loose_cosmic','e10_medium_cosmic']
  #HLTEgammaNavMon.doExtrapol=False
  ToolSvc += HLTEgammaNavMon;
  list = ['HLTEgammaNavMonTool/HLTEgammaNavMon'];
  #return list

  from TrigEgammaMonitoring.TrigEgammaMonitoringConf import HLTEgammaNavSigTEMonTool
  HLTEgammaNavSigTEMon = HLTEgammaNavSigTEMonTool(name = 'HLTEgammaNavSigTEMon', histoPathBase = "/Trigger/HLT",doExtrapol=False)
  #---Electron Signatures for Collisions:
  #HLTEgammaNavSigTEMon.signatures = ['e28_tight_iloose']
  HLTEgammaNavSigTEMon.signatures = hltmonList.monitoring_egamma
  HLTEgammaNavSigTEMon.categories = ['primary_single_ele' , 'primary_single_ele_cutbased' , 'primary_double_ele' , 'primary_double_ele_cutbased' , 'monitoring_ele_idperf' , 'monitoring_ele_idperf_cutbased' , 'monitoring_Zee' , 'monitoring_Jpsiee' , 'primary_single_pho' , 'primary_double_pho', 'primary_single_ele_iso', 'primary_single_ele_cutbased_iso']
  HLTEgammaNavSigTEMon.sigsPerCategory = [4, 4, 3, 3, 4, 4, 2, 2, 7, 3, 5, 5]
  #HLTEgammaNavSigTEMon.signatures +=['e5_loose1','e24_medium_iloose','e28_tight_iloose','e28_tight_iloose_L2StarA']
  #HLTEgammaNavSigTEMon.signatures += ['e24vh_medium1']
  #HLTEgammaNavSigTEMon.signatures +=['e24vhi_medium1']
  #HLTEgammaNavSigTEMon.signatures +=['2e12Tvh_loose1']
  #HLTEgammaNavSigTEMon.signatures +=['e24vh_tight1_e15_NoCut_Zee']
  #HLTEgammaNavSigTEMon.signatures +=['e24vh_medium1_L2StarB']
  #---Photon Signatures for Collisions:
  #HLTEgammaNavSigTEMon.signatures += ['g35_loose_g25_loose']
  #---Cosmics Signatures:
  #HLTEgammaNavSigTEMon.signatures += ['g5_NoCut_cosmic']
  #---HeavyIon signatures:
  #HLTEgammaNavSigTEMon.signatures += ['e5_medium1_IDTrkNoCut']
  #HLTEgammaNavSigTEMon.signatures += ['e5_tight1_e5_NoCut']
  #HLTEgammaNavSigTEMon.signatures += ['e15_loose1']
  #HLTEgammaNavSigTEMon.signatures += ['g20_medium']
  #---Configure pass-states to be monitored
  HLTEgammaNavSigTEMon.doActiveChain = False
  HLTEgammaNavSigTEMon.doActiveTe = True
  HLTEgammaNavSigTEMon.doAllTe = False
  #---Turns off PassedTE plots (partially covered by the doActiveTe option, but this
  #   suppresses booking and plotting entirely)
  HLTEgammaNavSigTEMon.skipActiveTe = False
  #---Configure rest
  HLTEgammaNavSigTEMon.UsePreConditionalPassThroughChain = False
  HLTEgammaNavSigTEMon.OfflineEleMinPTCut = 2. #GeV
  HLTEgammaNavSigTEMon.OfflinePhoMinPTCut = 2. #GeV
  HLTEgammaNavSigTEMon.DoLumiCalc = False
  HLTEgammaNavSigTEMon.DatabaseName = "COOLONL_TRIGGER/COMP200"
  HLTEgammaNavSigTEMon.CoolFolderName = "/TRIGGER/LUMI/LBLESTONL"
  HLTEgammaNavSigTEMon.TagName = "OflLumi-7TeV-001"
  ToolSvc += HLTEgammaNavSigTEMon;
  list = ['HLTEgammaNavSigTEMonTool/HLTEgammaNavSigTEMon', 'HLTEgammaNavMonTool/HLTEgammaNavMon'];
  return list

  list = ['HLTEgammaMonTool/HLTEgammaMon', 'HLTEgammaNavSigTEMonTool/HLTEgammaNavSigTEMon', 'HLTEgammaNavMonTool/HLTEgammaNavMon'];
  return list

def HLTEgammaMonitoringDumpTool():
  from AthenaCommon.AppMgr import ToolSvc
  from TrigEgammaMonitoring.TrigEgammaMonitoringConf import HLTEgammaDumpTool
  HLTEgammaDump = HLTEgammaDumpTool(name = 'HLTEgammaDump', histoPathBase = "/Trigger/HLT")
  ToolSvc += HLTEgammaDump;
  list = ['HLTEgammaDumpTool/HLTEgammaDump'];
  return list

def TrigEgammaMonitoringTool():
  from AthenaCommon.AppMgr import ToolSvc
  from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaEmulationTool
  from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavAnalysisTool,TrigEgammaNavTPAnalysisTool,TrigEgammaNavTPJpsieeAnalysisTool
  from TrigHLTMonitoring.HLTMonTriggerList import hltmonList

  probelist1 = hltmonList.primary_single_pho + hltmonList.primary_single_ele + hltmonList.primary_single_ele_cutbased + hltmonList.monitoring_ele_idperf + hltmonList.monitoring_ele_idperf_cutbased
  probelist1 += ['L1_EM18VH', 'L1_EM20VH', 'L1_EM20VHI']
  probelist2 = hltmonList.primary_single_ele + hltmonList.primary_single_ele_cutbased + hltmonList.monitoring_ele_idperf + hltmonList.monitoring_ele_idperf_cutbased + hltmonList.monitoring_Zee + hltmonList.monitoring_Jpsiee
  probelist2 += ['e24_tight_iloose_HLTCalo_L1EM20VH', 'e24_lhtight_iloose_L2EFCalo_L1EM20VH', 'e24_lhmedium_cutd0dphideta_iloose_L1EM20VH', 'e24_lhmedium_nod0_iloose_L1EM20VH', 'e24_lhmedium_nodeta_iloose_L1EM20VH', 'e24_lhmedium_nodphires_iloose_L1EM20VH', 'L1_EM18VH', 'L1_EM20VH', 'L1_EM20VHI']
  probelistJPsi = ['e5_loose','e5_lhloose','e5_vloose','e5_lhvloose']

  basePath = '/HLT/Egamma/'
  tagItems = hltmonList.primary_single_ele + hltmonList.primary_single_ele_cutbased
  tagItemsJPsi = ['e5_tight_e4_etcut','e9_tight_e4_etcut','e14_tight_e4_etcut','e9_etcut_e5_tight','e14_etcut_e5_tight','e5_tight_e4_etcut_Jpsiee','e9_tight_e4_etcut_Jpsiee','e14_tight_e4_etcut_Jpsiee','e9_etcut_e5_tight_Jpsiee','e14_etcut_e5_tight_Jpsiee']

  Analysis = TrigEgammaNavAnalysisTool(name='NavAnalysis',
                                       DirectoryPath=basePath+'Analysis',
                                       TriggerList=probelist1,
                                       File="",
                                       OutputLevel=0)
  TPAnalysis = TrigEgammaNavTPAnalysisTool(name='NavTPAnalysis',
                                           DirectoryPath=basePath+'TPAnalysis',
                                           TriggerList=probelist2,
                                           File="",
                                           TagTriggerList=tagItems,
                                           OutputLevel=0)
  JpsiTPAnalysis = TrigEgammaNavTPJpsieeAnalysisTool(name='NavTPJpsieeAnalysis',
                                                     DirectoryPath=basePath+'TPJpsieeAnalysis',
                                                     TriggerList=probelistJPsi,
                                                     File="",
                                                     TagTriggerList=tagItemsJPsi,
                                                     RemoveCrack=True,
                                                     ZeeLowerMass=2,
                                                     ZeeUpperMass=5,
                                                     OfflineTagMinEt=5,
                                                     OfflineProbeMinEt=5,
                                                     OutputLevel=0,
                                                     DetailedHistograms=False,
                                                     doJpsiee=True)
  from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaMonTool
  TrigEgammaMon = TrigEgammaMonTool( name = "TrigEgammaMon", 
                                     histoPathBase=basePath,
                                     Tools=["TrigEgammaNavAnalysisTool/NavAnalysis",
                                            "TrigEgammaNavTPAnalysisTool/NavTPAnalysis",
                                            "TrigEgammaNavTPAnalysisTool/NavTPJpsieeAnalysis"])
  ToolSvc += TrigEgammaMon;
  list = ['TrigEgammaMonTool/TrigEgammaMon'];
  return list
