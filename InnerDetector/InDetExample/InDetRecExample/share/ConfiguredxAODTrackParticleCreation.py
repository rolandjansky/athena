# Blocking the include for after first inclusion
include.block ('InDetRecExample/ConfiguredxAODTrackParticleCreation.py')

# ------------------------------------------------------------
#
# ----------- Setup InDet xAOD::TrackParticle Creation
#
# ------------------------------------------------------------

class ConfiguredxAODTrackParticleCreation:

     def __init__(self, InputTrackCollection = None, InputTrackTruthCollection = None, OutputTrackParticleContainer = None):


         from InDetRecExample.InDetJobProperties import InDetFlags
         from InDetRecExample.InDetKeys import InDetKeys
         #
         # --- get ToolSvc and topSequence
         #
         from AthenaCommon.AppMgr                import ToolSvc
         from AthenaCommon.AlgSequence           import AlgSequence
         topSequence = AlgSequence()

         #Always the same (so far) so can in principle go in InDetRecLoadTools
         from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
         InDetxAODParticleCreatorTool = Trk__TrackParticleCreatorTool(name = "InDetxAODParticleCreatorTool"+InputTrackCollection, 
                                                                      Extrapolator            = InDetExtrapolator,
                                                                      TrackSummaryTool        = InDetTrackSummaryToolSharedHits,
                                                                      BadClusterID            = InDetFlags.pixelClusterBadClusterID(),
                                                                      ForceTrackSummaryUpdate = False,
                                                                      KeepParameters          = True)

         ToolSvc += InDetxAODParticleCreatorTool
         if (InDetFlags.doPrintConfigurables()):
            print InDetxAODParticleCreatorTool

         from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
         xAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(name = "InDetxAODParticleCreatorAlg"+InputTrackCollection,
                                                                  ConvertTracks = True,
                                                                  ConvertTrackParticles = False,
                                                                  TrackContainerName = InputTrackCollection,
                                                                  xAODContainerName = OutputTrackParticleContainer,
                                                                  xAODTrackParticlesFromTracksContainerName = OutputTrackParticleContainer,
                                                                  TrackParticleCreator = InDetxAODParticleCreatorTool,
                                                                  PrintIDSummaryInfo = True)

         if (InDetFlags.doTruth() and not InputTrackTruthCollection == ''):
             xAODTrackParticleCnvAlg.AddTruthLink = True
             xAODTrackParticleCnvAlg.TrackTruthContainerName = InputTrackTruthCollection
         elif (InDetFlags.doTruth() and InputTrackTruthCollection == ''):
             print "WARNING: ConfiguredxAODTrackParticleCreation - doTruth = True, but no input Truth collection specified!"
         else:
            xAODTrackParticleCnvAlg.AddTruthLink = False

         topSequence += xAODTrackParticleCnvAlg
         if (InDetFlags.doPrintConfigurables()):
            print xAODTrackParticleCnvAlg
