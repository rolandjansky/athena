# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredLowBetaFinder.py')

# ------------------------------------------------------------------------------------
#
# ----------- Low beta finder
#
# ------------------------------------------------------------------------------------

class ConfiguredLowBetaFinder:

    def __init__(self, InputCollection=None, IsSimulation=False):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        from InDetLowBetaFinder.InDetLowBetaFinderConf import InDet__LowBetaAlg
        InDetLowBetaTrkAlgorithm = InDet__LowBetaAlg( name                   = "InDetLowBetaTrkAlgorithm",
                                                      MinimumTRThitsForIDpid = 5,
                                                      TracksName             = InputCollection,
                                                      InDetLowBetaOutputName = InDetKeys.LowBetaCandidates(),
                                                      CSMP_Rcorrection_Zero  = 11.0,
                                                      CSMP_Rcorrection_One   = -3.0,
                                                      CSMP_Rcorrection_Two   = 0.0,
                                                      CSMP_TimingOffset      =  0.0,
                                                      MC_flag                = IsSimulation)
        
        topSequence += InDetLowBetaTrkAlgorithm
        if (InDetFlags.doPrintConfigurables()):
            print InDetLowBetaTrkAlgorithm
