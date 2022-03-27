# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType


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
    kwargs.setdefault("usePRDs", False )    
    alg = CompFactory.MuonCombinedInDetExtensionAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result
def LRT_MuonCaloTagAlgCfg(flags, name="MuonCaloTagAlg_LRT", **kwargs):
    kwargs.setdefault("TagMap","caloTagMap_LRT")
    kwargs.setdefault("InDetCandidateLocation", "TrackParticleCandidateLRT")
    return  MuonCaloTagAlgCfg(flags,name, **kwargs)

def MuonSegmentTagAlgCfg(flags, name="MuonSegmentTagAlg", **kwargs ):
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonSegmentTagToolCfg
    result = MuonSegmentTagToolCfg(flags)
    muon_segment_tag_tool = result.getPrimary()
    kwargs.setdefault("MuonSegmentTagTool", muon_segment_tag_tool)
    kwargs.setdefault("InDetCandidateLocation", 
                      "InDetCandidates" if not flags.MuonCombined.doCombinedFit else "InDetCandidatesSystemExtened")
    alg = CompFactory.MuonSegmentTagAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result
def LRT_MuonSegmentTagAlgCfg(flags, name="MuonSegmentTagAlg_LRT", **kwargs):
    kwargs.setdefault("TagMap","segmentTagMap_LRT")
    kwargs.setdefault("InDetCandidateLocation", "TrackParticleCandidateLRT" 
                        if not flags.MuonCombined.doCombinedFit else "InDetCandidateLRT_SystemExtended")   
    return MuonSegmentTagAlgCfg(flags, name,**kwargs)

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
    kwargs.setdefault("TGCPrepDataLocation",    
                      'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')
    kwargs.setdefault("InDetCandidateLocation", 
                      "InDetCandidates" if not flags.MuonCombined.doCombinedFit else "InDetCandidatesSystemExtened")
    
    alg = CompFactory.MuonCombinedInDetExtensionAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def LRT_MuGirlAlgCfg(flags, name="MuGirlAlg_LRT",**kwargs):
    kwargs.setdefault("TagMap","MuGirlMap_LRT")
    kwargs.setdefault("METrackCollection","MuGirlMETracks_LRT")
    kwargs.setdefault("SegmentCollection","MuGirlSegments_LRT")
    kwargs.setdefault("CombinedTrackCollection","MuGirlCombinedMuonContainerLRT")
    kwargs.setdefault("InDetCandidateLocation","TrackParticleCandidateLRT" 
                            if not flags.MuonCombined.doCombinedFit else "InDetCandidateLRT_SystemExtended")
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
    kwargs.setdefault("TGCPrepDataLocation",    
                      'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')
    kwargs.setdefault("CombinedTrackCollection","MuGirlStauCombinedTracks")
    kwargs.setdefault("METrackCollection","")
    kwargs.setdefault("SegmentCollection","MuGirlStauSegments")
    kwargs.setdefault("InDetCandidateLocation", 
                      "InDetCandidates" if not flags.MuonCombined.doCombinedFit else "InDetCandidatesStaus")
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
    if flags.Beam.Type is BeamType.Cosmics:
        tool_kwargs.setdefault("ExtrapolationStrategy", 1 )

    acc = MuonCandidateToolCfg(flags, **tool_kwargs)
    kwargs.setdefault("MuonCandidateTool",acc.popPrivateTools())
    result.merge(acc)

    alg = CompFactory.MuonCombinedMuonCandidateAlg(name,**kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def EMEO_MuonCombinedMuonCandidateAlgCfg(flags, name = "MuonCombinedMuonCandidateAlg_EMEO"):
    result = ComponentAccumulator()
    
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import EMEO_CombinedMuonTrackBuilderCfg
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCandidateToolCfg

    track_builder = result.getPrimaryAndMerge(EMEO_CombinedMuonTrackBuilderCfg(flags))   
    acc = MuonCandidateToolCfg(flags,
                               name = "MuonCandidateTool_EMEO",
                               TrackBuilder = track_builder,
                               Commissioning = True)

    candidate_tool = result.getPrimaryAndMerge(acc)
    alg = CompFactory.MuonCombinedMuonCandidateAlg(name,
                                                   MuonCandidateTool = candidate_tool,
                                                   MuonSpectrometerTrackParticleLocation = "EMEO_MuonSpectrometerTrackParticles",
                                                   MuonCandidateLocation = "MuonCandidates_EMEO",
                                                   MSOnlyExtrapolatedTrackLocation= "EMEO_MSOnlyExtrapolatedTracks" )
    result.addEventAlgo( alg, primary=True )

    return result


def MuonCombinedInDetCandidateAlgCfg(flags, name="MuonCombinedInDetCandidateAlg",**kwargs ):
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCombinedInDetDetailedTrackSelectorToolCfg    
    result = MuonCombinedInDetDetailedTrackSelectorToolCfg(flags)
    kwargs.setdefault("TrackSelector", result.getPrimary() )
    if flags.MuonCombined.doSiAssocForwardMuons and flags.InDet.Tracking.doForwardTracks:
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
    kwargs.setdefault("ExtendBulk", not  flags.MuonCombined.doCombinedFit)
    
    if flags.Reco.EnableCaloExtension and not flags.Muon.MuonTrigger:
        kwargs.setdefault("CaloExtensionLocation", ["ParticleCaloExtension"] )
 
    alg = CompFactory.MuonCombinedInDetCandidateAlg(name, **kwargs)
    result.addEventAlgo( alg, primary=True )
    return result

def MuonInDetToMuonSystemExtensionAlgCfg(flags, name ="MuonInDetToMuonSystemExtensionAlg",  **kwargs):
    result = ComponentAccumulator()
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonSystemExtensionToolCfg
    muon_ext_tool = result.getPrimaryAndMerge(MuonSystemExtensionToolCfg(flags))
    kwargs.setdefault("MuonSystemExtensionTool", muon_ext_tool)
    kwargs.setdefault("WriteStauCandidates", "InDetCandidatesStaus")
    alg = CompFactory.MuonInDetToMuonSystemExtensionAlg(name,**kwargs) 
    result.addEventAlgo( alg, primary=True )
    return result

def LRT_MuonInDetToMuonSystemExtensionAlgCfg(flags, name ="MuonInDetToMuonSystemExtensionAlg_LRT",  **kwargs):
    kwargs.setdefault("WriteStauCandidates", "")
    kwargs.setdefault("WriteInDetCandidates", "InDetCandidateLRT_SystemExtended")
    kwargs.setdefault("InputInDetCandidates", "TrackParticleCandidateLRT")
    kwargs.setdefault("CombinedTagMap", "muidcoTagMap_LRT")
    return MuonInDetToMuonSystemExtensionAlgCfg(flags,name, **kwargs)

def LRT_MuonCombinedInDetCandidateAlgCfg(flags, name="MuonCombinedInDetCandidateAlg_LRT",**kwargs ):
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCombinedInDetDetailedTrackSelectorToolCfg, MuonCombinedInDetDetailedTrackSelectorTool_LRTCfg
    result = ComponentAccumulator()   
    sel_acc = MuonCombinedInDetDetailedTrackSelectorTool_LRTCfg(flags)
    kwargs.setdefault("TrackSelector", sel_acc.getPrimary() )
    result.merge(sel_acc)
    ### Use the Standard Track particle container in cases where no separate containters will be
    ### saved for the LRT tracking
    kwargs.setdefault("TrackParticleLocation",["InDetLargeD0TrackParticles"])
    kwargs.setdefault("InDetCandidateLocation","TrackParticleCandidateLRT")
    kwargs.setdefault("DoSiliconAssocForwardMuons", False)
    fwd_acc = MuonCombinedInDetDetailedTrackSelectorToolCfg(flags, "MuonCombinedInDetDetailedForwardTrackSelectorTool", nHitSct=0)

    kwargs.setdefault("InDetForwardTrackSelector", fwd_acc.getPrimary() )
    if flags.Reco.EnableCaloExtension and not flags.Muon.MuonTrigger:
        kwargs.setdefault("CaloExtensionLocation", ["ParticleCaloExtension_LRT"] )
    cand_alg = MuonCombinedInDetCandidateAlgCfg(flags, name, **kwargs)
    result.merge(cand_alg)    
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

def LRT_MuonCombinedAlgCfg(flags,name="MuonCombinedAlg_LRT",**kwargs ):
    kwargs.setdefault("InDetCandidateLocation", "TrackParticleCandidateLRT")
    kwargs.setdefault("CombinedTagMaps", ["muidcoTagMap_LRT","stacoTagMap_LRT"])
    kwargs.setdefault("MuidCombinedTracksLocation", "MuidCombinedTracks_LRT")
    kwargs.setdefault("MuidMETracksLocation", "MuidMETracks_LRT")    
    return MuonCombinedAlgCfg(flags,name,**kwargs)


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

def LRT_MuonCreatorAlgCfg(flags, name="MuonCreatorAlg_LRT",**kwargs ):
    result = ComponentAccumulator()
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCreatorToolCfg
    creatorTool = result.getPrimaryAndMerge(MuonCreatorToolCfg(flags, "MuonCreatorTool_LRT"))
    kwargs.setdefault("MuonCreatorTool", creatorTool)
    ### In cases we want to switch them off we should add the flags here
    tag_maps = ["muidcoTagMap_LRT","segmentTagMap_LRT", "caloTagMap_LRT", "stacoTagMap_LRT","MuGirlMap_LRT"]
    kwargs.setdefault("TagMaps", tag_maps)
    kwargs.setdefault("MuonContainerLocation", "MuonsLRT")
    kwargs.setdefault("ExtrapolatedLocation", "ExtraPolatedMuonsLRT")
    kwargs.setdefault("MSOnlyExtrapolatedLocation", "MSOnlyExtrapolatedMuonsLRT")
    kwargs.setdefault("CombinedLocation", "CombinedMuonsLRT")
    kwargs.setdefault("SegmentContainerName", "MuonSegments_LRT")
    kwargs.setdefault("TrackSegmentContainerName", "TrkMuonSegments_LRT")
    kwargs.setdefault("BuildSlowMuon", False)
    kwargs.setdefault("MakeClusters", False)
    kwargs.setdefault("ClusterContainerName", "")
    return MuonCreatorAlgCfg(flags, name = name, **kwargs)

def EMEO_MuonCreatorAlgCfg(flags, name = "MuonCreatorAlg_EMEO", **kwargs ):
    muon_maps = ["MuonCandidates_EMEO"]
    combined_maps = []
    kwargs.setdefault("TagMaps", combined_maps)
    kwargs.setdefault("MuonCandidateLocation", muon_maps)
    kwargs.setdefault("MuonContainerLocation", "EMEO_Muons")
    kwargs.setdefault("ExtrapolatedLocation", "EMEO_ExtrapolatedMuon")
    kwargs.setdefault("MSOnlyExtrapolatedLocation", "EMEO_MSOnlyExtrapolatedMuon")   
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
    if flags.Detector.GeometryID and flags.InDet.Tracking.doR3LargeD0:
        tp_coll += ["CombinedMuonsLRTTrackParticles", 
                     "ExtraPolatedMuonsLRTTrackParticles",
                    "MSOnlyExtrapolatedMuonsLRTTrackParticles"]
        track_coll +=  ["CombinedMuonsLRTTracks", 
                        "ExtraPolatedMuonsLRTTracks",
                        "MSOnlyExtrapolatedMuonsLRTTracks"]
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
    if flags.Detector.GeometryID and flags.InDet.Tracking.doR3LargeD0:
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

    particle_col, trk_col = GetCombinedTrkContainers(flags)
    if flags.Detector.EnableCalo: 
        aod_items+=["xAOD::CaloClusterContainer#MuonClusterCollection"]
        aod_items+=["xAOD::CaloClusterAuxContainer#MuonClusterCollectionAux."]
        aod_items+=["CaloClusterCellLinkContainer#MuonClusterCollection_links"]

    # Adding the xAOD content by default
    excludedAuxData = '-clusterAssociation'
    aod_items+=[ "xAOD::TrackParticleContainer#"+col for col in particle_col]
    aod_items+=[ "xAOD::TrackParticleAuxContainer#"+col+"Aux." +  excludedAuxData for col in particle_col]
    aod_items+=[ "xAOD::MuonContainer#Muons" ]
    aod_items+=[ "xAOD::MuonContainer#MuonsLRT"]

    # FIXME! Next two lines are hack to remove derivation framework variables that are added by DRAW building and are supposed to be transient
    wp_decors = ["DFCommonMuonsTight", "DFCommonGoodMuon","DFCommonMuonsMedium", "DFCommonMuonsLoose"]
    excludedMuonAuxData = ".-"+".-".join(iso_vars+wp_decors)
    aod_items+=[ "xAOD::MuonAuxContainer#MuonsAux" + excludedMuonAuxData ]
    aod_items+=[ "xAOD::MuonAuxContainer#MuonsLRTAux" + excludedMuonAuxData] 

    ### stau
    aod_items+=[ "xAOD::MuonContainer#Staus" ]
    aod_items+=[ "xAOD::MuonAuxContainer#StausAux." + excludedAuxData ]
    aod_items+=[ "xAOD::SlowMuonContainer#SlowMuons" ]
    aod_items+=[ "xAOD::SlowMuonAuxContainer#SlowMuonsAux." ]

    # +++++ ESD +++++

    # Tracks 
    esd_items = ["TrackCollection#"+col for col in trk_col]
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
    
    do_LRT = flags.Detector.GeometryID and flags.InDet.Tracking.doR3LargeD0
    if do_LRT: result.merge( LRT_MuonCombinedInDetCandidateAlgCfg(flags) )

    if flags.MuonCombined.doStatisticalCombination or flags.MuonCombined.doCombinedFit:
        result.merge( MuonCombinedAlgCfg(flags) )
        if do_LRT: result.merge(LRT_MuonCombinedAlgCfg(flags))

    ### Perform system extensions on ID tracks where MuidCo did not succeed
    if flags.MuonCombined.doCombinedFit:
        result.merge(MuonInDetToMuonSystemExtensionAlgCfg(flags))
        if do_LRT: result.merge(LRT_MuonInDetToMuonSystemExtensionAlgCfg(flags))
    if flags.MuonCombined.doMuGirl:
        ### Use only ID tracks rejected by MuidCo
        result.merge(MuonInsideOutRecoAlgCfg(flags, name="MuonInsideOutRecoAlg",
                                                    InDetCandidateLocation = "InDetCandidates" 
                                                     if not flags.MuonCombined.doCombinedFit else "InDetCandidatesSystemExtened") ) 
        if flags.MuonCombined.doMuGirlLowBeta:
            ### Use the InDetCandidateStaus as InDetCandidates as they've also the extensions
            ### from the MuidCo tracks
            result.merge(MuGirlStauAlgCfg(flags))
        if do_LRT: result.merge(LRT_MuGirlAlgCfg(flags) )

    if flags.MuonCombined.doCaloTrkMuId:
        result.merge( MuonCaloTagAlgCfg(flags) )
        if do_LRT: result.merge(LRT_MuonCaloTagAlgCfg(flags))
        
    if flags.MuonCombined.doMuonSegmentTagger:
        result.merge( MuonSegmentTagAlgCfg(flags) )
        if do_LRT: result.merge(LRT_MuonSegmentTagAlgCfg(flags))
    
    if flags.Muon.runCommissioningChain:
        result.merge(EMEO_MuonCombinedMuonCandidateAlgCfg(flags))
        result.merge(EMEO_MuonCreatorAlgCfg(flags))
    
    # runs over outputs and create xAODMuon collection
    result.merge(MuonCreatorAlgCfg(flags))
    if do_LRT: result.merge(LRT_MuonCreatorAlgCfg(flags))
    
    if flags.MuonCombined.doMuGirl and flags.MuonCombined.doMuGirlLowBeta:
        # Has to be at end if not using sequencer. If we drop this requirement, can be moved above
        result.merge( StauCreatorAlgCfg(flags) )

    # FIXME
    # I see errors related to an unconfigured public tool. This is a quick fix, but should really move to where it is called.
    from MuonCombinedConfig.MuonCombinedRecToolsConfig import MuonCombinedTrackSummaryToolCfg
    result.addPublicTool( result.popToolsAndMerge( MuonCombinedTrackSummaryToolCfg(flags) ) )

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

    # Keep this commented in for now until ATLASRECTS-6858 is fixed
    #ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonRecRTT/Run2/ESD/OUT_ESD.root']
    ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/21.0/v2/myESD.pool.root']
    
    ConfigFlags.Concurrency.NumThreads=args.threads
    ConfigFlags.Concurrency.NumConcurrentEvents=args.threads # Might change this later, but good enough for the moment.

    ConfigFlags.Output.ESDFileName=args.output
    ConfigFlags.InDet.Tracking.doR3LargeD0 = False # Not working with this input
    ConfigFlags.Muon.useTGCPriorNextBC = False
    if args.debug:
        from AthenaCommon.Debugging import DbgStage
        if args.debug not in DbgStage.allowed_values:
            raise ValueError("Unknown debug stage, allowed values {}".format
                             (DbgStage.allowed_values))
        ConfigFlags.Exec.DebugStage = args.debug

    ConfigFlags.lock()
    ConfigFlags.dump()

    cfg = SetupMuonStandaloneCA(args,ConfigFlags)
    from MuonConfig.MuonPrepDataConvConfig import MuonPrepDataConvCfg
    cfg.merge(MuonPrepDataConvCfg(ConfigFlags))
   
    # "Fixes" to get this working standalone i.e. from ESD
    #Configure topocluster algorithmsm, and associated conditions
    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    cfg.merge(CaloTopoClusterCfg(ConfigFlags))
    from TrackToCalo.CaloExtensionBuilderAlgCfg import CaloExtensionBuilderCfg
    cfg.merge(CaloExtensionBuilderCfg(ConfigFlags))

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
        
