# ScheduleCHSPFlowMods.py
#
# Teng Jian Khoo
# July 2017
#
# Apply PFlow p4 corrections (nPFO origin & cPFO weights) and
# Charged Hadron Subtraction with jet constituent modification tools
# * Checks first if the collection to be created exists in the file
# * Then checks if a correction algorithm was already added to the sequence
#   based on the algorithm name (only)

def scheduleCHSPFlowMods( job=None ):
    from JetRec.JetRecConf import JetToolRunner
    from RecExConfig.AutoConfiguration import IsInInputFile
    containerexists = IsInInputFile("xAOD::PFOContainer","CHSParticleFlowObjects")

    if job is None:
        from AthenaCommon.AlgSequence import AlgSequence
        job = AlgSequence()
    algexists = hasattr(job,"jetalgCHSPFlow")

    if not (containerexists or algexists):
        from JetRec.JetRecStandard import jtm
        jtm += JetToolRunner("jetconstitCHSPFlow",
                             EventShapeTools=[],
                             Tools=[jtm.JetConstitSeq_PFlowCHS],
                             )

        from JetRec.JetRecConf import JetAlgorithm
        job += JetAlgorithm("jetalgCHSPFlow",
                            Tools=[jtm.jetconstitCHSPFlow])
