#
# === configuration of tools/algs done BEFORE combined muon identification,
# === such as container initialisation, track splitting and ID track extrapolation
#
include.block ("MuonCombinedRecExample/MuonCombinedRec_preprocessing.py")
from AthenaCommon import CfgMgr
#
# Muon Calo Energy Container is needed even if Calo is Off: use the parametrized energy loss in that case - KAA
if rec.doMuonCombined() and muonCombinedRecFlags.doAnyMuons() and DetFlags.Muon_on():
    # Needed by MuonIsolationTools - whether calo is ON or OFF. In calo is Off use parametrized energy loss - KAA
  
    InitializeMuonCaloEnergy = CfgMgr.Rec__InitializeMuonCaloEnergy(
        name                    = "InitializeMuonCaloEnergy",
        MuonCaloEnergyContainer = "MuonCaloEnergyCollection"
        )
    topSequence += InitializeMuonCaloEnergy
    if muonCombinedRecFlags.printConfigurables():
        print InitializeMuonCaloEnergy

if rec.doMuonCombined() and muonCombinedRecFlags.doMuonClusters() and ( rec.readAOD() or rec.readESD() or DetFlags.haveRIO.Calo_on() ):
    # hack until MuonClusterCollection is properly added to ObjKeyStore
    # Needed by CaloCellAODGetter.addClusterToCaloCellAOD()
    if jobproperties.Beam.beamType()!='cosmics' and jobproperties.Beam.beamType()!='singlebeam':
       from RecExConfig.ObjKeyStore import objKeyStore
       objKeyStore.addStreamAOD("CaloClusterContainer","MuonClusterCollection")


if rec.doMuonCombined() and jobproperties.Beam.beamType()=='cosmics' and DetFlags.haveRIO.ID_on() and \
       InDetFlags.Enabled() and not InDetFlags.disableInDetReco():
    from InDetTrackSplitterTool.InDetTrackSplitterToolConf import InDet__InDetTrackSplitterTool
    splittertool= InDet__InDetTrackSplitterTool(name='InDetTrackSplitterToolForMuonCombined',
                                                TrackFitter=ToolSvc.InDetTrackFitter)
    ToolSvc += splittertool

    from InDetTrackValidation.InDetTrackValidationConf import InDet__InDetSplittedTracksCreator
    splitter=InDet__InDetSplittedTracksCreator(name='InDetTrackSplitterForMuonCombined',
                                               TrackSplitterTool     = splittertool,
                                               TrackCollection       = InDetKeys.Tracks(),
                                               OutputTrackCollection = InDetKeys.Tracks()+"_split")

    topSequence+=splitter

    from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
    InDetParticleCreatorTool_split = Trk__TrackParticleCreatorTool(name              = "InDetParticleCreatorTool_split",
                                                                   KeepParameters    = True,
                                                                   Extrapolator      = InDetExtrapolator,
                                                                   TrackSummaryTool  = InDetTrackSummaryTool,
                                                                   PerigeeExpression = "Origin")
    ToolSvc += InDetParticleCreatorTool_split

    output_track_particle_name = InDetKeys.TrackParticles()+"_split"
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
    xAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(name = "InDetxAODParticleCreatorAlg"+InputTrackCollection+"_split",
                                                             ConvertTracks = True,
                                                             ConvertTrackParticles = False,
                                                             TrackContainerName = InDetKeys.Tracks()+"_split",
                                                             xAODContainerName = output_track_particle_name,
                                                             xAODTrackParticlesFromTracksContainerName = output_track_particle_name,
                                                             TrackParticleCreator = InDetParticleCreatorTool_split,
                                                             PrintIDSummaryInfo = True)

    if (InDetFlags.doTruth() and not InputTrackTruthCollection == ''):
        xAODTrackParticleCnvAlg.AddTruthLink = True
        xAODTrackParticleCnvAlg.TrackTruthContainerName = InputTrackTruthCollection
    elif (InDetFlags.doTruth() and InputTrackTruthCollection == ''):
        print "WARNING: ConfiguredxAODTrackParticleCreation - doTruth = True, but no input Truth collection specified!"
    else:
        xAODTrackParticleCnvAlg.AddTruthLink = False

    topSequence += xAODTrackParticleCnvAlg

    #truth tracks
    if rec.doTruth():
        include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
        
        InDetTracksTruth = ConfiguredInDetTrackTruth(InDetKeys.Tracks()+"_split",
                                                     InDetKeys.DetailedTracksTruth()+"_split",
                                                     InDetKeys.Tracks()+"_Truth_split")

