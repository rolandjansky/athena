# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
# no need to create the CscCondDbAlg in case of no CSC chambers present in the layout
if MuonGeometryFlags.hasCSC():
    from AthenaCommon.AlgSequence import AthSequencer
    from MuonCondAlg.MuonTopCondAlgConfigRUN2 import CscCondDbAlg
    
    condSequence = AthSequencer("AthCondSeq")
    if not hasattr(condSequence,"CscCondDbAlg"): 
        condSequence += CscCondDbAlg("CscCondDbAlg")
