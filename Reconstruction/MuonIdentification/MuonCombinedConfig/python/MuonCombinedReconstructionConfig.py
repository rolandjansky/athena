# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def MuonCombinedInDetExtensionAlg(flags, name="MuonCombinedInDetExtensionAlg",**kwargs):
    from MuonCombinedRecToolsConfig import MuonCaloTagToolCfg
    tools = []
    result = ComponentAccumulator()
    if flags.MuonCombined.doCaloTrkMuId:
        acc = MuonCaloTagToolCfg(flags)
        tools.append( acc.getPrimary() )
        result.merge(acc)
        kwargs.setdefault("TagMap", "caloTagMap" )
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    kwargs.setdefault("HasCSC", flags.Detector.GeometryCSC )
    kwargs.setdefault("HasSTgc", flags.Detector.GeometrysTGC )
    kwargs.setdefault("HasMM", flags.Detector.GeometryMM )
    
    alg = CompFactory.MuonCombinedInDetExtensionAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def MuonCaloTagAlgCfg(flags, name="MuonCaloTagAlg",**kwargs):
    from MuonCombinedRecToolsConfig import MuonCaloTagToolCfg
    
    tools=[]
    result = MuonCaloTagToolCfg(flags)
    tools.append( result.getPrimary() )
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    result.addPublicTool(kwargs['MuonCombinedInDetExtensionTools'][0]) #Ugh
    kwargs.setdefault("TagMap","caloTagMap")
    kwargs.setdefault("CombinedTrackCollection","")
    kwargs.setdefault("METrackCollection","")
    kwargs.setdefault("SegmentCollection","")
    kwargs.setdefault("HasCSC", flags.Detector.GeometryCSC )
    kwargs.setdefault("HasSTgc", flags.Detector.GeometrysTGC )
    kwargs.setdefault("HasMM", flags.Detector.GeometryMM )
    alg = CompFactory.MuonCombinedInDetExtensionAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def MuonSegmentTagAlgCfg(flags, name="MuonSegmentTagAlg", **kwargs ):
    from MuonCombinedRecToolsConfig import MuonCaloTagToolCfg

    result = MuonCaloTagToolCfg(flags)
    kwargs.setdefault("MuonSegmentTagTool", result.getPrimary() )
    alg = CompFactory.MuonSegmentTagAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def MuonInsideOutRecoAlgCfg(flags, name="MuonInsideOutRecoAlg", **kwargs ):
    from MuonCombinedRecToolsConfig import MuonInsideOutRecoToolCfg

    tools = []
    result = MuonInsideOutRecoToolCfg(flags)
    insideoutrecotool = result.getPrimary()
    tools.append( insideoutrecotool )
    result.addPublicTool( insideoutrecotool )

    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    kwargs.setdefault("usePRDs",True)
    kwargs.setdefault("HasCSC", flags.Detector.GeometryCSC )
    kwargs.setdefault("HasSTgc", flags.Detector.GeometrysTGC )
    kwargs.setdefault("HasMM", flags.Detector.GeometryMM )
    kwargs.setdefault("TagMap","muGirlTagMap")
    alg = CompFactory.MuonCombinedInDetExtensionAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def MuGirlStauAlgCfg(flags, name="MuGirlStauAlg",**kwargs):
    from MuonCombinedRecToolsConfig import MuonStauRecoToolCfg
    result = MuonStauRecoToolCfg(flags)
    tools = [result.getPrimary()]
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    kwargs.setdefault("TagMap","stauTagMap")
    kwargs.setdefault("HasCSC", flags.Detector.GeometryCSC )
    kwargs.setdefault("HasSTgc", flags.Detector.GeometrysTGC )
    kwargs.setdefault("HasMM", flags.Detector.GeometryMM )
    kwargs.setdefault("CombinedTrackCollection","MuGirlStauCombinedTracks")
    kwargs.setdefault("METrackCollection","")
    kwargs.setdefault("SegmentCollection","MuGirlStauSegments")
    alg = CompFactory.MuonCombinedInDetExtensionAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result


