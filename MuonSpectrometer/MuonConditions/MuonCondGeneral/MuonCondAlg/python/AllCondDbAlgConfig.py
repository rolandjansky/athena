# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AthSequencer
from MuonCondAlg.MuonTopCondAlgConfigRUN2 import MdtCondDbAlg,RpcCondDbAlg
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

condSequence = AthSequencer("AthCondSeq")
if not athenaCommonFlags.isOnline:
    if not hasattr(condSequence,"MdtCondDbAlg"):
        condSequence += MdtCondDbAlg("MdtCondDbAlg")
    if not hasattr(condSequence,"RpcCondDbAlg"):
        condSequence += RpcCondDbAlg("RpcCondDbAlg")

## keep as template:
#    if not hasattr(condSequence,"TgcCondDbAlg"):
#        condSequence += TgcCondDbAlg("TgcCondDbAlg")

# no need to create the CscCondDbAlg in case of no CSC chambers present in the layout
if MuonGeometryFlags.hasCSC():
    from MuonCondAlg.MuonTopCondAlgConfigRUN2 import CscCondDbAlg
    if not hasattr(condSequence,"CscCondDbAlg"):
        condSequence += CscCondDbAlg("CscCondDbAlg")

