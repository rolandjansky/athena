# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def MuonCaloTagAlgCfg(flags, name="MuonCaloTagAlg",**kwargs):
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCaloTagToolCfg    
    tools=[]
    result = MuonCaloTagToolCfg(flags)
    tools.append( result.getPrimary() )
    kwargs.setdefault("MuonCombinedInDetExtensionTools", tools )
    result.addPublicTool(kwargs['MuonCombinedInDetExtensionTools'][0]) #Ugh
    kwargs.setdefault("TagMap","caloTagMap")
    kwargs.setdefault("CombinedTrackCollection","")
    kwargs.setdefault("METrackCollection","") 
    alg = CompFactory.MuonCombinedInDetExtensionAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def MuonSegmentTagAlgCfg(flags, name="MuonSegmentTagAlg", **kwargs ):
    # from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCaloTagToolCfg FIXME

    result = MuonSegmentTagToolCfg(flags)
    muon_segment_tag_tool = result.getPrimary()
    kwargs.setdefault("MuonSegmentTagTool", muon_segment_tag_tool)
    result.addPublicTool(muon_segment_tag_tool)
    alg = CompFactory.MuonSegmentTagAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result
  
def MuTagMatchingToolCfg(flags, name='MuTagMatchingTool', **kwargs ):
    from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasRKPropagatorCfg
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

    #TODO: defaults in cxx
    kwargs.setdefault("AssumeLocalErrors", True)
    kwargs.setdefault("PhiCut", 30.)
    kwargs.setdefault("GlobalPhiCut", 1.)
    kwargs.setdefault("ThetaCut", 5.)
    kwargs.setdefault("GlobalThetaCut", 0.5)
    kwargs.setdefault("ThetaAngleCut", 5.)
    kwargs.setdefault("DoDistanceCut", True)
    kwargs.setdefault("CombinedPullCut", 3.0)

    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("IExtrapolator", result.popPrivateTools())

    kwargs.setdefault("Propagator", result.popToolsAndMerge( AtlasRKPropagatorCfg(flags) ))

    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
        TrackingGeometryCondAlgCfg)
    acc = TrackingGeometryCondAlgCfg(flags)
    geom_cond_key = acc.getPrimary().TrackingGeometryWriteKey
    result.merge(acc)
    kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)

    tool = CompFactory.MuTagMatchingTool(name,**kwargs)
    
    result.addPublicTool(tool, primary=True)
    return result

def MuTagAmbiguitySolverToolCfg(flags, name='MuTagAmbiguitySolverTool', **kwargs ):
    #TODO: defaults in cxx
    kwargs.setdefault("RejectOuterEndcap",True)
    kwargs.setdefault("RejectMatchPhi",True)
    tool =  CompFactory.MuTagAmbiguitySolverTool(name,**kwargs)
    result = ComponentAccumulator()
    result.addPublicTool(tool, primary=True)
    return result

  
def MuonSegmentTagToolCfg(flags, name="MuonSegmentTagTool", **kwargs ):
    result = ComponentAccumulator()
    mu_tag_matching = result.getPrimaryAndMerge(MuTagMatchingToolCfg(flags))
    kwargs.setdefault("MuTagMatchingTool", mu_tag_matching)
    
    acc = MuTagAmbiguitySolverToolCfg(flags)
    kwargs.setdefault("MuTagAmbiguitySolverTool", acc.getPrimary())
    result.merge(acc)

    result.setPrivateTools(CompFactory.MuonCombined.MuonSegmentTagTool(name,**kwargs))

    return result


def MuonInsideOutRecoAlgCfg(flags, name="MuonInsideOutRecoAlg", **kwargs ):
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonInsideOutRecoToolCfg

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
    kwargs.setdefault("SegmentCollection","MuGirlSegments")
    alg = CompFactory.MuonCombinedInDetExtensionAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def MuGirlAlg_LRTCfg(flags, name="MuGirlAlg_LRT",**kwargs):
    kwargs.setdefault("TagMap","MuGirlMap_LRT")
    kwargs.setdefault("METrackCollection","MuGirlMETracks_LRT")
    kwargs.setdefault("SegmentCollection","MuGirlSegments_LRT")
    kwargs.setdefault("CombinedTrackCollection","MuGirlCombinedMuonContainerLRT")
    kwargs.setdefault("InDetCandidateLocation","TrackParticleCandidateLRT")
    return MuonInsideOutRecoAlgCfg(flags, name, **kwargs)