def MuonCombinedMuonCandidateAlgCfg(flags, name="MuonCombinedMuonCandidateAlg", **kwargs ):
    from MuonCombinedRecToolsConfig import MuonCandidateToolCfg

    result = ComponentAccumulator()

    tool_kwargs={}
    if flags.Beam.Type == 'cosmics':
        tool_kwargs.setdefault("ExtrapolationStrategy", 1 )

    acc = MuonCandidateToolCfg(flags, **tool_kwargs)
    kwargs.setdefault("MuonCandidateTool",acc.popPrivateTools())
    result.merge(acc)

    alg = CompFactory.MuonCombinedMuonCandidateAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def MuonCombinedInDetCandidateAlg(flags, name="MuonCombinedInDetCandidateAlg",**kwargs ):
    # FIXME - need to have InDet flags set at this point to know if doForwardTracks is true. 
    from MuonCombinedRecToolsConfig import MuonCombinedInDetDetailedTrackSelectorToolCfg
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

    result = MuonCombinedInDetDetailedTrackSelectorToolCfg(flags)
    kwargs.setdefault("TrackSelector", result.getPrimary() )
    # if flags.MuonCombined.doSiAssocForwardMuons and flags.InDet.doForwardTracks: FIXME
    if flags.MuonCombined.doSiAssocForwardMuons:
        kwargs.setdefault("DoSiliconAssocForwardMuons", True )
        # From old config, addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedInDetDetailedTrackSelectorTool","MuonCombinedInDetDetailedForwardTrackSelectorTool", nHitSct=0)
        acc = MuonCombinedInDetDetailedTrackSelectorToolCfg(flags, "MuonCombinedInDetDetailedForwardTrackSelectorTool", nHitSct=0)
        kwargs.setdefault("InDetForwardTrackSelector", acc.getPrimary() )
        result.merge(acc)
    
    acc = AtlasExtrapolatorCfg(flags)
    extrapolator = acc.getPrimary()
    result.merge(acc)

    muon_particle_extension_tool = CompFactory.Trk__ParticleCaloExtensionTool(Extrapolator=extrapolator)

    muon_ext_tool = CompFactory.Muon__MuonSystemExtensionTool(ParticleCaloExtensionTool = muon_particle_extension_tool, Extrapolator = extrapolator)
    kwargs.setdefault("MuonSystemExtensionTool", muon_ext_tool)
    
    alg = CompFactory.MuonCombinedInDetCandidateAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def recordMuonCreatorAlgObjs (kw):
    Alg = CompFactory.MuonCreatorAlg
    def val (prop):
        d = kw.get (prop)
        if d is None:
            d = Alg.__dict__[prop].default
        return d
    objs = {'xAOD::MuonContainer': val('MuonContainerLocation'),
            'xAOD::TrackParticleContainer': (val('CombinedLocation')+'TrackParticles',
                                             val('ExtrapolatedLocation')+'TrackParticles',
                                             val('MSOnlyExtrapolatedLocation')+'TrackParticles'),
            'xAOD::MuonSegmentContainer': val('SegmentContainerName'),
            }
    if val('BuildSlowMuon'):
        objs['xAOD::SlowMuonContainer'] = val('SlowMuonContainerLocation')
    if val('MakeClusters'):
        objs['CaloClusterCellLinkContainer'] =  val('CaloClusterCellLinkName') + '_links'
        objs['xAOD::CaloClusterContainer'] =  val('ClusterContainerName')
        
    # from RecExConfig.ObjKeyStore import objKeyStore
    # objKeyStore.addManyTypesTransient (objs)
    return


