#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
# ScheduleCHSPFlowMods.py
#
# Teng Jian Khoo
# July 2017
#
# Apply PFlow p4 corrections (nPFO origin & cPFO weights) and
# Charged Hadron Subtraction with jet constituent modification tools


from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetToolRunner
from JetRec.JetRecFlags import jetFlags
if jetFlags.useTrackVertexTool:
    jtm += JetToolRunner("jetconstitCHSPFlow",
                             EventShapeTools=[],
                             Tools=[jtm.ttvaassocNew,jtm.JetConstitSeq_PFlowCHS],
                             )
else:
    jtm += JetToolRunner("jetconstitCHSPFlow",
                             EventShapeTools=[],
                             Tools=[jtm.JetConstitSeq_PFlowCHS],
                             )

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from JetRec.JetRecConf import JetAlgorithm
job += JetAlgorithm("jetalgCHSPFlow",
                    Tools=[jtm.jetconstitCHSPFlow])

from ThinningUtils.ThinningUtilsConf import ThinNegativeEnergyNeutralPFOsAlg
CHSnPFOsThinAlg = ThinNegativeEnergyNeutralPFOsAlg(
    "ThinNegativeEnergyNCHSeutralPFOsAlg",
    NeutralPFOsKey="CHSNeutralParticleFlowObjects",
    ThinNegativeEnergyNeutralPFOs = True,
    StreamName = 'StreamAOD'
)
job += CHSnPFOsThinAlg
