# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AthSequencer
from MuonCondAlg.MuonTopCondAlgConfigRUN2 import MdtCondDbAlg,RpcCondDbAlg,CscCondDbAlg,TgcCondDbAlg

condSequence = AthSequencer("AthCondSeq")
if not hasattr(condSequence,"MdtCondDbAlg"):
    condSequence += MdtCondDbAlg("MdtCondDbAlg")
if not hasattr(condSequence,"RpcCondDbAlg"):
    condSequence += RpcCondDbAlg("RpcCondDbAlg")
#if not hasattr(condSequence,"CscCondDbAlg"):
#    condSequence += CscCondDbAlg("CscCondDbAlg")
#if not hasattr(condSequence,"TgcCondDbAlg"):
#    condSequence += TgcCondDbAlg("TgcCondDbAlg")

