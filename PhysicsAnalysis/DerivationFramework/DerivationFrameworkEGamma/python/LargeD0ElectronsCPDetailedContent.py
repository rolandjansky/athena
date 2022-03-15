# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

ExtraLRTElectronShowerShapes = ["LRTElectrons.Rhad.Rhad1.e277.Reta.Rphi.weta2.f1.Eratio.DeltaE.weta1.fracs1.wtots1.f3.f3core.deltaEta1.deltaPhi1.deltaPhi2.deltaPhiRescaled2.deltaPhiFromLastMeasurement"]  
ExtraLRTElectronTruthInfo = ["LRTElectrons.lastEgMotherTruthType.lastEgMotherTruthOrigin.lastEgMotherTruthParticleLink.lastEgMotherPdgId"]
LRTElectronsCPDetailedContent = ExtraLRTElectronShowerShapes + ExtraLRTElectronTruthInfo

ExtraLRTElectronGSFVar = ["LRTGSFTrackParticles.parameterX.parameterPX.parameterPY.parameterPZ.parameterPosition.numberOfTRTHits.numberOfTRTOutliers.numberOfTRTHighThresholdHits.numberOfTRTHighThresholdOutliers.numberOfTRTXenonHits.eProbabilityComb.eProbabilityHT"]  
LRTGSFTracksCPDetailedContent = ExtraLRTElectronGSFVar

LRTElectronsAddAmbiguityContent = ["LRTElectrons.DFCommonSimpleConvRadius.DFCommonSimpleConvPhi.DFCommonSimpleMee.DFCommonSimpleMeeAtVtx.DFCommonSimpleSeparation.DFCommonProdTrueRadius.DFCommonProdTruePhi.DFCommonProdTrueZ"]

