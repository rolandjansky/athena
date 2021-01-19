# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Always schedule beam spot conditions for digi
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr(condSeq, "BeamSpotCondAlg"):
    from IOVDbSvc.CondDB import conddb
    conddb.addFolderSplitOnline("INDET", "/Indet/Onl/Beampos", "/Indet/Beampos", className="AthenaAttributeList")
    from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg
    condSeq += BeamSpotCondAlg("BeamSpotCondAlg")

# Add beam spot size reweighting algorithm
job = AlgSequence()
job += CfgGetter.getAlgorithm("BeamSpotReweightingAlg")
    

