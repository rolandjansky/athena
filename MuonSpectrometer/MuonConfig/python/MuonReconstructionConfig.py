# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Core configuration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Local
from MuonConfig.MuonSegmentFindingConfig import MuonSegmentFindingCfg
from MuonConfig.MuonTrackBuildingConfig import MuonTrackBuildingCfg, MuonStandaloneTrackParticleCnvAlgCfg

def StandaloneMuonOutputCfg(flags):
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    result = ComponentAccumulator()

    # FIXME! Fix for ATLASRECTS-5151. Remove when better solution found.
    Trk__EventCnvSuperTool = CompFactory.Trk.EventCnvSuperTool
    cnvTool = Trk__EventCnvSuperTool(name = 'EventCnvSuperTool')
    cnvTool.MuonCnvTool.FixTGCs = True 
    result.addPublicTool(cnvTool)

    aod_items=[]
    # Segments 
    aod_items+=[ "xAOD::MuonSegmentContainer#MuonSegments" ]
    aod_items+=[ "xAOD::MuonSegmentAuxContainer#MuonSegmentsAux." ]

    aod_items+=[ "xAOD::MuonSegmentContainer#NCB_MuonSegments" ]
    aod_items+=[ "xAOD::MuonSegmentAuxContainer#NCB_MuonSegmentsAux." ]
    if flags.Muon.runCommissioningChain:
        aod_items+=[ "xAOD::TrackParticleContainer#EMEO_MuonSpectrometerTrackParticles" ]
        aod_items+=[ "xAOD::TrackParticleAuxContainer#EMEO_MuonSpectrometerTrackParticlesAux." ]
        
    # TrackParticles 
    aod_items+=[ "xAOD::TrackParticleContainer#MuonSpectrometerTrackParticles" ]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#MuonSpectrometerTrackParticlesAux." ]
    aod_items+=[ "xAOD::TrackParticleContainer#MuonSpectrometerOnlyTrackParticles" ]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#MuonSpectrometerOnlyTrackParticlesAux." ]

    aod_items+=[ "xAOD::TrackParticleContainer#MSonlyTracklets" ]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#MSonlyTrackletsAux." ]
    aod_items+=[ "xAOD::VertexContainer#MSDisplacedVertex" ]
    aod_items+=[ "xAOD::VertexAuxContainer#MSDisplacedVertexAux." ]

    if flags.Input.isMC:
        # Truth Particle Container
        aod_items+=["xAOD::TruthParticleContainer#MuonTruthParticles"]
        aod_items+=["xAOD::TruthParticleAuxContainer#MuonTruthParticlesAux."]

        # Truth Segment Container
        aod_items+=["xAOD::MuonSegmentContainer#MuonTruthSegments"]
        aod_items+=["xAOD::MuonSegmentAuxContainer#MuonTruthSegmentsAux."]

    if flags.Muon.prdToxAOD:
        aod_items += [ "xAOD::TrackMeasurementValidationContainer#MDT_DriftCircles"] 
        aod_items += [ "xAOD::TrackMeasurementValidationAuxContainer#MDT_DriftCirclesAux."] 
        aod_items += [ "xAOD::TrackMeasurementValidationContainer#RPC_Measurements"] 
        aod_items += [ "xAOD::TrackMeasurementValidationAuxContainer#RPC_MeasurementsAux."] 
        aod_items += [ "xAOD::TrackMeasurementValidationContainer#TGC_MeasurementsAllBCs"] 
        aod_items += [ "xAOD::TrackMeasurementValidationAuxContainer#TGC_MeasurementsAllBCsAux."]
        if flags.Detector.EnableCSC:
            aod_items += [ "xAOD::TrackMeasurementValidationContainer#CSC_Clusters"] 
            aod_items += [ "xAOD::TrackMeasurementValidationAuxContainer#CSC_ClustersAux."] 

    if flags.Muon.rpcRawToxAOD:
        aod_items += [ "xAOD::TrackMeasurementValidationContainer#RPC_RDO_Measurements"] 
        aod_items += [ "xAOD::TrackMeasurementValidationAuxContainer#RPC_RDO_MeasurementsAux."] 

    # ESD list includes all AOD items
    esd_items = aod_items

    #PRDs
    if flags.Detector.EnableMM: esd_items+=["Muon::MMPrepDataContainer#MM_Measurements"]
    if flags.Detector.EnablesTGC: esd_items+=["Muon::sTgcPrepDataContainer#STGC_Measurements"]
    if flags.Detector.EnableCSC:
        esd_items+=["Muon::CscPrepDataContainer#CSC_Clusters"]
        esd_items+=["Muon::CscStripPrepDataContainer#CSC_Measurements"]
    esd_items+=["Muon::RpcPrepDataContainer#RPC_Measurements"]
    esd_items+=["Muon::TgcPrepDataContainer#TGC_MeasurementsAllBCs"]
    esd_items+=["Muon::MdtPrepDataContainer#MDT_DriftCircles"]

    #trigger related info for offline DQA
    esd_items+=["Muon::TgcCoinDataContainer#TrigT1CoinDataCollection"]
    esd_items+=["Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionPriorBC"]
    esd_items+=["Muon::TgcCoinDataContainer#TrigT1CoinDataCollectionNextBC"]
    esd_items+=["Muon::RpcCoinDataContainer#RPC_triggerHits"]
    esd_items+=["RpcSectorLogicContainer#RPC_SECTORLOGIC"]

    # Segments
    esd_items+=["Trk::SegmentCollection#TrackMuonSegments"]
    esd_items+=["Trk::SegmentCollection#NCB_TrackMuonSegments"]

    # Tracks
    esd_items+=["TrackCollection#MuonSpectrometerTracks"] 
    if flags.Muon.runCommissioningChain:
        esd_items+=["TrackCollection#EMEO_MuonSpectrometerTracks"] 
        
    # Truth
    if flags.Input.isMC:
        esd_items += [ "TrackRecordCollection#MuonEntryLayerFilter" ]
        esd_items += [ "TrackRecordCollection#MuonExitLayerFilter" ]
                
        esd_items += ["PRD_MultiTruthCollection#MDT_TruthMap","PRD_MultiTruthCollection#RPC_TruthMap", "PRD_MultiTruthCollection#TGC_TruthMap"]
        if flags.Detector.EnableCSC: esd_items += ["PRD_MultiTruthCollection#CSC_TruthMap"]
        if flags.Detector.EnablesTGC: esd_items += ["PRD_MultiTruthCollection#STGC_TruthMap"]
        if flags.Detector.EnableMM: esd_items += ["PRD_MultiTruthCollection#MM_TruthMap"]

        #Track truth
        esd_items+=["DetailedTrackTruthCollection#MuonSpectrometerTracksTruth"]
        esd_items+=["TrackTruthCollection#MuonSpectrometerTracksTruth"]

        # Since this is only used for debugging, probably okay not to have a writeSDO flag..
        # Leaving this here for the moment in case this turns out to be a mistake.
        # if muonRecFlags.writeSDOs():
        #     if flags.Detector.EnableCSC: esd_items+=["CscSimDataCollection#CSC_SDO"]
        #     esd_items+=["MuonSimDataCollection#MDT_SDO"]
        #     esd_items+=["MuonSimDataCollection#RPC_SDO"]
        #     esd_items+=["MuonSimDataCollection#TGC_SDO"]
        #     if flags.Detector.EnablesTGC: esd_items+=["MuonSimDataCollection#sTGC_SDO"]
        #     if flags.Detector.EnableMM: esd_items+=["MuonSimDataCollection#MM_SDO"]

    if flags.Output.doWriteESD:
        result = OutputStreamCfg(flags, "ESD", esd_items)
    if flags.Output.doWriteAOD:
        result.merge(OutputStreamCfg(flags, "AOD", aod_items))
    return result

