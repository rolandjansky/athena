# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def MuonPrepDataConvCfg(flags):
    result = ComponentAccumulator()
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
    for cont_type, cont_name in prepdata_container:
        if len([item for item in flags.Input.Collections if item == cont_name]) == 0: continue
        the_alg = CompFactory.AthReadAlg (f'AthReadAlg_{cont_name}',
                                          Key = f'{cont_type}/{cont_name}',
                                          Aliases = [],
                                          ExtraInputs = dependencies)
        result.addCondAlgo(the_alg)
    return result

