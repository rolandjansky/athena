# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def HLTEgammaMonitoringTool():

  from AthenaCommon.AppMgr import ToolSvc

  from TrackToCalo.ExtrapolateToCaloToolBase import \
       ExtrapolateToCaloToolFactory
  extrap = ExtrapolateToCaloToolFactory (depth='showerdefault',
                                         straightLine=False)
  ToolSvc+=extrap

  from TrigEgammaMonitoring.TrigEgammaMonitoringConf import HLTEgammaMonTool
  HLTEgammaMon = HLTEgammaMonTool(name = 'HLTEgammaMon', histoPathBase = "/Trigger/HLT")
  ToolSvc += HLTEgammaMon;

  from TrigEgammaMonitoring.TrigEgammaMonitoringConf import HLTEgammaNavMonTool
  HLTEgammaNavMon = HLTEgammaNavMonTool(name = 'HLTEgammaNavMon', histoPathBase = "/Trigger/HLT")
  HLTEgammaNavMon.signatures = ['e5_loose1','e24_medium_iloose','e28_tight_iloose','e28_tight_iloose_L2StarA']
  #HLTEgammaNavMon.signatures = ['e5_NoCut','e5_medium1','e10_loose','e20_loose_IdScan']
  #HLTEgammaNavMon.signatures +=['e5_NoCut_cosmic','e10_loose_cosmic','e10_medium_cosmic']
  #HLTEgammaNavMon.doExtrapol=False
  HLTEgammaNavMon.TrackExtrapolator=extrap
  ToolSvc += HLTEgammaNavMon;
  list = ['HLTEgammaNavMonTool/HLTEgammaNavMon'];
  return list

  from TrigEgammaMonitoring.TrigEgammaMonitoringConf import HLTEgammaNavSigTEMonTool
  HLTEgammaNavSigTEMon = HLTEgammaNavSigTEMonTool(name = 'HLTEgammaNavSigTEMon', histoPathBase = "/Trigger/HLT")
  #---Electron Signatures for Collisions:
  HLTEgammaNavSigTEMon.signatures = ['e24vh_medium1']
  HLTEgammaNavSigTEMon.signatures +=['e24vhi_medium1']
  HLTEgammaNavSigTEMon.signatures +=['2e12Tvh_loose1']
  HLTEgammaNavSigTEMon.signatures +=['e24vh_tight1_e15_NoCut_Zee']
  HLTEgammaNavSigTEMon.signatures +=['e24vh_medium1_L2StarB']
  #---Photon Signatures for Collisions:
  HLTEgammaNavSigTEMon.signatures += ['g35_loose_g25_loose']
  #---Cosmics Signatures:
  HLTEgammaNavSigTEMon.signatures += ['g5_NoCut_cosmic']
  #---HeavyIon signatures:
  HLTEgammaNavSigTEMon.signatures += ['e5_medium1_IDTrkNoCut']
  HLTEgammaNavSigTEMon.signatures += ['e5_tight1_e5_NoCut']
  HLTEgammaNavSigTEMon.signatures += ['e15_loose1']
  HLTEgammaNavSigTEMon.signatures += ['g20_medium']
  #---Configure pass-states to be monitored
  HLTEgammaNavSigTEMon.doActiveChain = True
  HLTEgammaNavSigTEMon.doActiveTe = False
  HLTEgammaNavSigTEMon.doAllTe = False
  #---Turns off PassedTE plots (partially covered by the doActiveTe option, but this
  #   suppresses booking and plotting entirely)
  HLTEgammaNavSigTEMon.skipActiveTe = True
  #---Configure rest
  HLTEgammaNavSigTEMon.UsePreConditionalPassThroughChain = False
  HLTEgammaNavSigTEMon.OfflineEleMinPTCut = 25. #GeV
  HLTEgammaNavSigTEMon.OfflinePhoMinPTCut = 40. #GeV
  HLTEgammaNavSigTEMon.DoLumiCalc = False
  HLTEgammaNavSigTEMon.DatabaseName = "COOLONL_TRIGGER/COMP200"
  HLTEgammaNavSigTEMon.CoolFolderName = "/TRIGGER/LUMI/LBLESTONL"
  HLTEgammaNavSigTEMon.TagName = "OflLumi-7TeV-001"
  ToolSvc += HLTEgammaNavSigTEMon;

  list = ['HLTEgammaMonTool/HLTEgammaMon', 'HLTEgammaNavSigTEMonTool/HLTEgammaNavSigTEMon', 'HLTEgammaNavMonTool/HLTEgammaNavMon'];
  return list

def HLTEgammaMonitoringDumpTool():
  from AthenaCommon.AppMgr import ToolSvc
  from TrigEgammaMonitoring.TrigEgammaMonitoringConf import HLTEgammaDumpTool
  HLTEgammaDump = HLTEgammaDumpTool(name = 'HLTEgammaDump', histoPathBase = "/Trigger/HLT")
  ToolSvc += HLTEgammaDump;
  list = ['HLTEgammaDumpTool/HLTEgammaDump'];
  return list