def MuonReconstructionCfg(flags):
    # https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py
    result=ComponentAccumulator()
    from MuonConfig.MuonPrepDataConvConfig import MuonPrepDataConvCfg
    result.merge(MuonPrepDataConvCfg(flags))

    result.merge( MuonSegmentFindingCfg(flags))
    result.merge( MuonTrackBuildingCfg(flags))
    result.merge( MuonStandaloneTrackParticleCnvAlgCfg(flags) )

    # Setup output
    if flags.Output.doWriteESD or flags.Output.doWriteAOD:
        result.merge(StandaloneMuonOutputCfg(flags))
    return result
    
if __name__=="__main__":
    # To run this, do e.g. 
    # python -m MuonConfig.MuonReconstructionConfig --run --threads=1
    from MuonConfig.MuonConfigUtils import SetupMuonStandaloneArguments, SetupMuonStandaloneConfigFlags, SetupMuonStandaloneCA

    args = SetupMuonStandaloneArguments()
    ConfigFlags = SetupMuonStandaloneConfigFlags(args)
    cfg = SetupMuonStandaloneCA(args,ConfigFlags)

    # Run the actual test.
    acc = MuonReconstructionCfg(ConfigFlags)
    cfg.merge(acc)
    
    if args.threads>1 and args.forceclone:
        from AthenaCommon.Logging import log
        log.info('Forcing track building cardinality to be equal to '+str(args.threads))
        # We want to force the algorithms to run in parallel (eventually the algorithm will be marked as cloneable in the source code)
        AlgResourcePool = CompFactory.AlgResourcePool
        cfg.addService(AlgResourcePool( OverrideUnClonable=True ) )
        track_builder = acc.getPrimary()
        track_builder.Cardinality=args.threads
            
    from SGComps.AddressRemappingConfig import InputRenameCfg
    cfg.merge(InputRenameCfg("TrackCollection", "MuonSpectrometerTracks", "MuonSpectrometerTracks_old"))

    # This is a temporary fix! Should be private!
    Muon__MuonEDMHelperSvc=CompFactory.Muon.MuonEDMHelperSvc
    muon_edm_helper_svc = Muon__MuonEDMHelperSvc("MuonEDMHelperSvc")
    cfg.addService( muon_edm_helper_svc )

    cfg.printConfig(withDetails = True)
    # drop faulty remapping
    # the evaluation of MuonSegmentNameFixCfg should happen conditinally instead
    # this is hack that is functioning only because this is top level CA
    oldRemaps = cfg.getService("AddressRemappingSvc").TypeKeyRenameMaps
    cfg.getService("AddressRemappingSvc").TypeKeyRenameMaps = [ remap for remap in oldRemaps if "Trk::SegmentCollection" not in remap]
    
    f=open("MuonReconstruction.pkl","wb")
    cfg.store(f)
    f.close()
    
    if args.run:
        sc = cfg.run(20)
        if not sc.isSuccess():
            import sys
            sys.exit("Execution failed")
