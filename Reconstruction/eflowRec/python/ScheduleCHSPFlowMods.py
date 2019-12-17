#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
# ScheduleCHSPFlowMods.py
#
# Teng Jian Khoo
# July 2017
#
# Apply PFlow p4 corrections (nPFO origin & cPFO weights) and
# Charged Hadron Subtraction with jet constituent modification tools


from JetRec.JetRecStandard import jtm
from AthenaConfiguration.ComponentFactory import CompFactory
JetToolRunner=CompFactory.JetToolRunner
jtm += JetToolRunner("jetconstitCHSPFlow",
                     EventShapeTools=[],
                     Tools=[jtm.JetConstitSeq_PFlowCHS],
                     )

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

JetAlgorithm=CompFactory.JetAlgorithm
job += JetAlgorithm("jetalgCHSPFlow",
                    Tools=[jtm.jetconstitCHSPFlow])

ThinNegativeEnergyNeutralPFOsAlg=CompFactory.ThinNegativeEnergyNeutralPFOsAlg
CHSnPFOsThinAlg = ThinNegativeEnergyNeutralPFOsAlg(
    "ThinNegativeEnergyNCHSeutralPFOsAlg",
    NeutralPFOsKey="CHSNeutralParticleFlowObjects",
    ThinNegativeEnergyNeutralPFOs = True
)
job += CHSnPFOsThinAlg
