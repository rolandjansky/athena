# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AthSequencer
from MuonCondAlg.MuonTopCondAlgConfigRUN2 import MdtCondDbAlg,RpcCondDbAlg,CscCondDbAlg,TgcCondDbAlg
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if not athenaCommonFlags.isOnline:
    condSequence = AthSequencer("AthCondSeq")
    if not hasattr(condSequence,"MdtCondDbAlg"):
        condSequence += MdtCondDbAlg("MdtCondDbAlg")
    if not hasattr(condSequence,"RpcCondDbAlg"):
        condSequence += RpcCondDbAlg("RpcCondDbAlg")
    if not hasattr(condSequence,"CscCondDbAlg"):
        condSequence += CscCondDbAlg("CscCondDbAlg")
## keep as template:
#    if not hasattr(condSequence,"TgcCondDbAlg"):
#        condSequence += TgcCondDbAlg("TgcCondDbAlg")

