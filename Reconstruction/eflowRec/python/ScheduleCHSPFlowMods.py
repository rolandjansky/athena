# ScheduleCHSPFlowMods.py
#
# David Adams
# March 2014
# October 2014: Update to provide a fn that allow specification of alg sequence.
#
# Configure the jet algorithm after the tool manager has been configured.


from JetRec.JetRecStandard import jtm
jtm += JetToolRunner("jetconstitCHSPFlow",
                     EventShapeTools=[],
                     Tools=[jtm.JetConstitSeq_PFlowCHS],
                     )

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from JetRec.JetRecConf import JetAlgorithm
job += JetAlgorithm("jetalgCHSPFlow",
                    Tools=jtm.jetconstitCHSPFlow)
