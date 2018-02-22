# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT14SmartContent = [
  "TauJets",
  "MET_Reference_AntiKt4EMTopo",
  "MET_Reference_AntiKt4EMPFlow",
	"PrimaryVertices",
	"Electrons",
  "Muons",
	"InDetTrackParticles",
	"AntiKt4EMTopoJets",
  "AntiKt4EMPFlowJets",
  "BTagging_AntiKt4EMTopo",
  "BTagging_AntiKt4EMPFlow",
  "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET"
]

EXOT14AllVariablesContent = [
	"GSFTrackParticles",
	"egammaClusters",
	"CaloCalTopoClusters",
  "GSFConversionVertices",
  "METAssoc_AntiKt4EMTopo",
  "MET_Core_AntiKt4EMTopo",
  "LVL1JetRoIs",
  "LVL1MuonRoIs",
  "LVL1EmTauRoIs",
  "MET_Truth",
  "TruthEvents",
  "TruthParticles",
  "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht",
  "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl",
  "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PS",
  "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC",
  "MET_Track",
  "AntiKt4PV0TrackJets"
]

EXOT14ExtraVariablesContent = [
  "Electrons.author.pt.Medium.Tight.Loose.charge",
  "Muons.ptcone20.ptcone30.ptcone40.etcone20.etcone30.etcone40.InnerDetectorPt.extrapolatedMuonSpectrometerTrackParticleLink.charge.ptvarcone30.topoetcone20",
  "CombinedMuonTrackParticles.d0.z0.phi.theta.charge.chiSquared.numberDoF.qOverP.vz.definingParametersCovMatrix", # SUSYTools
  "ExtrapolatedMuonTrackParticles.d0.z0.phi.theta.charge.chiSquared.numberDoF.qOverP.vz.definingParametersCovMatrix", # SUSYTools
  "AntiKt4TruthJets.pt.eta.phi.m",
  "AntiKt4EMTopoJets.Timing",
  "AntiKt4EMPFlowJets.Timing"
]

EXOT14Content = []
