include.block('FTK_RecExample/FTKRec_jobOptions.py')

from FTK_RecExample.FTKJobProperties import FTKFlags
FTKFlags.init()

from RecExConfig.RecFlags import rec
from AthenaCommon.BeamFlags import jobproperties

from FTK_RecExample.FTKJobProperties import FTKFlags
FTKFlags.init()

if rec.doFTK() and (globalflags.InputFormat() == 'bytestream' or rec.readRDO):
    from AthenaCommon.GlobalFlags import GlobalFlags
    if rec.doFTK() and globalflags.InputFormat() == 'bytestream':
        ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc")
        ByteStreamAddressProviderSvc.TypeNames += [ "FTK_RawTrackContainer/FTK_RDO_Tracks"]

    from TrigFTK_RecExample.TrigFTKLoadTools import theFTK_DataProviderSvc

    from FTK_RecExample.FTKJobProperties import FTKFlags
    if FTKFlags.doSmearing:
        theFTKrandomSvc = AtRanluxGenSvc("FTKFastSim_Smearing")
        theFTKrandomSvc.EventReseeding = True
        theFTKrandomSvc.UseOldBrokenSeeding = False
        ServiceMgr += theFTKrandomSvc
    
    from TrigFTK_RawDataAlgs.TrigFTK_RawDataAlgsConf import FTK_RDO_ReaderAlgo

    FTK_RDO_Reader = FTK_RDO_ReaderAlgo( "FTK_RDO_ReaderAlgo")
    FTK_RDO_Reader.FTK_DataProvider=theFTK_DataProviderSvc
    FTK_RDO_Reader.RDO_CollectionName="FTK_RDO_Tracks"
    FTK_RDO_Reader.fillHists=False
    FTK_RDO_Reader.fillTree=False
    FTK_RDO_Reader.GetTracks=True
    FTK_RDO_Reader.GetTrackParticles=True
    FTK_RDO_Reader.GetRefitTracks=True
    FTK_RDO_Reader.GetRefitTrackParticles=True
    if (jobproperties.Beam.beamType() == 'cosmics'):
        FTK_RDO_Reader.GetVertex=False
        FTK_RDO_Reader.GetRefitVertex=False
    else:
        FTK_RDO_Reader.GetVertex=True
        FTK_RDO_Reader.GetRefitVertex=True
    FTK_RDO_Reader.GetTruthVertex=False

    from AthenaCommon.AlgSequence import AlgSequence

    alg = AlgSequence() 
    alg += FTK_RDO_Reader 
    

    if FTKFlags.doTruthLinks() and rec.doTruth() and (rec.doWriteAOD() or rec.doWriteESD()):
        include ('FTK_RecExample/ConfiguredFTK_TrackTruth.py')
        FTK_TracksTruth = ConfiguredFTK_TrackTruth(Tracks="FTK_TrackCollection",
                                                TracksTruth = "FTK_Tracks_TruthCollection",
                                                DetailedTruth = "FTK_Tracks_DetailedTruthCollection")
        FTK_RefitTracksTruth = ConfiguredFTK_TrackTruth(Tracks="FTK_TrackCollectionRefit", 
                                                     TracksTruth = "FTK_RefitTracks_TruthCollection",
                                                     DetailedTruth = "FTK_RefitTracks_DetailedTruthCollection")
        from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg

        FTKTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg("FTKTrackParticleCnvAlg")
        FTKTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = "Converted_FTKTrackParticleContainer"
        FTKTrackParticleCnvAlg.TrackContainerName = "FTK_TrackCollection"
        FTKTrackParticleCnvAlg.ConvertTrackParticles = False
        FTKTrackParticleCnvAlg.ConvertTracks = True
        FTKTrackParticleCnvAlg.AddTruthLink = True
        FTKTrackParticleCnvAlg.TrackTruthContainerName = "FTK_Tracks_TruthCollection"
        FTKTrackParticleCnvAlg.PrintIDSummaryInfo = True
        topSequence += FTKTrackParticleCnvAlg

        FTKRefitTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg("FTKRefitTrackParticleCnvAlg")
        FTKRefitTrackParticleCnvAlg.xAODTrackParticlesFromTracksContainerName = "Converted_FTKTrackParticleContainerRefit" 
        FTKRefitTrackParticleCnvAlg.TrackContainerName = "FTK_TrackCollectionRefit"
        FTKRefitTrackParticleCnvAlg.ConvertTrackParticles = False
        FTKRefitTrackParticleCnvAlg.ConvertTracks = True
        FTKRefitTrackParticleCnvAlg.AddTruthLink = True
        FTKRefitTrackParticleCnvAlg.TrackTruthContainerName = "FTK_RefitTracks_TruthCollection"
        FTKRefitTrackParticleCnvAlg.PrintIDSummaryInfo = True
        topSequence += FTKRefitTrackParticleCnvAlg

