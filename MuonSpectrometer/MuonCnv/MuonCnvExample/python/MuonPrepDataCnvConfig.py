# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from RecExConfig.ObjKeyStore import cfgKeyStore
from AthenaCommon.AlgSequence import  AthSequencer
condSeq = AthSequencer("AthCondSeq")
dependencies = [('MuonGM::MuonDetectorManager', 'ConditionStore+MuonDetectorManager')]
prepdata_container = [
    ("Muon::RpcPrepDataContainer", "RPC_Measurements"),
    ("Muon::TgcPrepDataContainer", "TGC_MeasurementsAllBCs"),
    ("Muon::MdtPrepDataContainer", "MDT_DriftCircles"),
    ("Muon::CscStripPrepDataContainer", "CSC_Measurements"),
    ("Muon::CscPrepDataContainer", "CSC_Clusters"),
    ("Muon::MMPrepDataContainer", "MM_Measurements"),
    ("Muon::sTgcPrepDataContainer", "STGC_Measurements"),
]

for cont_type, cont_key in prepdata_container:
    if not cfgKeyStore.isInInput(type=cont_type , key=cont_key):
        continue
    condSeq += CfgMgr.AthReadAlg("AthReadAlg_"+cont_key,
                                 Key="%s/%s"%(cont_type, cont_key),
                                 ExtraInputs = dependencies)