def MuonCreatorAlgCfg( flags, name="MuonCreatorAlg",**kwargs ):
    from MuonCombinedRecToolsConfig import MuonCreatorToolCfg
    result = MuonCreatorToolCfg(flags)
    kwargs.setdefault("MuonCreatorTool",result.popPrivateTools())
    # recordMuonCreatorAlgObjs (kwargs)
    # if muGirl is off, remove "muGirlTagMap" from "TagMaps"
    # but don't set this default in case the StauCreatorAlg is created (see below)
    if not flags.MuonCombined.doMuGirl and not name=="StauCreatorAlg":
        kwargs.setdefault("TagMaps",["muidcoTagMap","stacoTagMap","caloTagMap","segmentTagMap"])
    # if TriggerFlags.MuonSlice.doTrigMuonConfig:
    #     kwargs.setdefault("MakeClusters", False)
    #     kwargs.setdefault("ClusterContainerName", "")
    alg = CompFactory.MuonCreatorAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def StauCreatorAlgCfg(flags, name="StauCreatorAlg", **kwargs ):
    from MuonCombinedRecToolsConfig import MuonCreatorToolCfg
    result = MuonCreatorToolCfg(flags, BuildStauContainer=True)
    kwargs.setdefault("MuonCreatorTool",result.popPrivateTools())
    kwargs.setdefault("MuonContainerLocation","Staus")
    kwargs.setdefault("CombinedLocation","CombinedStau")
    kwargs.setdefault("ExtrapolatedLocation","ExtrapolatedStau")
    kwargs.setdefault("MSOnlyExtrapolatedLocation","MSOnlyExtrapolatedStau")
    kwargs.setdefault("MuonCandidateLocation","")
    kwargs.setdefault("SegmentContainerName","StauSegments")
    kwargs.setdefault("BuildSlowMuon",1)
    kwargs.setdefault("ClusterContainerName", "SlowMuonClusterCollection")
    kwargs.setdefault("TagMaps",["stauTagMap"])
    # if not TriggerFlags.MuonSlice.doTrigMuonConfig:
    #     recordMuonCreatorAlgObjs (kwargs)
    acc = MuonCreatorAlgCfg(flags, name,**kwargs)
    result.merge(acc)
    return result # don't have the right algorithm being primary here, but should be okay?

def MuonCombinedReconstructionCfg(flags):
    result = ComponentAccumulator()
    #Arguably this should be somewhere deeper - not quite sure where yet though. FIXME
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg 
    result.merge( MuonGeoModelCfg(flags) )
    
    Muon__MuonEDMHelperSvc=CompFactory.Muon__MuonEDMHelperSvc
    muon_edm_helper_svc = Muon__MuonEDMHelperSvc("MuonEDMHelperSvc")
    result.addService( muon_edm_helper_svc )

    result.merge( MuonCombinedInDetCandidateAlg(flags) )
    result.merge( MuonCombinedMuonCandidateAlgCfg(flags) )

    if flags.MuonCombined.doStatisticalCombination or flags.MuonCombined.doCombinedFit:
        result.merge( MuonCombinedMuonCandidateAlgCfg(flags) )

    if flags.MuonCombined.doMuGirl:
        result.merge(MuonInsideOutRecoAlgCfg(flags) ) 
        if flags.MuonCombined.doMuGirlLowBeta:
            result.merge(MuGirlStauAlgCfg)

    if flags.MuonCombined.doCaloTrkMuId:
        result.merge( MuonCaloTagAlgCfg(flags) )
        
    if flags.MuonCombined.doMuonSegmentTagger:
        result.merge( MuonSegmentTagAlgCfg(flags) )

    # runs over outputs and create xAODMuon collection
    acc = MuonCreatorAlgCfg(flags)
    result.merge(acc)
    
    if flags.MuonCombined.doMuGirl and flags.MuonCombined.doMuGirlLowBeta:
        # Has to be at end if not using sequencer. If we drop this requirement, can be moved above
        result.merge( StauCreatorAlgCfg(flags) )

    return result