def MuGirlStauAlgCfg(flags, name="MuGirlStauAlg",**kwargs):
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonStauRecoToolCfg
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
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCandidateToolCfg

    result = ComponentAccumulator()

    # EJWM - not completely sure where this comes from. Perhaps should be retrieved by a sub-tool?
    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(flags,"totalNoise"))

    tool_kwargs={}
    if flags.Beam.Type == 'cosmics':
        tool_kwargs.setdefault("ExtrapolationStrategy", 1 )

    acc = MuonCandidateToolCfg(flags, **tool_kwargs)
    kwargs.setdefault("MuonCandidateTool",acc.popPrivateTools())
    result.merge(acc)

    alg = CompFactory.MuonCombinedMuonCandidateAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def MuonCombinedMuonCandidateAlg_EMEO_Cfg(flags, name = "MuonCombinedMuonCandidateAlg_EMEO"):
    result = ComponentAccumulator()
    
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import CombinedMuonTrackBuilder_EMEO_Cfg
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCandidateToolCfg

    track_builder = result.getPrimaryAndMerge(CombinedMuonTrackBuilder_EMEO_Cfg(flags))
   
    acc = MuonCandidateToolCfg(flags,
                               name = "MuonCandidateTool_EMEO",
                               TrackBuilder = track_builder,
                               Commissioning = True)
    candidate_tool = acc.getPrimary()
    result.merge(acc)

    alg = CompFactory.MuonCombinedMuonCandidateAlg(name,
                                                   MuonCandidateTool = candidate_tool)
    result.addEventAlgo( alg, primary=True )

    return result


