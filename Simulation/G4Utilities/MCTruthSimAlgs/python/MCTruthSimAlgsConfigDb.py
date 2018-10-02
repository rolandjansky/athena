# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool

addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.getTrackRange"                  , "TrackRange" )
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.MergeTrackRecordCollTool"       , "MergeTrackRecordCollTool")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.MergeCaloEntryLayerTool"        , "MergeCaloEntryLayerTool")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.MergeMuonEntryLayerTool"        , "MergeMuonEntryLayerTool")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.MergeMuonExitLayerTool"         , "MergeMuonExitLayerTool")

addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.getCalibRange"                  , "CalibRange")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.MergeCalibHitsTool"             , "MergeCalibHitsTool")

addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.getTruthJetRange"               , "TruthJetRange")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.getMergeTruthJetsTool"          , "MergeTruthJetsTool")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.getMergeTruthJetsFilterTool"    , "MergeTruthJetsFilterTool")

addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.getTimingObjRange"              , "TimingObjRange")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.MergeRecoTimingObjTool"         , "MergeRecoTimingObjTool")

addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.MergeHijingParsTool"            , "MergeHijingParsTool")

addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.MergeMcEventCollTool"           , "MergeMcEventCollTool")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.getNewMergeMcEventCollTool_Signal"        , "NewMergeMcEventCollTool_Signal")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.getNewMergeMcEventCollTool_MinBias"        , "NewMergeMcEventCollTool_MinBias")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.getNewMergeMcEventCollTool_HighPtMinBias"        , "NewMergeMcEventCollTool_HighPtMinBias")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.getNewMergeMcEventCollTool_Cavern"        , "NewMergeMcEventCollTool_Cavern")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.getNewMergeMcEventCollTool_HaloGas"        , "NewMergeMcEventCollTool_HaloGas")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.NewMergeMcEventCollTool"        , "InTimeOnlyNewMergeMcEventCollTool_MinBias")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.NewMergeMcEventCollTool"        , "InTimeOnlyNewMergeMcEventCollTool_HighPtMinBias")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.NewMergeMcEventCollTool"        , "InTimeOnlyNewMergeMcEventCollTool_Cavern")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.NewMergeMcEventCollTool"        , "InTimeOnlyNewMergeMcEventCollTool_HaloGas")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.SignalOnlyMcEventCollTool"      , "SignalOnlyMcEventCollTool")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.InTimeOnlyMcEventCollTool"      , "InTimeOnlyMcEventCollTool")

addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.MergeGenericMuonSimHitCollTool" , "MergeGenericMuonSimHitCollTool")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.MergeMicromegasSimHitCollTool"  , "MergeMicromegasSimHitCollTool")
addTool("MCTruthSimAlgs.MCTruthSimAlgsConfig.MergeSTGCSimHitCollTool"        , "MergeSTGCSimHitCollTool")
