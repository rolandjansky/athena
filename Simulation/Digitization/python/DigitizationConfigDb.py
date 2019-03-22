# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addService, addAlgorithm
addService("Digitization.PileUpConfig.getStepArrayBM"                , "StepArrayBM")
addService("Digitization.PileUpConfig.getFixedArrayBM"               , "FixedArrayBM")
addService("Digitization.PileUpConfig.getArrayBM"                    , "ArrayBM")

addService("Digitization.PileUpConfig.getLowPtMinBiasEventSelector"  , "LowPtMinBiasEventSelector")
addService("Digitization.PileUpConfig.getHighPtMinBiasEventSelector" , "HighPtMinBiasEventSelector")
addService("Digitization.PileUpConfig.getcavernEventSelector"        , "cavernEventSelector")
addService("Digitization.PileUpConfig.getBeamGasEventSelector"       , "BeamGasEventSelector")
addService("Digitization.PileUpConfig.getBeamHaloEventSelector"      , "BeamHaloEventSelector")

addTool("Digitization.PileUpConfig.getMinBiasCache"                  , "MinBiasCache")
addTool("Digitization.PileUpConfig.getLowPtMinBiasCache"             , "LowPtMinBiasCache")
addTool("Digitization.PileUpConfig.getHighPtMinBiasCache"            , "HighPtMinBiasCache")
addTool("Digitization.PileUpConfig.getCavernCache"                   , "CavernCache")
addTool("Digitization.PileUpConfig.getBeamGasCache"                  , "BeamGasCache")
addTool("Digitization.PileUpConfig.getBeamHaloCache"                 , "BeamHaloCache")

addTool("Digitization.DigiAlgConfig.getTestPileUpTool"               , "TestPileUpTool")
addTool("Digitization.DigiAlgConfig.getTestFilterPileUpTool"         , "TestFilterPileUpTool")

addService("Digitization.RunDependentConfig.getLumiProfileSvc"       , "LumiProfileSvc")
addService("Digitization.RunDependentConfig.getNoProfileSvc"         , "NoProfileSvc")
addService("Digitization.RunDependentConfig.getEvtIdModifierSvc"     , "EvtIdModifierSvc")

addService("Digitization.PileUpConfig.getPileUpEventLoopMgr"         , "PileUpEventLoopMgr")

addAlgorithm("Digitization.DigiAlgConfig.getStandardPileUpToolsAlg"  , "StandardPileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getFastPileUpToolsAlg"      , "FastPileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getSplitPileUpToolsAlg"     , "SplitPileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getSplitSFPileUpToolsAlg"  , "SplitSFPileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getStandardSignalOnlyTruthPileUpToolsAlg"  , "StandardSignalOnlyTruthPileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getStandardInTimeOnlyTruthPileUpToolsAlg"  , "StandardInTimeOnlyTruthPileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getStandardInTimeOnlyGeantinoTruthPileUpToolsAlg"  , "StandardInTimeOnlyGeantinoTruthPileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getSplitNoMergePileUpToolsAlg"  , "SplitNoMergePileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getSplitNoMergeSFPileUpToolsAlg"  , "SplitNoMergeSFPileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getSplitNoMergeFSPileUpToolsAlg"  , "SplitNoMergeFSPileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getSplitNoMergeFFPileUpToolsAlg"  , "SplitNoMergeFFPileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getTestPileUpToolsAlg"      , "TestPileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getTestFilterPileUpToolsAlg"      , "TestFilterPileUpToolsAlg")
addAlgorithm("Digitization.DigiAlgConfig.getTestTruthJetFilterPileUpToolsAlg" , "TestTruthJetFilterPileUpToolsAlg")

addService("Digitization.PileUpMergeSvcConfig.getPileUpMergeSvc"     , "PileUpMergeSvc")
