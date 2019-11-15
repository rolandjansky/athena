"""Configure Truth output for digitization with ComponentAccumulator style

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg


def TruthDigitizationOutputCfg(flags):
    """Return ComponentAccumulator with Truth output items"""
    ItemList = [
        "McEventCollection#*",
        "TrackRecordCollection#*",
        "xAOD::JetContainer#*InTimeAntiKt4TruthJets",
        "xAOD::JetAuxContainer#*InTimeAntiKt4TruthJetsAux.",
        "xAOD::JetContainer#*OutOfTimeAntiKt4TruthJets",
        "xAOD::JetAuxContainer#*OutOfTimeAntiKt4TruthJetsAux.",
    ]
    return OutputStreamCfg(flags, "RDO", ItemList)
