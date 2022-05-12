# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def ThinNegativeEnergyNeutralPFOCfg(flags, StreamName="StreamAOD"):
    acc = ComponentAccumulator()

    thinAlg = CompFactory.ThinNegativeEnergyNeutralPFOsAlg(
        "ThinNegativeEnergyNeutralPFOsAlg",
        ThinNegativeEnergyNeutralPFOs = True,
        StreamName = StreamName,
        LCNeutralPFOsKey = "JetETMissLCNeutralParticleFlowObjects",
    )

    acc.addEventAlgo( thinAlg )
    
    return acc
