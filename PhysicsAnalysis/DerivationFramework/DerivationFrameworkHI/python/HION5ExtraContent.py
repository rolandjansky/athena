# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

ExtraContentElectrons = [
  "Electrons.etcone30",
  "Electrons.etcone40"
]

ExtraContentPhotons = [
  "Photons.ecore",
  "Photons.etcone20",
  "Photons.etcone30",
  "Photons.etcone40",
  "Photons.core57cellsEnergyCorrection",
  "Photons.topoetconecoreConeEnergyCorrection"
]

#ExtraContentJets = [
#  "AntiKt2HIJets.SumPtTrkPt4000",
#  "AntiKt3HIJets.SumPtTrkPt4000",
#  "AntiKt4HIJets.SumPtTrkPt4000"
#]

ExtraContentPrimaryVertices = [
  "PrimaryVertices.x",
  "PrimaryVertices.y",
  "PrimaryVertices.sumPt2"
]

ExtraContentInDetTrackParticles = [
  "InDetTrackParticles.truthMatchProbability"
]

ExtraContentGSFTrackParticles = [
  "GSFTrackParticles.numberOfTRTHighThresholdHitsTotal",
  "GSFTrackParticles.truthMatchProbability"
]


#ExtraContentAll = ExtraContentElectrons + ExtraContentPhotons + ExtraContentJets + ExtraContentPrimaryVertices + ExtraContentInDetTrackParticles + ExtraContentGSFTrackParticles
ExtraContentAll = ExtraContentElectrons + ExtraContentPhotons + ExtraContentPrimaryVertices + ExtraContentInDetTrackParticles + ExtraContentGSFTrackParticles
