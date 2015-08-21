# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def HLTEgammaMonitoringDumpTool():
  from AthenaCommon.AppMgr import ToolSvc
  from TrigEgammaMonitoring.TrigEgammaMonitoringConf import HLTEgammaDumpTool
  HLTEgammaDump = HLTEgammaDumpTool(name = 'HLTEgammaDump', histoPathBase = "/Trigger/HLT")
  ToolSvc += HLTEgammaDump;
  list = ['HLTEgammaDumpTool/HLTEgammaDump'];
  return list

def TrigEgammaMonitoringTool():
  from AthenaCommon.AppMgr import ToolSvc
  from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavAnalysisTool,TrigEgammaNavTPAnalysisTool,TrigEgammaNavTPJpsieeAnalysisTool
  from TrigHLTMonitoring.HLTMonTriggerList import hltmonList

  probelist1 = hltmonList.primary_single_pho + hltmonList.primary_single_ele + hltmonList.primary_single_ele_cutbased + hltmonList.primary_single_ele_iso + hltmonList.primary_single_ele_cutbased_iso
  probelist1 += ['L1_EM18VH', 'L1_EM20VH', 'L1_EM20VHI']
  probelist2 = hltmonList.primary_single_ele + hltmonList.primary_single_ele_cutbased + hltmonList.primary_single_ele_iso + hltmonList.primary_single_ele_cutbased_iso + hltmonList.monitoring_Zee + hltmonList.monitoring_Jpsiee
  probelist2 += ['e24_tight_iloose_HLTCalo_L1EM20VH', 'e24_lhtight_iloose_L2EFCalo_L1EM20VH', 'e24_lhmedium_cutd0dphideta_iloose_L1EM20VH', 'e24_lhmedium_nod0_iloose_L1EM20VH', 'e24_lhmedium_nodeta_iloose_L1EM20VH', 'e24_lhmedium_nodphires_iloose_L1EM20VH', 'L1_EM18VH', 'L1_EM20VH', 'L1_EM20VHI']
  probelistJPsi = ['e5_loose','e5_lhloose','e5_vloose','e5_lhvloose']
  probelistZee = hltmonList.primary_single_ele + hltmonList.primary_single_ele_cutbased

  basePath = '/HLT/Egamma/'
  tagItems = hltmonList.primary_single_ele + hltmonList.primary_single_ele_cutbased
  tagItemsJPsi = ['e5_tight_e4_etcut','e9_tight_e4_etcut','e14_tight_e4_etcut','e9_etcut_e5_tight','e14_etcut_e5_tight','e5_tight_e4_etcut_Jpsiee','e9_tight_e4_etcut_Jpsiee','e14_tight_e4_etcut_Jpsiee','e9_etcut_e5_tight_Jpsiee','e14_etcut_e5_tight_Jpsiee']
  tagItemsZee = hltmonList.monitoring_Zee

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
  ZeeTPAnalysis = TrigEgammaNavTPJpsieeAnalysisTool(name='NavTPZeeAnalysis',
                                                    DirectoryPath=basePath+'TPZeeAnalysis',
                                                    TriggerList=probelistZee,
                                                    File="",
                                                    TagTriggerList=tagItemsZee,
                                                    RemoveCrack=True,
                                                    ZeeLowerMass=80,
                                                    ZeeUpperMass=100,
                                                    OfflineTagMinEt=25,
                                                    OfflineProbeMinEt=24,
                                                    OutputLevel=0,
                                                    DetailedHistograms=False,
                                                    doJpsiee=False)
  from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaMonTool
  TrigEgammaMon = TrigEgammaMonTool( name = "TrigEgammaMon", 
                                     histoPathBase=basePath,
                                     Tools=["TrigEgammaNavAnalysisTool/NavAnalysis",
                                            "TrigEgammaNavTPAnalysisTool/NavTPAnalysis",
                                            "TrigEgammaNavTPAnalysisTool/NavTPJpsieeAnalysis",
                                            "TrigEgammaNavTPAnalysisTool/NavTPZeeAnalysis"])
  ToolSvc += TrigEgammaMon;
  list = ['TrigEgammaMonTool/TrigEgammaMon'];
  return list