def MuonCombinedInDetCandidateAlgCfg(flags, name="MuonCombinedInDetCandidateAlg",**kwargs ):
    # FIXME - need to have InDet flags set at this point to know if doForwardTracks is true. 
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCombinedInDetDetailedTrackSelectorToolCfg
    
    result = MuonCombinedInDetDetailedTrackSelectorToolCfg(flags)
    kwargs.setdefault("TrackSelector", result.getPrimary() )
    # if flags.MuonCombined.doSiAssocForwardMuons and flags.InDet.doForwardTracks: FIXME
    if flags.MuonCombined.doSiAssocForwardMuons:
        kwargs.setdefault("DoSiliconAssocForwardMuons", True )
        # From old config, addTool("MuonCombinedRecExample.MuonCombinedTools.MuonCombinedInDetDetailedTrackSelectorTool","MuonCombinedInDetDetailedForwardTrackSelectorTool", nHitSct=0)
        acc = MuonCombinedInDetDetailedTrackSelectorToolCfg(flags, "MuonCombinedInDetDetailedForwardTrackSelectorTool", nHitSct=0)
        kwargs.setdefault("InDetForwardTrackSelector", acc.getPrimary() )
        result.merge(acc)

    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonSystemExtensionToolCfg
    muon_ext_tool = result.getPrimaryAndMerge(MuonSystemExtensionToolCfg(flags))
    kwargs.setdefault("MuonSystemExtensionTool", muon_ext_tool)
    
    #### Switch off the muon system extensions if we fit combined muons 
    #### The MuonInDetToSystemExtensionAlg will perform the system extensions then
    kwargs.setdefault("ExtendBulk", not  flags.MuonCombined.doCombinedFit or flags.Muon.MuonTrigger)
    
    ### FIX ME... Are the CaloExtensions already available in CA?
    if False and not flags.Muon.MuonTrigger:
        kwargs.setdefault("CaloExtensionLocation", ["ParticleCaloExtension"] )
 
    alg = CompFactory.MuonCombinedInDetCandidateAlg(name, **kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def MuonInDetToMuonSystemExtensionAlgCfg(flags, name ="MuonInDetToMuonSystemExtensionAlg",  **kwargs):
    result = ComponentAccumulator()
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonSystemExtensionToolCfg
    muon_ext_tool = result.getPrimaryAndMerge(MuonSystemExtensionToolCfg(flags))
    kwargs.setdefault("MuonSystemExtensionTool", muon_ext_tool)
    kwargs.setdefault("WriteStauCandidates","")
    alg = CompFactory.MuonInDetToMuonSystemExtensionAlg(name,**kwargs) 
    result.addEventAlgo( alg, primary=True )
    return result


def MuonCombinedInDetCandidateAlg_LRTCfg(flags, name="MuonCombinedInDetCandidateAlg_LRT",**kwargs ):
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCombinedInDetDetailedTrackSelectorToolCfg, MuonCombinedInDetDetailedTrackSelectorTool_LRTCfg
    tmpAcc1 = MuonCombinedInDetDetailedTrackSelectorTool_LRTCfg(flags)
    kwargs.setdefault("TrackSelector", tmpAcc1.getPrimary() )
    ### Use the Standard Track particle container in cases where no separate containters will be
    ### saved for the LRT tracking
    kwargs.setdefault("TrackParticleLocation",["InDetLargeD0TrackParticles"])
    kwargs.setdefault("InDetCandidateLocation","TrackParticleCandidateLRT")
    kwargs.setdefault("DoSiliconAssocForwardMuons", False)
    tmpAcc2 = MuonCombinedInDetDetailedTrackSelectorToolCfg(flags, "MuonCombinedInDetDetailedForwardTrackSelectorTool", nHitSct=0)
    kwargs.setdefault("InDetForwardTrackSelector", tmpAcc2.getPrimary() )
    result = MuonCombinedInDetCandidateAlgCfg(flags, name, **kwargs)
    result.merge(tmpAcc1)
    result.merge(tmpAcc2)
    return result

def MuonCombinedAlgCfg( flags, name="MuonCombinedAlg",**kwargs ):
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCombinedToolCfg

    result = MuonCombinedToolCfg(flags)
    kwargs.setdefault("MuonCombinedTool",result.getPrimary())
    tagmaps = []
    # CombinedTagMaps must be in a 1-1 correspondence
    # with MuonCombinedTagTools.
    for h in kwargs['MuonCombinedTool'].MuonCombinedTagTools:
        if str(h).find('FitTagTool') >= 0:
            tagmaps.append ('muidcoTagMap')
        elif str(h).find('StacoTagTool') >= 0:
            tagmaps.append ('stacoTagMap')
    kwargs.setdefault("CombinedTagMaps", tagmaps)
    alg = CompFactory.MuonCombinedAlg(name,**kwargs)
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
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCreatorToolCfg
    result = MuonCreatorToolCfg(flags)
    kwargs.setdefault("MuonCreatorTool",result.popPrivateTools())

    # MuonSegmentConvertorTool not set up. But it currently only contains:
    # MuonSegmentHitSummaryTool and MuonHitTimingTool, neither which appear to need explicit configuration

    # recordMuonCreatorAlgObjs (kwargs)
    # if muGirl is off, remove "muGirlTagMap" from "TagMaps"
    # but don't set this default in case the StauCreatorAlg is created (see below)
    if not flags.MuonCombined.doMuGirl and not name=="StauCreatorAlg":
        tag_maps = ["muidcoTagMap","stacoTagMap","caloTagMap","segmentTagMap"]        
        kwargs.setdefault("TagMaps",tag_maps)        
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("MakeClusters", False)
        kwargs.setdefault("ClusterContainerName", "")
        kwargs.setdefault("CopySegments", False)
        if flags.Muon.SAMuonTrigger:
            kwargs.setdefault("CreateSAmuons", True)
            kwargs.setdefault("TagMaps", [])

    alg = CompFactory.MuonCreatorAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def MuonCreatorAlg_EMEO(flags, name = "MuonCreatorAlg_EMEO", **kwargs ):
    muon_maps = ["MuonCandidates_EMEO"]
    combined_maps = []
    kwargs.setdefault("TagMaps", combined_maps)
    kwargs.setdefault("MuonCandidateLocation", muon_maps)
    kwargs.setdefault("MuonContainerLocation", "EMEO_Muons")
    kwargs.setdefault("ExtrapolatedLocation", "EMEO_ExtraPolatedMuon")
    kwargs.setdefault("MSOnlyExtrapolatedLocation", "EMEO_MSOnlyExtraPolatedMuon")   
    kwargs.setdefault("CombinedLocation", "EMEO_CombinedMuon")
    kwargs.setdefault("SegmentContainerName", "EMEO_MuonSegments")
    kwargs.setdefault("TrackSegmentContainerName", "EMEO_TrkMuonSegments")
    kwargs.setdefault("BuildSlowMuon", False)
    kwargs.setdefault("MakeClusters", False)
    kwargs.setdefault("ClusterContainerName", "")
    return MuonCreatorAlgCfg(flags, name = name, **kwargs)

def StauCreatorAlgCfg(flags, name="StauCreatorAlg", **kwargs ):
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCreatorToolCfg
    result = MuonCreatorToolCfg(flags, BuildStauContainer=True)
    kwargs.setdefault("MuonCreatorTool",result.popPrivateTools())
    kwargs.setdefault("MuonContainerLocation","Staus")
    kwargs.setdefault("CombinedLocation","CombinedStau")
    kwargs.setdefault("ExtrapolatedLocation","ExtrapolatedStau")
    kwargs.setdefault("MSOnlyExtrapolatedLocation","MSOnlyExtrapolatedStau")
    kwargs.setdefault("MuonCandidateLocation","")
    kwargs.setdefault("SegmentContainerName","StauSegments")
    kwargs.setdefault("TrackSegmentContainerName","TrkStauSegments")
    kwargs.setdefault("BuildSlowMuon",1)
    kwargs.setdefault("ClusterContainerName", "SlowMuonClusterCollection")
    kwargs.setdefault("TagMaps",["stauTagMap"])
    kwargs.setdefault("CopySegments", False)
    # if not flags.Muon.MuonTrigger:
    #     recordMuonCreatorAlgObjs (kwargs)
    acc = MuonCreatorAlgCfg(flags, name,**kwargs)
    result.merge(acc)
    return result # don't have the right algorithm being primary here, but should be okay?

### Returns a pair vectors containing th names of the 
### track particle collections associated with combined muon tracks
def GetCombinedTrkContainers(flags):
    tp_coll = []
    track_coll = []
    tp_coll = ["ExtrapolatedMuonTrackParticles", 
                "CombinedMuonTrackParticles", 
                "MSOnlyExtrapolatedMuonTrackParticles" ]
    track_coll = ["ExtrapolatedMuonTracks", 
                  "CombinedMuonTracks", 
                  "MSOnlyExtrapolatedTracks"]
    if flags.Muon.runCommissioningChain:
        tp_coll += ["EMEO_ExtrapolatedMuonTrackParticles",
                    "EMEO_CombinedMuonTrackParticles",
                    "EMEO_MSOnlyExtrapolatedMuonTrackParticles"] 
        track_coll += ["EMEO_ExtrapolatedMuonTracks", 
                      "EMEO_CombinedMuonTracks", 
                      "EMEO_MSOnlyExtrapolatedTracks"]
    if flags.MuonCombined.doMuGirl and flags.MuonCombined.doMuGirlLowBeta:
        tp_coll+= ["CombinedStauTrackParticles",
                   "ExtrapolatedStauTrackParticles"]
        track_coll+=["CombinedStauTracks",
                     "ExtrapolatedStauTracks"]
    ### Disable the LRT tracks for the moment.
    if False and flags.InDet.Tracking.doR3LargeD0:
        tp_coll += ["CombinedMuonsLRTTrackParticles", 
                     "ExtrapolatedMuonsLRTTrackParticles",
                    "MSOnlyExtrapolatedMuonsLRTTrackParticles"]
    return tp_coll, track_coll

def MuonTrkIDMSScatterDecorAlgCfg(flags, name ="MuonCombIDMSScatterDecorAlg", **kwargs ):
    result = ComponentAccumulator()
    the_alg = CompFactory.MuonTrkIDMSScatterDecorAlg(name = name, **kwargs)
    result.addEventAlgo(the_alg)
    return result

def MuonTrkAEOTDecorAlgCfg(flags, name ="MuonCombAEOTDecorAlg", **kwargs ):
    result = ComponentAccumulator()
    the_alg = CompFactory.MuonTrkAEOTDecorationAlg(name = name, **kwargs)
    result.addEventAlgo(the_alg)
    return result
def MuonPrecisionLayerDecorAlgCfg(flags, name = "MuonPrecisionLayerDecorAlg", **kwargs):
    result = ComponentAccumulator()
    the_alg = CompFactory.MuonPrecisionLayerDecorAlg(name = name, **kwargs)
    result.addEventAlgo(the_alg)
    return result

def MuonDecorationAlgsCfg(flags):
    result  = ComponentAccumulator()
    trk_cols = GetCombinedTrkContainers(flags)[0]
    
    ### Decorate the muon tracks
    for coll in trk_cols:
            result.merge(MuonTrkIDMSScatterDecorAlgCfg(flags, "MuonCombIDMSScatterDecorAlg"+coll,
                                                            TrackContainer=coll))

            result.merge(MuonTrkAEOTDecorAlgCfg(flags, "MuonCombAEOTDecorAlg"+coll,
                                                TrackContainer = coll))
    ### Proceed with the precision layer decoration                                            
    result.merge(MuonPrecisionLayerDecorAlgCfg(flags, "MuonCombPrecisionLayerDecorAlg",
                                            MuonContainer="Muons",
                                            TrackContainer=trk_cols))
    if flags.MuonCombined.doMuGirl and flags.MuonCombined.doMuGirlLowBeta:
        result.merge(MuonPrecisionLayerDecorAlgCfg(flags, "MuonCombStauPrecisionLayerDecorAlg",
                                                    MuonContainer="Staus",
                                                    TrackContainer=trk_cols))
    if False and flags.InDet.Tracking.doR3LargeD0:
        result.merge(MuonPrecisionLayerDecorAlgCfg(flags, "MuonCombLRTPrecisionLayerDecorAlg",
                                                         MuonContainer="MuonsLRT",
                                                         TrackContainer=trk_cols))

    return result

def CombinedMuonOutputCfg(flags):
    from OutputStreamAthenaPool.OutputStreamConfig import addToESD,addToAOD
    result = ComponentAccumulator()

    # FIXME! Fix for ATLASRECTS-5151. Remove when better solution found.
    Trk__EventCnvSuperTool = CompFactory.Trk.EventCnvSuperTool
    cnvTool = Trk__EventCnvSuperTool(name = 'EventCnvSuperTool')
    cnvTool.MuonCnvTool.FixTGCs = True 
    result.addPublicTool(cnvTool)

    # Avoid old-style import from from IsolationAlgs.IsoUpdatedTrackCones import iso_vars
    # But shouldn't be here.
    iso_vars = []
    for track_pt in 500, 1000:
        for cone_str in "", "LooseCone":
            name = f"Nonprompt_All_MaxWeightTTVA{cone_str}_pt{track_pt}"
            iso_vars += ["ptconeCorrBitset_"+name, "ptconecoreTrackPtrCorrection_"+name]
            for cone_size in 20, 30, 40:
                for var_str in "", "var":
                    iso_vars.append(f"pt{var_str}cone{cone_size}_{name}")


    aod_items = []

    if flags.Detector.EnableCalo: 
        aod_items+=["xAOD::CaloClusterContainer#MuonClusterCollection"]
        aod_items+=["xAOD::CaloClusterAuxContainer#MuonClusterCollectionAux."]
        aod_items+=["CaloClusterCellLinkContainer#MuonClusterCollection_links"]

    # Adding the xAOD content by default
    excludedAuxData = '-clusterAssociation'
    aod_items+=[ "xAOD::TrackParticleContainer#CombinedMuonTrackParticles"]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#CombinedMuonTrackParticlesAux." + excludedAuxData ]
    aod_items+=[ "xAOD::TrackParticleContainer#ExtrapolatedMuonTrackParticles" ]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#ExtrapolatedMuonTrackParticlesAux." + excludedAuxData ]
    aod_items+=[ "xAOD::TrackParticleContainer#MSOnlyExtrapolatedMuonTrackParticles" ]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#MSOnlyExtrapolatedMuonTrackParticlesAux." + excludedAuxData ]
    aod_items+=[ "xAOD::TrackParticleContainer#MuonSpectrometerTrackParticles" ]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#MuonSpectrometerTrackParticlesAux." + excludedAuxData ]
    aod_items+=[ "xAOD::MuonContainer#Muons" ]
    aod_items+=[ "xAOD::MuonContainer#MuonsLRT"]

    # FIXME! Next two lines are hack to remove derivation framework variables that are added by DRAW building and are supposed to be transient
    excludedMuonAuxData = ".-"+".-".join(iso_vars)
    aod_items+=[ "xAOD::MuonAuxContainer#MuonsAux.-DFCommonMuonsTight.-DFCommonGoodMuon.-DFCommonMuonsMedium.-DFCommonMuonsLoose" + excludedMuonAuxData ]
    aod_items+=[ "xAOD::MuonAuxContainer#MuonsLRTAux.-DFCommonMuonsTight.-DFCommonGoodMuon.-DFCommonMuonsMedium.-DFCommonMuonsLoose" + excludedMuonAuxData] 
    
    ### Combined muon track particles
    aod_items+=[ "xAOD::TrackParticleContainer#CombinedMuonsLRTTrackParticles"]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#CombinedMuonsLRTTrackParticlesAux." + excludedAuxData]
    ### ME trackParticles
    aod_items+=[ "xAOD::TrackParticleContainer#ExtraPolatedMuonsLRTTrackParticles"]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#ExtraPolatedMuonsLRTTrackParticlesAux." + excludedAuxData]
    aod_items+=[ "xAOD::TrackParticleContainer#MSOnlyExtraPolatedMuonsLRTTrackParticles"]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#MSOnlyExtraPolatedMuonsLRTTrackParticlesAux." + excludedAuxData]


    ### stau
    aod_items+=[ "xAOD::TrackParticleContainer#CombinedStauTrackParticles"]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#CombinedStauTrackParticlesAux." + excludedAuxData]
    aod_items+=[ "xAOD::TrackParticleContainer#ExtrapolatedStauTrackParticles"]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#ExtrapolatedStauTrackParticlesAux." + excludedAuxData]
    aod_items+=[ "xAOD::MuonContainer#Staus" ]
    aod_items+=[ "xAOD::MuonAuxContainer#StausAux." + excludedAuxData ]
    aod_items+=[ "xAOD::SlowMuonContainer#SlowMuons" ]
    aod_items+=[ "xAOD::SlowMuonAuxContainer#SlowMuonsAux." ]

    # +++++ ESD +++++

    # Tracks 
    esd_items =["TrackCollection#ExtrapolatedMuonTracks"] 
    esd_items+=["TrackCollection#CombinedMuonTracks"]
    esd_items+=["TrackCollection#MSOnlyExtrapolatedTracks"]
  
    if flags.Muon.runCommissioningChain:
         esd_items+=["TrackCollection#EMEO_MSOnlyExtrapolatedTracks"]

    # Truth    
    if flags.Input.isMC:
        esd_items =["DetailedTrackTruthCollection#ExtrapolatedMuonTracksTruth"] 
        esd_items+=["DetailedTrackTruthCollection#CombinedMuonTracksTruth"]
        esd_items+=["DetailedTrackTruthCollection#ExtrapolatedMuonTrackParticlesTruth"]
        esd_items+=["DetailedTrackTruthCollection#CombinedMuonTrackParticlesTruth"]

    if flags.MuonCombined.doCosmicSplitTracks:
        esd_items+=["TrackCollection#Combined_Tracks_split"]
        esd_items+=["TrackCollection#Tracks_split"]
        esd_items+=["Rec::TrackParticleContainer#TrackParticleCandidate_split"]

    esd_items += aod_items
    
    result = addToESD(flags,esd_items)
    result.merge(addToAOD(flags, aod_items))

    # Leaving in for the moment, because we might still need this. Will remove once it's confirmed we don't
    # if flags.Output.doWriteESD:
        # 
        # if runOnESD:
        #     # Need to make it possible to write Object ... so rename read containers
        #     from AthenaCommon.Logging import log
        #     from SGComps.AddressRemappingConfig import AddressRemappingCfg
        #     rename_maps = list(map(lambda item:'%s->%s' % (item, 'old_'+item.split('#')[1]), esd_items))
        #     log.info('Since we are running on ESD, rename inputs as follows:'+str(rename_maps))
        #     cfg.merge( AddressRemappingCfg(rename_maps) )
    
    return result

def MuonCombinedReconstructionCfg(flags):
    result = ComponentAccumulator()

    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    result.merge( TrackingGeometryCondAlgCfg(flags) )

    muon_edm_helper_svc = CompFactory.Muon.MuonEDMHelperSvc("MuonEDMHelperSvc")
    result.addService( muon_edm_helper_svc )

    # Set up to read Tracks.
    from TrkConfig.TrackCollectionReadConfig import TrackCollectionReadCfg
    result.merge (TrackCollectionReadCfg (flags, 'Tracks'))
    
    result.merge( MuonCombinedInDetCandidateAlgCfg(flags) )
    result.merge( MuonCombinedMuonCandidateAlgCfg(flags) )
     
    if flags.Detector.GeometryID and flags.InDet.Tracking.doR3LargeD0:
        result.merge( MuonCombinedInDetCandidateAlg_LRTCfg(flags) )

    if flags.MuonCombined.doStatisticalCombination or flags.MuonCombined.doCombinedFit:
        result.merge( MuonCombinedAlgCfg(flags) )

    if flags.MuonCombined.doCombinedFit:
        result.merge(MuonInDetToMuonSystemExtensionAlgCfg(flags))

    if flags.MuonCombined.doMuGirl:
        result.merge(MuonInsideOutRecoAlgCfg(flags, name="MuonInsideOutRecoAlg") ) 
        if flags.MuonCombined.doMuGirlLowBeta:
            result.merge(MuGirlStauAlgCfg)
        if flags.Detector.GeometryID and flags.InDet.Tracking.doR3LargeD0:
            result.merge( MuGirlAlg_LRTCfg(flags) )

    if flags.MuonCombined.doCaloTrkMuId:
        result.merge( MuonCaloTagAlgCfg(flags) )
        
    if flags.MuonCombined.doMuonSegmentTagger:
        result.merge( MuonSegmentTagAlgCfg(flags) )
    if flags.Muon.runCommissioningChain:
        result.merge(MuonCombinedMuonCandidateAlg_EMEO_Cfg(flags))
        result.merge(MuonCreatorAlg_EMEO(flags))
    # runs over outputs and create xAODMuon collection
    acc = MuonCreatorAlgCfg(flags)
    result.merge(acc)
    
    if flags.MuonCombined.doMuGirl and flags.MuonCombined.doMuGirlLowBeta:
        # Has to be at end if not using sequencer. If we drop this requirement, can be moved above
        result.merge( StauCreatorAlgCfg(flags) )

    # post processing
    result.addEventAlgo( CompFactory.ClusterMatching.CaloClusterMatchLinkAlg("MuonTCLinks",ClustersToDecorate="MuonClusterCollection") )
    result.merge(MuonDecorationAlgsCfg(flags))
    # Setup output
    result.merge(CombinedMuonOutputCfg(flags))
    
    return result

if __name__=="__main__":
    # To run this, do e.g. 
    # python -m MuonCombinedConfig.MuonCombinedReconstructionConfig --run --threads=1
    
    from MuonConfig.MuonConfigUtils import SetupMuonStandaloneArguments, SetupMuonStandaloneCA

    args = SetupMuonStandaloneArguments()
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/21.0/v2/myESD.pool.root']
    # Keep this comment in, for easy local debugging.
    # ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonCombinedConfig/myESD_q221_unslimmedTracks.pool.root']
    
    ConfigFlags.Concurrency.NumThreads=args.threads
    ConfigFlags.Concurrency.NumConcurrentEvents=args.threads # Might change this later, but good enough for the moment.

    ConfigFlags.Output.ESDFileName=args.output
    ConfigFlags.InDet.Tracking.doR3LargeD0 = False # Not working with this input
    if args.debug:
        from AthenaCommon.Debugging import DbgStage
        if args.debug not in DbgStage.allowed_values:
            raise ValueError("Unknown debug stage, allowed values {}".format
                             (DbgStage.allowed_values))
        ConfigFlags.Exec.DebugStage = args.debug

    ConfigFlags.lock()
    ConfigFlags.dump()

    cfg = SetupMuonStandaloneCA(args,ConfigFlags)

    # "Fixes" to get this working standalone i.e. from ESD
    #Configure topocluster algorithmsm, and associated conditions
    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    cfg.merge(CaloTopoClusterCfg(ConfigFlags))
    acc = MuonCombinedReconstructionCfg(ConfigFlags)
    cfg.merge(acc)
    
    # Commented out for now as this causes a stall. FIXME
    # Should be what provides xoadMuonSegments
    # cfg.addEventAlgo(CompFactory.xAODMaker.MuonSegmentCnvAlg("MuonSegmentCnvAlg"))

    # Keep this in, since it makes debugging easier to simply uncomment and change Algo/Service name,
    # from AthenaCommon.Constants import VERBOSE
    # tmp = cfg.getEventAlgo("MuonCombinedMuonCandidateAlg")
    # tmp.OutputLevel=VERBOSE
    # tmp = cfg.getService("StoreGateSvc")
    # tmp.OutpuDumptLevel=True

    if args.threads>1 and args.forceclone:
        from AthenaCommon.Logging import log
        log.info('Forcing track building cardinality to be equal to '+str(args.threads))
        # We want to force the algorithms to run in parallel (eventually the algorithm will be marked as cloneable in the source code)
        AlgResourcePool=CompFactory.AlgResourcePool
        cfg.addService(AlgResourcePool( OverrideUnClonable=True ) )
        track_builder = acc.getPrimary()
        track_builder.Cardinality=args.threads
            
    # Need to make it possible to write Muons ... so rename read containers
    from SGComps.AddressRemappingConfig import AddressRemappingCfg

    rename_maps = [ '%s#%s->%s' % ("xAOD::MuonContainer", "Muons", "old_Muons"), 
                    '%s#%s->%s' % ("xAOD::MuonAuxContainer", "MuonsAux.", "old_MuonsAux."),
                    '%s#%s->%s' % ("xAOD::MuonContainer", "Muons.rpcHitIdentifier", "old_Muons.rpcHitIdentifier")]
    cfg.merge( AddressRemappingCfg(rename_maps) )

    # Commented, because it should be added back in very soon.
    # itemsToRecord = ["xAOD::MuonContainer#Muons", "xAOD::MuonAuxContainer#MuonsAux.-DFCommonMuonsTight.-DFCommonGoodMuon.-DFCommonMuonsMedium.-DFCommonMuonsLoose"]
    # SetupMuonStandaloneOutput(cfg, ConfigFlags, itemsToRecord)
    cfg.printConfig(withDetails = True, summariseProps=True)
    # f=open("MuonCombinedReconstruction.pkl","wb")
    # cfg.store(f)
    # f.close()
    
    if args.run:
        sc = cfg.run(20)
        if not sc.isSuccess():
            import sys
            sys.exit("Execution failed")
        
