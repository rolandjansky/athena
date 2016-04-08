include.block( "UserAnalysisUtils/UserAnalysisSelectionTool_jobOptions.py" )

from UserAnalysisUtils.UserAnalysisUtilsConf import \
     UserAnalysisSelectionTool as ConfiguredUserSelectionTool
UserAnalysisSelectionTool = ConfiguredUserSelectionTool(


  IsAtlfastData=False, # set this to true if running on Atlfast AOD - some of the cuts are not used in this case

  CaloClusterE=5.0*GeV,

  TrackParticlePt=1.0*GeV,

  ElectronPt=7.0*GeV,
  ElectronEta=2.5,
  ElectronIsEMFlag="Medium", #Loose/Medium/Tight
  ElectronIsEM=0xffffff,           #or set the IsEM value yourself
  AuthorEgammaOnly=True,
  ElectronEtaWindowCut=False,
  ElectronEtaWindowCenter=1.445,
  ElectronEtaWindow=0.075,
  DoElectronIsolation=True,
  ElectronIsolationConeIndex=3, #0-4 for dR=0.1, 0.2, 0.3, and 0.4
  ElectronIsolationEt=10.0*GeV,
  NormalizedElectronIsolationEt=0.2,

  PhotonPt=10.0*GeV,
  PhotonEta=3.2,
  PhotonIsEM=0,

  MuonPt=6.0*GeV,
  MuonEta=2.7,
  DoMuonIsolation=True,
  MuonIsolationConeIndex=3, # 0-4 for dR=0.1, 0.2, 0.3, and 0.4
  MuonIsolationEt=10*GeV,
  NormalizedMuonIsolationEt=0.2,
  UseMatchChi2=True, 
  MuonMatchChi2=100,

  TauJetPt=20*GeV,
  TauJetEta=2.5,
  TauJetLikelihood=-6.0,
  TauEleBDTCut=0.5, #

  JetPt=20*GeV,
  JetEta=5.0,
  BJetLikelihood=6.0

    )

ToolSvc += UserAnalysisSelectionTool
print      UserAnalysisSelectionTool
