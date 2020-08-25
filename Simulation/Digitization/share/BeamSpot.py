# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Always schedule beam spot conditions for digi
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr(condSeq, "BeamSpotCondAlg"):
    from IOVDbSvc.CondDB import conddb
    conddb.addFolderSplitOnline("INDET", "/Indet/Onl/Beampos", "/Indet/Beampos", className="AthenaAttributeList")
    from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg
    condSeq += BeamSpotCondAlg("BeamSpotCondAlg")

# Add beam spot fixer
from AthenaCommon import CfgGetter
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags
from Digitization.DigitizationFlags import digitizationFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags
if (DetFlags.pileup.any_on() or digitizationFlags.doXingByXingPileUp()) or (globalflags.isOverlay() and not overlayFlags.isOverlayMT()):
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    job += CfgGetter.getAlgorithm("BeamSpotFixerAlg")
