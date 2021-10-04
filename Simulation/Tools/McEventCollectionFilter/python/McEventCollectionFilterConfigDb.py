# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm

addAlgorithm("McEventCollectionFilter.McEventCollectionFilterConfigLegacy.getDecorateTruthPileupParticles", "DecorateTruthPileupParticles")
addAlgorithm("McEventCollectionFilter.McEventCollectionFilterConfigLegacy.getDecoratePileupAntiKt4TruthJets", "DecoratePileupAntiKt4TruthJets")
addAlgorithm("McEventCollectionFilter.McEventCollectionFilterConfigLegacy.getDecoratePileupAntiKt6TruthJets", "DecoratePileupAntiKt6TruthJets")
