# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Core configuration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Local
from MuonConfig.MuonSegmentFindingConfig import MuonSegmentFindingCfg
from MuonConfig.MuonTrackBuildingConfig import MuonTrackBuildingCfg


def MuonReconstructionCfg(flags):
    # https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py
    result=ComponentAccumulator()
    result.merge( MuonSegmentFindingCfg(flags))
    result.merge( MuonTrackBuildingCfg(flags))

    return result
    
if __name__=="__main__":
    # To run this, do e.g. 
    # python -m MuonConfig.MuonReconstructionConfig --run --threads=1
    from MuonConfig.MuonConfigUtils import SetupMuonStandaloneArguments, SetupMuonStandaloneConfigFlags, SetupMuonStandaloneOutput, SetupMuonStandaloneCA

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

    itemsToRecord = ["Trk::SegmentCollection#TrackMuonSegments", "Trk::SegmentCollection#NCB_TrackMuonSegments"]
    itemsToRecord += ["TrackCollection#MuonSpectrometerTracks"] 
    SetupMuonStandaloneOutput(cfg, ConfigFlags, itemsToRecord)
    cfg.printConfig(withDetails = True)
    # drop faulty remapping
    # the evaluation of MuonSegmentNameFixCfg should happen conditinally instead
    # this is hack that is functioning only because this is top level CA
    oldRemaps = cfg.getService("AddressRemappingSvc").TypeKeyRenameMaps
    cfg.getService("AddressRemappingSvc").TypeKeyRenameMaps = [ remap for remap in oldRemaps if not "Trk::SegmentCollection" in remap]
    

    f=open("MuonReconstruction.pkl","wb")
    cfg.store(f)
    f.close()
    
    if args.run:
        sc = cfg.run(20)
        if not sc.isSuccess():
            import sys
            sys.exit("Execution failed")
