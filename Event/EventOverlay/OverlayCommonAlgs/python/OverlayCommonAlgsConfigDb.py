# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addAlgorithm

addAlgorithm("OverlayCommonAlgs.OverlayCommonAlgsConfig.getCopyCaloCalibrationHitContainer", "CopyCaloCalibrationHitContainer")
addAlgorithm("OverlayCommonAlgs.OverlayCommonAlgsConfig.getCopyInTimeJetTruthInfo", "CopyInTimeJetTruthInfo")
addAlgorithm("OverlayCommonAlgs.OverlayCommonAlgsConfig.getCopyOutOfTimeJetTruthInfo", "CopyOutOfTimeJetTruthInfo")
addAlgorithm("OverlayCommonAlgs.OverlayCommonAlgsConfig.getCopyMcEventCollection", "CopyMcEventCollection")
addAlgorithm("OverlayCommonAlgs.OverlayCommonAlgsConfig.getCopyTimings", "CopyTimings")
addAlgorithm("OverlayCommonAlgs.OverlayCommonAlgsConfig.getCopyTrackRecordCollection", "CopyTrackRecordCollection")
