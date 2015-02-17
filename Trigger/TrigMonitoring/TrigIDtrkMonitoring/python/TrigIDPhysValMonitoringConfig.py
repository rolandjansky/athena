# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def TrigIDPhysValMonitoringTool():
  from AthenaCommon.Constants import INFO,ERROR,FALSE,TRUE,DEBUG,VERBOSE

  # dataTypes: userDefined = 0, monteCarlo, collisions, cosmics
  if not 'DQMonFlags' in dir():
    from AthenaMonitoring.DQMonFlags import DQMonFlags
  dataType = DQMonFlags.monManDataType()

  from AthenaCommon.AppMgr import ToolSvc

  if not 'rec' in dir():
    from RecExConfig.RecFlags  import rec

  list = []
  if rec.doInDet:
    from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestPhysValMon
    from AthenaCommon.AppMgr import release_metadata
    d = release_metadata()



    ############### Electrons ###############
    TestIDPhysValMonElectron = TrigTestPhysValMon(name="TestIDPhysValMonElectron")
    TestIDPhysValMonElectron.OutputLevel = INFO

    try:
      TestIDPhysValMonElectron.EnableLumi = False
    except:
      pass

    TestIDPhysValMonElectron.buildNtuple = False
    TestIDPhysValMonElectron.AnalysisConfig = "Tier0" #T0 Analysis
    # TestIDPhysValMonElectron.AnalysisConfig = "nTuple" #nTuple Analysis

    if (rec.doTruth == True):
      TestIDPhysValMonElectron.mcTruth = True
      TestIDPhysValMonElectron.ntupleChainNames = ['Truth']
      TestIDPhysValMonElectron.sctHitsOffline = -1
      TestIDPhysValMonElectron.pixHitsOffline = -1
      TestIDPhysValMonElectron.SelectTruthPdgId = 11
    else:
      TestIDPhysValMonElectron.mcTruth = False
      TestIDPhysValMonElectron.ntupleChainNames = ['Offline','Electron']
      TestIDPhysValMonElectron.sctHitsOffline = 1
      TestIDPhysValMonElectron.pixHitsOffline = 1


    TestIDPhysValMonElectron.ntupleChainNames += ['HLT_e.*idperf.*:InDetTrigTrackingxAODCnv_Electron_EFID']
    TestIDPhysValMonElectron.releaseMetaData = d['nightly name'] + " " + d['nightly release'] + " " + d['date'] + " " + d['platform'] + " " + d['release']

    ToolSvc += TestIDPhysValMonElectron
    list += ["TrigTestPhysValMon/TestIDPhysValMonElectron"]



    ############### Muons ###############
    TestIDPhysValMonMuon = TrigTestPhysValMon(name="TestIDPhysValMonMuon")
    TestIDPhysValMonMuon.OutputLevel = INFO

    try:
      TestIDPhysValMonMuon.EnableLumi = False
    except:
      pass

    TestIDPhysValMonMuon.buildNtuple = False
    TestIDPhysValMonMuon.AnalysisConfig = "Tier0" #T0 Analysis
    # TestIDPhysValMonMuon.AnalysisConfig = "nTuple" #nTuple Analysis

    if (rec.doTruth == True):
      TestIDPhysValMonMuon.mcTruth = True
      TestIDPhysValMonMuon.ntupleChainNames = ['Truth']
      TestIDPhysValMonMuon.sctHitsOffline = -1
      TestIDPhysValMonMuon.pixHitsOffline = -1
      TestIDPhysValMonMuon.SelectTruthPdgId = 13
    else:
      TestIDPhysValMonMuon.mcTruth = False
      TestIDPhysValMonMuon.ntupleChainNames = ['Offline','Muon']
      TestIDPhysValMonMuon.sctHitsOffline = 1
      TestIDPhysValMonMuon.pixHitsOffline = 1


    TestIDPhysValMonMuon.ntupleChainNames += ['HLT_mu.*idperf.*:InDetTrigTrackingxAODCnv_Muon_EFID']
    TestIDPhysValMonMuon.releaseMetaData = d['nightly name'] + " " + d['nightly release'] + " " + d['date'] + " " + d['platform'] + " " + d['release']

    ToolSvc += TestIDPhysValMonMuon
    list += ["TrigTestPhysValMon/TestIDPhysValMonMuon"]



    ############### Taus ###############
    TestIDPhysValMonTau = TrigTestPhysValMon(name="TestIDPhysValMonTau")
    TestIDPhysValMonTau.OutputLevel = INFO

    try:
      TestIDPhysValMonTau.EnableLumi = False
    except:
      pass

    TestIDPhysValMonTau.buildNtuple = False
    TestIDPhysValMonTau.AnalysisConfig = "Tier0" #T0 Analysis
    # TestIDPhysValMonTau.AnalysisConfig = "nTuple" #nTuple Analysis

    if (rec.doTruth == True):
      TestIDPhysValMonTau.mcTruth = True
      TestIDPhysValMonTau.ntupleChainNames = ['Truth']
      TestIDPhysValMonTau.sctHitsOffline = -1
      TestIDPhysValMonTau.pixHitsOffline = -1
      TestIDPhysValMonTau.SelectTruthPdgId = 15
    else:
      TestIDPhysValMonTau.mcTruth = False
      TestIDPhysValMonTau.ntupleChainNames = ['Offline','Tau']
      TestIDPhysValMonTau.sctHitsOffline = 1
      TestIDPhysValMonTau.pixHitsOffline = 1


    TestIDPhysValMonTau.ntupleChainNames += ['HLT_tau.*idperf.*:InDetTrigTrackingxAODCnv_Tau_EFID']
    TestIDPhysValMonTau.releaseMetaData = d['nightly name'] + " " + d['nightly release'] + " " + d['date'] + " " + d['platform'] + " " + d['release']

    ToolSvc += TestIDPhysValMonTau
    list += ["TrigTestPhysValMon/TestIDPhysValMonTau"]



    ############### Cosmics ###############
    TestIDPhysValMonCosmic = TrigTestPhysValMon(name="TestIDPhysValMonCosmic")
    TestIDPhysValMonCosmic.OutputLevel = INFO

    try:
      TestIDPhysValMonCosmic.EnableLumi = False
    except:
      pass

    TestIDPhysValMonCosmic.buildNtuple = False
    TestIDPhysValMonCosmic.AnalysisConfig = "Tier0" #T0 Analysis
    # TestIDPhysValMonCosmic.AnalysisConfig = "nTuple" #nTuple Analysis

    TestIDPhysValMonCosmic.mcTruth = False
    TestIDPhysValMonCosmic.ntupleChainNames = ['Offline']
    TestIDPhysValMonCosmic.sctHitsOffline = -1
    TestIDPhysValMonCosmic.pixHitsOffline = -1


    TestIDPhysValMonCosmic.ntupleChainNames += ['HLT_.*id.*cosmic.*:InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID',
                                              'HLT_.*id.*cosmic.*:InDetTrigTrackingxAODCnv_CosmicsN_EFID']
    TestIDPhysValMonCosmic.releaseMetaData = d['nightly name'] + " " + d['nightly release'] + " " + d['date'] + " " + d['platform'] + " " + d['release']

    ToolSvc += TestIDPhysValMonCosmic
    list += ["TrigTestPhysValMon/TestIDPhysValMonCosmic"]


  return list
