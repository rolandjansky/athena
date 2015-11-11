# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TrigEgammaMonitoringTool():
  from AthenaCommon.AppMgr import ToolSvc
  from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConfig import TrigEgammaNavAnalysisTool,TrigEgammaNavTPAnalysisTool

  probelist = ['e15_loose_ion','e15_etcut_ion','e15_medium_ion']
  probelist += ['e15_loose','e15_etcut','e15_medium','e15_lhloose','e15_lhmedium']
  probelist += ['g20_loose_ion','g20_etcut_ion','g20_loose','g20_etcut']
  probelist += ['L1_EM10','L1_EM12','L1_EM15']
  probelistTP = ['e15_loose_ion','e15_etcut_ion','e15_medium_ion']
  probelistTP += ['e15_loose','e15_etcut','e15_medium','e15_lhloose','e15_lhmedium']
  tagItems = ['e15_loose_ion','e15_medium_ion']
  tagItems += ['e15_loose','e15_medium','e15_lhloose','e15_lhmedium']
  basePath = '/HLT/Egamma/'

  Analysis = TrigEgammaNavAnalysisTool(name='NavAnalysis',
                                       DirectoryPath=basePath+'Analysis',
                                       TriggerList=probelist,
                                       File="",
                                       OutputLevel=0)
  TPAnalysis = TrigEgammaNavTPAnalysisTool(name='NavTPAnalysis',
                                           DirectoryPath=basePath+'TPAnalysis',
                                           TriggerList=probelistTP,
                                           File="",
                                           TagTriggerList=tagItems,
                                           OutputLevel=0)
  from TrigEgammaAnalysisTools.TrigEgammaAnalysisToolsConf import TrigEgammaMonTool
  TrigEgammaMon = TrigEgammaMonTool( name = "TrigEgammaMon", 
                                     histoPathBase=basePath,
                                     Tools=["TrigEgammaNavAnalysisTool/NavAnalysis",
                                            "TrigEgammaNavTPAnalysisTool/NavTPAnalysis"])
  ToolSvc += TrigEgammaMon;
  list = ['TrigEgammaMonTool/TrigEgammaMon'];
  return list
