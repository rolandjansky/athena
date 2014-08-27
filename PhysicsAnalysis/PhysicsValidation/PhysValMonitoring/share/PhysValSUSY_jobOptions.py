### configuration
run_default_rel19         = True
run_migrated_rel17_tools  = True
rel17_use_separate_output = False
run_other_tools = {
  'MET'  : True,
  'Muon' : True,
  'Tau'  : True,
  'InDet': False,
}


### print configuration
print "PhysValSUSY job options:"
print "  Running default release 19 tool   : %s" % str(run_default_rel19)
print "  Running migrated release 17 tool  : %s" % str(run_migrated_rel17_tools)
print "    output in separate file         : %s" % str(rel17_use_separate_output)
print "  Running following other tools     : %s" % str(run_other_tools)


### consistency checks

# avoid tools being added twice
for other_tool_name in [other_tool for other_tool, run_this in run_other_tools.items() if run_this is True]:
  if "do" + other_tool_name in vars():
    if vars()["do" + other_tool_name] is True:
      print "Configuration warning: do%s was already set active, removing from configuration." % other_tool_name
      run_other_tools[other_tool_name] = False


### perform initialization / add tools

if run_default_rel19:
  # add default tool
  from SUSYPhysValMonitoring.SUSYPhysValMonitoringConf import SUSYPhysVal__SUSYPhysValMonitoring

  tool1 = SUSYPhysVal__SUSYPhysValMonitoring()
  tool1.EnableLumi = False
  tool1.OutputLevel = INFO
  tool1.DetailLevel = 10
  tool1.UseTruthInformation = True

  from AthenaCommon.AppMgr import ToolSvc
  ToolSvc += tool1

  monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
  monMan.AthenaMonTools += [ tool1 ]

  
for run_other_tool in [other_tool for other_tool, run_this in run_other_tools.items() if run_this is True]:
  # add "external tools"
  include("PhysValMonitoring/PhysVal" + run_other_tool + "_jobOptions.py")
  

if run_migrated_rel17_tools:
  # add migrated tool (main part of old release 17 validation package)

  ### imports
  from SUSYPhysValMonitoring.SUSYPhysValMonitoringConf import *
  
  doTrigger = False
  doTruth   = False
  
  # specify input container names
  TrigDecisionTool       = "TrigDecisionTool"
  McEventCollection      = "GEN_AOD"
  TrackParticleContainer = "TrackParticleCandidate"
  JetContainer           = "AntiKt4EMTopoJets" ##leave off the suffix "Jets"
  ElectronContainer      = "ElectronCollection"
  MuonContainer          = "Muons"
  TauJetContainer        = "TauRecContainer"
  MissingET              = "MET_RefFinal"
  # for 17.2.1 (SUSYD3PDMaker-00-12-00)
  # SUSY_MET_name          = "MET_RefFinal_Simplified20"          # MET name to be used by SUSYSusyRec.cxx
  # SUSY_MET_muons_name    = "MET_Muon_Total_Staco_Simplified20"  # MET muons contribution name to be used by SUSYSusyRec.cxx
  # for 17.2.7.5.9 (SUSYD3PDMaker-00-12-36)
  #SUSY_MET_name          = "MET_RefFinal_Egamma10NoTau"          # MET name to be used by SUSYSusyRec.cxx
  #SUSY_MET_muons_name    = "MET_Muon_Staco_Egamma10NoTau"        # MET muons contribution name to be used by SUSYSusyRec.cxx
  SUSY_MET_name          = "Final"
  SUSY_MET_muons_name    = "Muons"

  # init and add tool
  SusyPlot = SUSYPlot("SusyPlot")
  SusyPlot.DoTrigger = doTrigger
  SusyPlot.DoTruth   = doTruth
  SusyPlot.HistToolKeys = [ "SUSYSusyRec/susyTool" ]
  SusyPlot += SUSYSusyRec("susyTool")
  SusyPlot.susyTool.ElectronName  = ElectronContainer
  SusyPlot.susyTool.MuonName      = MuonContainer
  SusyPlot.susyTool.TauName       = TauJetContainer
  SusyPlot.susyTool.JetName       = JetContainer
  SusyPlot.susyTool.MetName       = MissingET
  SusyPlot.susyTool.SUSYMissingET      = SUSY_MET_name
  SusyPlot.susyTool.SUSYMissingETMuons = SUSY_MET_muons_name
  SusyPlot.susyTool.McEventName   = McEventCollection
  SusyPlot.susyTool.OutputLevel   = INFO
  if doTrigger:
    SusyPlot.TrigDecisionTool = ToolSvc.TrigDecisionTool
  SusyPlot.susyTool.PtLeptonCut    = 20*GeV
  SusyPlot.susyTool.PtLeptonPreCut = 20*GeV 
  SusyPlot.susyTool.EtIsolCut      = 10*GeV
  SusyPlot.susyTool.EtMissCut      = 80*GeV
  SusyPlot.susyTool.etaJetCut      = 3
  SusyPlot.susyTool.PtJetCut       = 20*GeV
  SusyPlot.susyTool.PtJet0Cut      = 60*GeV
  SusyPlot.susyTool.PtJet1Cut      = 30*GeV
  SusyPlot.susyTool.MeffCut        = 500*GeV
  SusyPlot.susyTool.MTCut          = 100*GeV
  SusyPlot.susyTool.STCut          = 0.2

  topSequence += SusyPlot
  
  if rel17_use_separate_output:
    ### if you want to write histograms to separate file use this:
    from AthenaCommon.AppMgr import theApp
    theApp.HistogramPersistency = "ROOT"
    from AthenaCommon.AppMgr import ServiceMgr
    ## The string "TestMon" in the argument below is the 'FileKey'
    ## used by Athena to access the output file internally
    svcMgr.THistSvc.Output += ["PhysVal2 DATAFILE='hist.root' OPT='RECREATE'"]
    svcMgr.THistSvc.PrintAll = True
    svcMgr.THistSvc.OutputLevel = DEBUG
  else:
    SusyPlot.susyTool.HistBaseDirectory = "/PhysVal/SUSY/ETmiss/"

    