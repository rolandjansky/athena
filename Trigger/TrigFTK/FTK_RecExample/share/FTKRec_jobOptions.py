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
    elif FTKFlags.doTruthSmearing:
        from PyJobTransforms.trfUtils import findFile
        pmap8L_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_8LcIbl3D123.pmap')
        pmap12L_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12LiblHW3D.pmap')
        print "Using PMAP 8L:", pmap8L_path
        print "Using PMAP 12L:", pmap12L_path
        rmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12Libl64TmodB_3D_t13.tmap')
        print "Using RMAP:", rmap_path

        import glob
        sectors = []
        ntower = 64
        for i in range(ntower):
          sectors += glob.glob("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/FTK/FitConstants/ftk.64tower.2016.mc16.v2/sectors_raw_12L_reg"+str(i)+".patt.bz2")



        from TrigFTKSim.TrigFTKSimConf import FTK_SGHitInput
        FTKSGInput = FTK_SGHitInput( maxEta= 3.2, minPt= 0.8*GeV)
        FTKSGInput.ReadTruthTracks = False
        FTKSGInput.DoOutFileRawHits=False
        ToolSvc += FTKSGInput



        from TrigFTKFastSimTruth.TrigFTKFastSimTruthConf import  TrigFTKSectorMatchTool
        FTKTrajectorSelector = TrigFTKSectorMatchTool()
        FTKTrajectorSelector.PMapPath8L = pmap8L_path
        FTKTrajectorSelector.PMapPath12L = pmap12L_path
        FTKTrajectorSelector.RMapPath = rmap_path
        FTKTrajectorSelector.SectorFiles = sectors
        ToolSvc+=FTKTrajectorSelector



        from TrigFTKFastSimTruth.TrigFTKFastSimTruthConf import  TrigFTKTrackBuilderTool
        FTKTruthTrackBuilder = TrigFTKTrackBuilderTool()
        FTKTruthTrackBuilder.PMapPath = pmap12L_path
        ToolSvc+=FTKTruthTrackBuilder


        from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
        theFTKrandomSvc = AtRanluxGenSvc("TrigFTKFastSimTruthRandom")
        theFTKrandomSvc.EventReseeding = True
        theFTKrandomSvc.UseOldBrokenSeeding = False
        ServiceMgr += theFTKrandomSvc

        from TrigFTKFastSimTruth.TrigFTKFastSimTruthConf import TrigFTKFastSimTruth
        theTrigFTKFastSimTruth = TrigFTKFastSimTruth(OutputLevel = INFO, 
                             PMapPath_8L = pmap8L_path,
                             PMapPath_12L = pmap12L_path,
                             SectorMatchTool = FTKTrajectorSelector,
                             TrackBuilderTool  = FTKTruthTrackBuilder)
        theTrigFTKFastSimTruth.IBLMode = 2
        theTrigFTKFastSimTruth.HitInputTool = FTKSGInput
        theTrigFTKFastSimTruth.Clustering = True
        theTrigFTKFastSimTruth.PixelClusteringMode = 101
        topSequence += theTrigFTKFastSimTruth

    
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

        if FTKFlags.doDetailedTruth():
            augmentation_tools = []
            from DerivationFrameworkInDet.DerivationFrameworkInDetConf import (DerivationFramework__TrackParametersForTruthParticles)
            
            TruthDecor = DerivationFramework__TrackParametersForTruthParticles(
                name="TruthTPDecor",
                TruthParticleContainerName="TruthParticles",
                            DecorationPrefix="")
            augmentation_tools.append(TruthDecor)
            
            # Set up derivation framework
            from AthenaCommon import CfgMgr
            
            theFTKseq = CfgMgr.AthSequencer("FTKSeq")
            from DerivationFrameworkCore.DerivationFrameworkCoreConf import (
                DerivationFramework__CommonAugmentation)
            
            from AthenaCommon.AppMgr import ToolSvc
            ToolSvc += DerivationFramework__TrackParametersForTruthParticles('TruthTPDecor')
            theFTKseq += CfgMgr.DerivationFramework__CommonAugmentation(
                "TSOS_Kernel",
                AugmentationTools=augmentation_tools,
                OutputLevel=INFO)
            topSequence += theFTKseq


