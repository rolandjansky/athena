# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

ExtraElectronShowerShapes = ["Electrons.Rhad.Rhad1.e277.Reta.Rphi.weta2.f1.Eratio.DeltaE.weta1.fracs1.wtots1.f3.f3core.deltaEta1.deltaPhi1.deltaPhi2.deltaPhiRescaled2.deltaPhiFromLastMeasurement"]  
ExtraElectronTruthInfo = ["Electrons.lastEgMotherTruthType.lastEgMotherTruthOrigin.lastEgMotherTruthParticleLink.lastEgMotherPdgId"]
ElectronsCPDetailedContent = ExtraElectronShowerShapes + ExtraElectronTruthInfo

ExtraElectronGSFVar = ["GSFTrackParticles.parameterX.parameterPX.parameterPY.parameterPZ.parameterPosition.numberOfTRTHits.numberOfTRTOutliers.numberOfTRTHighThresholdHits.numberOfTRTHighThresholdOutliers.numberOfTRTXenonHits.eProbabilityComb.eProbabilityHT"]  
GSFTracksCPDetailedContent = ExtraElectronGSFVar

ElectronsAddAmbiguityContent = ["Electrons.DFCommonSimpleConvRadius.DFCommonSimpleConvPhi.DFCommonSimpleMee.DFCommonSimpleMeeAtVtx.DFCommonSimpleSeparation.DFCommonProdTrueRadius.DFCommonProdTruePhi.DFCommonProdTrueZ"]