if __name__=="__main__":
    # To run this, do e.g. 
    # python -m MuonCombinedConfig.MuonCombinedReconstructionConfig --run --threads=1
    
    from argparse import ArgumentParser    
    parser = ArgumentParser()
    parser.add_argument("-t", "--threads", type=int,
                        help="number of threads", default=1)
                        
    parser.add_argument("-o", "--output", default='newESD.pool.root',
                        help="write ESD to FILE", metavar="FILE")
                        
    parser.add_argument("--run", help="Run directly from the python. If false, just stop once the pickle is written.",
                        action="store_true")

    parser.add_argument("--forceclone", help="Override default cloneability of algorithms to force them to run in parallel",
                        action="store_true")
                        
    args = parser.parse_args()    
    
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaCommon.Logging import log
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
    ConfigFlags.Concurrency.NumThreads=args.threads
    ConfigFlags.Concurrency.NumConcurrentEvents=args.threads # Might change this later, but good enough for the moment.

    ConfigFlags.Detector.GeometryMDT   = True 
    ConfigFlags.Detector.GeometryTGC   = True
    ConfigFlags.Detector.GeometryCSC   = True     
    ConfigFlags.Detector.GeometryRPC   = True 
        
    # from AthenaConfiguration.TestDefaults import defaultTestFiles
    # ConfigFlags.Input.Files = defaultTestFiles.ESD
    ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/ESD.16747874._000011_100events.pool.root']
    ConfigFlags.Output.ESDFileName=args.output

    # from AthenaCommon.Constants import DEBUG
    # log.setLevel(DEBUG)
    # log.info('About to set up Muon Track Building.')
    
    ConfigFlags.Input.isMC = True
    ConfigFlags.lock()
    ConfigFlags.dump()

    # When running from a pickled file, athena inserts some services automatically. So only use this if running now.
    if args.run:
        from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
        cfg = MainServicesThreadedCfg(ConfigFlags)
        msgService = cfg.getService('MessageSvc')
        msgService.Format = "S:%s E:%e % F%58W%S%7W%R%T  %0W%M"
    else:
        cfg=ComponentAccumulator()
    
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    log.debug('About to set up Combined Muon Reconstruction.')  
    acc = MuonCombinedReconstructionCfg(ConfigFlags)
    cfg.merge(acc)
    
    if args.threads>1 and args.forceclone:
        log.info('Forcing track building cardinality to be equal to '+str(args.threads))
        # We want to force the algorithms to run in parallel (eventually the algorithm will be marked as cloneable in the source code)
        AlgResourcePool=CompFactory.AlgResourcePool
        cfg.addService(AlgResourcePool( OverrideUnClonable=True ) )
        track_builder = acc.getPrimary()
        track_builder.Cardinality=args.threads
            
    # This is a temporary fix - it should go someplace central as it replaces the functionality of addInputRename from here:
    # https://gitlab.cern.ch/atlas/athena/blob/master/Control/SGComps/python/AddressRemappingSvc.py
    AddressRemappingSvc, ProxyProviderSvc=CompFactory.getComps("AddressRemappingSvc","ProxyProviderSvc",)
    pps = ProxyProviderSvc()
    ars=AddressRemappingSvc()
    pps.ProviderNames += [ 'AddressRemappingSvc' ]
    ars.TypeKeyRenameMaps += [ '%s#%s->%s' % ("TrackCollection", "MuonSpectrometerTracks", "MuonSpectrometerTracks_old") ]
    
    cfg.addService(pps)
    cfg.addService(ars)
    
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    itemsToRecord = ["TrackCollection#MuonSpectrometerTracks"] 
    
    cfg.merge( OutputStreamCfg( ConfigFlags, 'ESD', ItemList=itemsToRecord) )
    
    outstream = cfg.getEventAlgo("OutputStreamESD")
    # outstream.OutputLevel=DEBUG
    outstream.ForceRead = True
    
    cfg.printConfig(withDetails = True, summariseProps = True)
              
    f=open("MuonCombinedReconstruction.pkl","wb")
    cfg.store(f)
    f.close()
    
    if args.run:
        cfg.run(20)
        
