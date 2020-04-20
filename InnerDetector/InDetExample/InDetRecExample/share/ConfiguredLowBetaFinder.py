# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredLowBetaFinder.py')

# ------------------------------------------------------------------------------------
#
# ----------- Low beta finder
#
# ------------------------------------------------------------------------------------

class ConfiguredLowBetaFinder:

    def __init__(self, InputCollection=None, IsSimulation=False, unslimmed_tracks_container="CombinedInDetTracks"):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        from InDetRecExample.TrackingCommon import getInDetTRT_dEdxTool
        from InDetLowBetaFinder.InDetLowBetaFinderConf import InDet__LowBetaAlg
        InDetLowBetaTrkAlgorithm = InDet__LowBetaAlg( name                   = "InDetLowBetaTrkAlgorithm",
                                                      MinimumTRThitsForIDpid = 5,
                                                      TracksName             = InputCollection,
                                                      UnslimmedTracksContainer = unslimmed_tracks_container,
                                                      InDetLowBetaOutputName = InDetKeys.LowBetaCandidates(),
                                                      CSMP_Rcorrection_Zero  = 11.0,
                                                      CSMP_Rcorrection_One   = -3.0,
                                                      CSMP_Rcorrection_Two   = 0.0,
                                                      CSMP_TimingOffset      =  0.0,
                                                      TRT_ToT_dEdx_Tool      = getInDetTRT_dEdxTool(),
                                                      MC_flag                = IsSimulation)

        topSequence += InDetLowBetaTrkAlgorithm
        if (InDetFlags.doPrintConfigurables()):
            printfunc (InDetLowBetaTrkAlgorithm)
