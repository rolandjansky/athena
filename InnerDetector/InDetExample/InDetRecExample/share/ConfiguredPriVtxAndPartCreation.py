# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredPriVtxAndPartCreation.py')

# ------------------------------------------------------------------------------------
#
# ----------- Configures primary vertexing creation
#-------(xAOD TrackParticle creation now in InDetxAODCreator.py) 
#
# ------------------------------------------------------------------------------------

class ConfiguredPriVtxAndPartCreation:

  def __init__(self,
               extension                 = "",
               InputTrackCollection      = None, 
               InputTrackCollectionTruth = None, 
               VxCandidates              = None,
               TrackParticles            = None,
               TrackParticlesTruth       = None):
               
    # get ToolSvc and topSequence
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    # get InDetFlags
    from InDetRecExample.InDetJobProperties import InDetFlags

    # ------------------------------------------------------------
    #
    #  --- now load primary vertex finder
    #
    # ------------------------------------------------------------
    if InDetFlags.doVertexFinding() and not extension == "ForwardTracks" and not extension == "ForwardSLHCTracks" and not extension == "VeryForwardSLHCTracks":
    
      if InDetFlags.primaryVertexSetup() == 'DummyVxFinder':
        from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxDummyFinder
        InDetPriVxFinder = InDet__InDetPriVxDummyFinder(name                        = "InDet"+extension+"PriVxDummyFinder",
                                                        VxCandidatesOutputName      = VxCandidates)
      else:

        # the loading of all tools for the primary vertex finding has been moved to InDetRecLoadTools.py
        from InDetPriVxFinder.InDetPriVxFinderConf import InDet__InDetPriVxFinder
        InDetPriVxFinder = InDet__InDetPriVxFinder(name                        = "InDet"+extension+"PriVxFinder",
                                                   VertexFinderTool            = InDetPriVxFinderTool,
                                                   TracksName                  = InputTrackCollection,
                                                   VxCandidatesOutputName      = VxCandidates,
                                                   VertexCollectionSortingTool = VertexCollectionSortingTool,
                                                   InternalEdmFactory          = InDetVxEdmCnv)

        if InDetFlags.primaryVertexSortingSetup() == 'NoReSorting':
          InDetPriVxFinder.doVertexSorting = False
        else:
          InDetPriVxFinder.doVertexSorting = True

      topSequence += InDetPriVxFinder
      if InDetFlags.doPrintConfigurables():
        print InDetPriVxFinder



