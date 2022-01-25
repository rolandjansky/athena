# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Defines the shared tools used in muon identification
# Based on :
# https://gitlab.cern.ch/atlas/athena/blob/release/22.0.8/Reconstruction/MuonIdentification/MuonCombinedRecExample/python/MuonCombinedTools.py
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
from MuonConfig.MuonRecToolsConfig import MuonEDMPrinterTool
from TrkConfig.AtlasExtrapolatorToolsConfig import AtlasRKPropagatorCfg, MuonCombinedPropagatorCfg, AtlasNavigatorCfg, AtlasEnergyLossUpdatorCfg

#FIXME
GeV = 1000
mm = 1

def MuonCombinedTrackSummaryToolCfg(flags, name="CombinedMuonTrackSummary", **kwargs):
    # Based on https://gitlab.cern.ch/atlas/athena/blob/release/22.0.8/Reconstruction/MuonIdentification/MuonCombinedRecExample/python/CombinedMuonTrackSummary.py
    from TrkConfig.AtlasTrackSummaryToolConfig import AtlasTrackSummaryToolCfg
    result = AtlasTrackSummaryToolCfg(flags, name=name)
    result._privateTools.InDetSummaryHelperTool.name = 'CombinedMuonIDSummaryHelper' # Horrible hack, but just need to change name to match old config
    return result

def MuonTrackToVertexCfg(flags, name = 'MuonTrackToVertexTool', **kwargs ):
    acc = ComponentAccumulator()
    if 'Extrapolator' not in kwargs:
        accExtrapolator = AtlasExtrapolatorCfg(flags, 'AtlasExtrapolator')
        atlasExtrapolator = accExtrapolator.popPrivateTools()
        acc.merge(accExtrapolator)
        kwargs.setdefault('Extrapolator', atlasExtrapolator)
    acc.setPrivateTools(CompFactory.Reco.TrackToVertex( name, **kwargs))
    return acc

def MuonCombinedInDetDetailedTrackSelectorToolCfg(flags, name="MuonCombinedInDetDetailedTrackSelectorTool",**kwargs):
    if flags.Beam.Type == 'collisions':
        kwargs.setdefault("pTMin", 2000 )
        kwargs.setdefault("nHitBLayer", 0 )
        kwargs.setdefault("nHitBLayerPlusPix", 0 )
        kwargs.setdefault("nHitTrt", 0 )
        kwargs.setdefault("useTrackQualityInfo", False )
        if flags.Muon.SAMuonTrigger:
            kwargs.setdefault("IPd0Max", 19999.0 )
            kwargs.setdefault("IPz0Max", 19999.0 )
            kwargs.setdefault("z0Max", 19999.0 )
            kwargs.setdefault("useTrackSummaryInfo", False )
            kwargs.setdefault("nHitPix", 0 )
            kwargs.setdefault("nHitSct", 0 )
            kwargs.setdefault("nHitSi", 0 )
        else:
            kwargs.setdefault("IPd0Max", 50.0 )
            kwargs.setdefault("IPz0Max", 9999.0 )
            kwargs.setdefault("z0Max", 9999.0 )
            kwargs.setdefault("useTrackSummaryInfo", True )
            kwargs.setdefault("nHitPix", 1 )
            kwargs.setdefault("nHitSct", 3 )
            kwargs.setdefault("nHitSi", 4 )
    else:
        kwargs.setdefault("pTMin", 500 )
        kwargs.setdefault("IPd0Max", 19999.0 )
        kwargs.setdefault("IPz0Max", 19999.0 )
        kwargs.setdefault("z0Max", 19999.0 )
        kwargs.setdefault("useTrackSummaryInfo", False )
        kwargs.setdefault("useTrackQualityInfo", False )


    result = AtlasExtrapolatorCfg(flags)
    extrapolator = result.getPrimary()
    kwargs.setdefault("Extrapolator", extrapolator )

    kwargs.setdefault("TrackSummaryTool", "")

    # Has two CondKeys
    # SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

    # FIXME - let's put this someplace central?
    result.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Beampos","/Indet/Beampos", className='AthenaAttributeList'))
    result.addCondAlgo(CompFactory.BeamSpotCondAlg("BeamSpotCondAlg"))

    # SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
    # FIXME - handle this ^
    
    tool = CompFactory.InDet.InDetDetailedTrackSelectorTool(name,**kwargs)
    result.setPrivateTools(tool)
    return result

def MuonCombinedInDetDetailedTrackSelectorTool_LRTCfg(flags, name='MuonCombinedInDetDetailedTrackSelectorTool_LRT', **kwargs):
    kwargs.setdefault("pTMin", 2000 )
    kwargs.setdefault("IPd0Max", 1.e4 )
    kwargs.setdefault("IPz0Max",  1.e4 )
    kwargs.setdefault("z0Max",  1.e4  )
    kwargs.setdefault("useTrackSummaryInfo", True )
    kwargs.setdefault("nHitBLayer", 0 )
    kwargs.setdefault("nHitPix", 0 )
    kwargs.setdefault("nHitBLayerPlusPix", 0 )
    kwargs.setdefault("nHitSct", 4 )
    kwargs.setdefault("nHitSi", 4 )
    kwargs.setdefault("nHitTrt", 0 )
    kwargs.setdefault("useTrackQualityInfo", False )
    return MuonCombinedInDetDetailedTrackSelectorToolCfg(flags, name,**kwargs)

def MuonCombinedParticleCreatorCfg(flags, name="MuonCombinedParticleCreator",**kwargs):
    result = ComponentAccumulator()    
    if flags.Muon.SAMuonTrigger:
        from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
        acc = MuonTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", acc.popPrivateTools())
        result.merge(acc)
    else:
        acc = MuonCombinedTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", acc.getPrimary() ) 
        result.merge (acc)

    if 'TrackToVertex' not in kwargs :
        kwargs.setdefault('TrackToVertex',result.popToolsAndMerge(MuonTrackToVertexCfg(flags)))

    kwargs.setdefault("KeepAllPerigee",True )
    kwargs.setdefault("MuonSummaryTool", CompFactory.Muon.MuonHitSummaryTool("MuonHitSummaryTool"))
    if flags.Beam.Type=="cosmics":
        kwargs.setdefault("PerigeeExpression","Origin")
    kwargs.setdefault("IBLParameterSvc", "IBLParameterSvc" if flags.Detector.GeometryID else "")
    tool = CompFactory.Trk.TrackParticleCreatorTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result
 

def InDetCandidateToolCfg(flags, name="InDetCandidateTool",**kwargs ):
    result = MuonCombinedInDetDetailedTrackSelectorToolCfg(flags)
    kwargs.setdefault("TrackSelector", result.getPrimary() )
    tool = CompFactory.MuonCombined.InDetCandidateTool(name,**kwargs)
    result.addPublicTool(tool)
    result.setPrivateTools(tool)
    return result
 

def MuonInDetForwardCandidateToolCfg(flags,  name = 'MuonInDetForwardCandidateTool', **kwargs):
    result = MuonCombinedInDetDetailedTrackSelectorToolCfg(flags, "MuonCombinedInDetDetailedForwardTrackSelectorTool", nHitSct=0)
    
    acc=InDetCandidateToolCfg(flags, name = "InDetForwardCandidateTool", 
                                    TrackSelector = result.getPrimary(), 
                                    FlagCandidatesAsSiAssociated=True )
    tool = acc.getPrimary()
    result.merge(acc)
    result.addPublicTool(tool)
    result.setPrivateTools(tool)
    return result # FIXME - is this and the above, actually used?

def MuonCaloParticleCreatorCfg(flags, name = "MuonCaloParticleCreator", **kwargs):
    from InDetConfig.TrackRecoConfig import TrackToVertexCfg
    result = ComponentAccumulator()
    if "TrackToVertex" not in kwargs:
        kwargs["TrackToVertex"] = result.popToolsAndMerge(TrackToVertexCfg(flags))

    if flags.Muon.SAMuonTrigger:
        from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonTrackSummaryToolCfg(flags)))
    else:
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonCombinedTrackSummaryToolCfg(flags)))
    kwargs.setdefault("KeepAllPerigee", True) 
    kwargs.setdefault("PerigeeExpression", "Origin")
    kwargs.setdefault("IBLParameterSvc", "IBLParameterSvc" if flags.Detector.GeometryID else "")
    track_particle_creator = CompFactory.Trk.TrackParticleCreatorTool(name="MuonCaloParticleCreator",**kwargs)
    result.setPrivateTools(track_particle_creator)
    return result

def MuonCaloEnergyToolCfg(flags,  name = "MuonCaloEnergyTool", **kwargs):
    from TrackToCalo.TrackToCaloConfig import ParticleCaloCellAssociationToolCfg, ParticleCaloExtensionToolCfg

    result = ParticleCaloCellAssociationToolCfg(flags)
    particle_calo_cell_association_tool = result.getPrimary()

    particle_calo_extension_tool = result.getPrimaryAndMerge(ParticleCaloExtensionToolCfg(flags))

    track_particle_creator = result.getPrimaryAndMerge(MuonCaloParticleCreatorCfg(flags))

    muonCaloEnergyTool = CompFactory.Rec.MuonCaloEnergyTool(name="MuonCaloEnergy", ParticleCaloExtensionTool = particle_calo_extension_tool,
                                                 ParticleCaloCellAssociationTool = particle_calo_cell_association_tool,
                                                 TrackParticleCreator = track_particle_creator)
    result.setPrivateTools(muonCaloEnergyTool)
    return result


def MuonMaterialProviderToolCfg(flags,  name = "MuonMaterialProviderTool", **kwargs):
    # workaround as long as public tool is required
    result = AtlasExtrapolatorCfg(flags)
    atlas_extrapolator = result.popPrivateTools()
    kwargs.setdefault("Extrapolator", atlas_extrapolator )

    muonCaloEnergyTool = result.getPrimaryAndMerge(MuonCaloEnergyToolCfg(flags))

    useCaloEnergyMeas = True
    if flags.Muon.MuonTrigger:
        useCaloEnergyMeas = False

    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    acc  = TrackingGeometryCondAlgCfg(flags)
    result.merge( acc )

    ELossUpdatorTool = result.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags))

    #FIXME AtlasFieldCacheCondObj?

    tool = CompFactory.Trk.TrkMaterialProviderTool(name = name,
                                                   MuonCaloEnergyTool = muonCaloEnergyTool,
                                                   UseCaloEnergyMeasurement = useCaloEnergyMeas,
                                                   EnergyLossUpdator = ELossUpdatorTool,
                                                   TrackingGeometryReadKey= acc.getPrimary().TrackingGeometryWriteKey)
    result.setPrivateTools(tool)
    return result 


def MuonCreatorToolCfg(flags, name="MuonCreatorTool", **kwargs):
    from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg
    result = ComponentAccumulator()
    # Not explicitly setting up MuonIdHelperSvc, nor MuonEDMHelperSvc
    kwargs.setdefault("Printer", MuonEDMPrinterTool(flags) )
    # Not explicitly setting up MuonPrintingTool
    acc = ParticleCaloExtensionToolCfg(flags,StartFromPerigee=True)
    kwargs.setdefault("ParticleCaloExtensionTool", acc.getPrimary() )
    result.merge(acc)

    acc = MuonCombinedParticleCreatorCfg(flags)
    kwargs.setdefault("TrackParticleCreator", acc.getPrimary() )
    result.merge(acc)

    acc = ParticleCaloExtensionToolCfg(flags,StartFromPerigee=True)
    kwargs.setdefault("ParticleCaloExtensionTool", acc.getPrimary() )
    result.merge(acc)
    
    from MuonConfig.MuonRecToolsConfig import MuonAmbiProcessorCfg
    acc = MuonAmbiProcessorCfg(flags)
    kwargs.setdefault("AmbiguityProcessor", acc.popPrivateTools())
    result.merge(acc)

    kwargs.setdefault("Propagator", result.popToolsAndMerge( AtlasRKPropagatorCfg(flags) ) )
    # Not explicitly setting up MuonDressingTool (but probably should FIXME)
    # Not explicitly setting up MomentumBalanceTool nor ScatteringAngleTool
    # Not explicitly setting up MuonSegmentConverterTool (but probably should FIXME)
    # Not explicitly setting up MeanMDTdADCTool (but probably should FIXME)

    kwargs.setdefault( "CaloMaterialProvider", result.popToolsAndMerge( MuonMaterialProviderToolCfg(flags) ) )

    # Not explicitly setting up TrackSegmentAssociationTool 

    kwargs.setdefault("TrackQuery",   result.popToolsAndMerge(MuonTrackQueryCfg(flags)) )

    if flags.Muon.SAMuonTrigger:
        from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
        acc = MuonTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", acc.popPrivateTools())
        result.merge(acc)
    else:
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonCombinedTrackSummaryToolCfg(flags)))

    if flags.Muon.MuonTrigger:
        kwargs.setdefault("MuonSelectionTool", "")
        kwargs.setdefault("UseCaloCells", False)
        kwargs.setdefault("TrackSegmentAssociationTool", "")

    # This tool needs MuonScatteringAngleSignificanceTool... which in turn needs TrackingVolumeSvc.
    # FIXME - probably this should be someplace central.
    trackingVolSvc = CompFactory.Trk.TrackingVolumesSvc(name="TrackingVolumesSvc")
    result.addService(trackingVolSvc)

    tool = CompFactory.MuonCombined.MuonCreatorTool(name,**kwargs)
    result.setPrivateTools(tool)
    return result 

def ExtrapolateMuonToIPToolCfg(flags, name="ExtrapolateMuonToIPTool", **kwargs):
    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", result.popPrivateTools() )
    from TrkConfig.AtlasExtrapolatorConfig import MuonExtrapolatorCfg
    kwargs.setdefault("MuonExtrapolator", result.popToolsAndMerge( MuonExtrapolatorCfg(flags) ) )

    if flags.Muon.MuonTrigger:
        from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonTrackSummaryToolCfg(flags)))
    else:
        kwargs.setdefault("TrackSummaryTool", result.popToolsAndMerge(MuonCombinedTrackSummaryToolCfg(flags)))
    result.setPrivateTools(CompFactory.ExtrapolateMuonToIPTool(name,**kwargs))
    return result

def MuonCandidateToolCfg(flags, name="MuonCandidateTool",**kwargs):
    from MuonConfig.MuonRecToolsConfig import MuonAmbiProcessorCfg

    kwargs.setdefault("Printer", MuonEDMPrinterTool(flags) )

    result = CombinedMuonTrackBuilderCfg(flags, name="CombinedMuonTrackBuilder")
    kwargs.setdefault("TrackBuilder", result.popPrivateTools() )
    #   Why was this dependent on cosmics? will now always create this 
    #   if flags.Beam.Type=="cosmics":
    if flags.Muon.MuonTrigger and flags.Beam.Type!="cosmics":
        #trigger definitely only uses the ExtrapolateToIPtool in cosmics mode
        kwargs.setdefault("TrackExtrapolationTool", "")
    else:
        acc = ExtrapolateMuonToIPToolCfg(flags)
        extrapolator = acc.popPrivateTools()
        kwargs.setdefault("TrackExtrapolationTool", extrapolator )
        result.merge(acc)
#   if cosmics was until here

    acc = MuonAmbiProcessorCfg(flags)
    ambiguityprocessor = acc.popPrivateTools()

    kwargs.setdefault("AmbiguityProcessor", ambiguityprocessor )
    result.merge(acc)

    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
    acc = MuonTrackSummaryToolCfg(flags)
    track_summary = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("TrackSummaryTool",  track_summary)

    if flags.Beam.Type=="cosmics":
        kwargs.setdefault("ExtrapolationStrategy", 1 )
    tool = CompFactory.MuonCombined.MuonCandidateTool(name,**kwargs)
    result.setPrivateTools(tool)
    return result 

def MuonCombinedDebuggerToolCfg( flags, name="MuonCombinedDebuggerTool",**kwargs ):
    result=ComponentAccumulator()
    match_quality = CompFactory.Rec.MuonMatchQuality(TrackQuery = result.popToolsAndMerge( MuonTrackQueryCfg(flags) ) )
    kwargs.setdefault("MuonMatchQuality",match_quality )
    tool = CompFactory.MuonCombined.MuonCombinedDebuggerTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result    

def MuonCombinedToolCfg(flags, name="MuonCombinedTool",**kwargs):
    tools = []
    result=ComponentAccumulator()
    if flags.MuonCombined.doCombinedFit:
        acc = MuonCombinedFitTagToolCfg(flags)
        tool = acc.popPrivateTools()
        tools.append( tool  )
        result.merge(acc)
    if flags.MuonCombined.doStatisticalCombination and flags.Beam.Type != 'cosmics':
        acc = MuonCombinedStacoTagToolCfg(flags)
        tool = acc.popPrivateTools()
        tools.append( tool  )
        result.merge(acc)

    kwargs.setdefault("MuonCombinedTagTools", tools )
    kwargs.setdefault("MuonCombinedDebuggerTool", result.popToolsAndMerge( MuonCombinedDebuggerToolCfg(flags) ) )

    acc = MuonAlignmentUncertToolThetaCfg(flags)
    result.merge(acc)
    kwargs.setdefault("AlignmentUncertTool", result.getPublicTool('MuonAlignmentUncertToolTheta') )
 
    kwargs.setdefault("DeltaEtaPreSelection", 0.2)
    kwargs.setdefault("DeltaPhiPreSelection", 0.2)    
    tool = CompFactory.MuonCombined.MuonCombinedTool(name,**kwargs)
    result.setPrivateTools(tool)
    return result 

def MuonCombinedFitTagToolCfg(flags, name="MuonCombinedFitTagTool",**kwargs):
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("VertexContainer", "")

    result = CombinedMuonTrackBuilderCfg(flags)
    tool = result.popPrivateTools()
    kwargs.setdefault("TrackBuilder",  tool )

    kwargs.setdefault("TrackQuery",   result.popToolsAndMerge(MuonTrackQueryCfg(flags)) )

    acc = MuonMatchQualityCfg(flags)
    kwargs.setdefault("MatchQuality",           acc.popPrivateTools() )
    result.merge(acc)

    from MuonConfig.MuonRecToolsConfig import MuonTrackScoringToolCfg
    acc = MuonTrackScoringToolCfg(flags)
    kwargs.setdefault("TrackScoringTool", acc.popPrivateTools())
    result.merge(acc)

    tool = CompFactory.MuonCombined.MuonCombinedFitTagTool(name,**kwargs)
    result.setPrivateTools(tool)
    return result 
                 
def MuonCombinedStacoTagToolCfg(flags, name="MuonCombinedStacoTagTool",**kwargs):
    
    result = ComponentAccumulator()
    kwargs.setdefault("Printer", MuonEDMPrinterTool(flags) )
    kwargs.setdefault("TagTool", result.popToolsAndMerge(CombinedMuonTagTestToolCfg(flags)))
    kwargs.setdefault("Extrapolator", result.popToolsAndMerge(AtlasExtrapolatorCfg(flags)) )

    tool = CompFactory.MuonCombined.MuonCombinedStacoTagTool(name,**kwargs)
    result.setPrivateTools(tool)
    return result 

# From Combined fit tools.py

def MuidMaterialAllocatorCfg(flags, name='MuidMaterialAllocator', **kwargs): 
    kwargs.setdefault("AggregateMaterial",True)
    kwargs.setdefault("AllowReordering",False)

    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", result.popPrivateTools() )

    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    result.merge( TrackingGeometryCondAlgCfg(flags) )
    kwargs.setdefault("TrackingGeometryReadKey", "AtlasTrackingGeometry")

    tool = CompFactory.Trk.MaterialAllocator(name,**kwargs)
    result.setPrivateTools(tool)
    return result 

# and the fitter
def iPatFitterCfg(flags, name='iPatFitter', **kwargs): 
    kwargs.setdefault("AggregateMaterial",True)
    kwargs.setdefault("FullCombinedFit", True )
    result = MuidMaterialAllocatorCfg(flags)
    kwargs.setdefault("MaterialAllocator",result.popPrivateTools() )
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("MaxIterations", 15)
    if flags.Muon.SAMuonTrigger:
        from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
        acc = MuonTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", acc.popPrivateTools())
        result.merge(acc)
    else:
        acc = MuonCombinedTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", acc.getPrimary() )
        result.merge(acc)

    from TrkConfig.SolenoidalIntersectorConfig import SolenoidalIntersectorCfg
    acc = SolenoidalIntersectorCfg (flags)
    kwargs.setdefault ('SolenoidalIntersector', acc.popPrivateTools())
    result.merge (acc)

    tool = CompFactory.Trk.iPatFitter(name,**kwargs)
    result.setPrivateTools(tool)
    return result 

def iPatSLFitterCfg(flags, name='iPatSLFitter', **kwargs): 
    kwargs.setdefault("LineFit",True)
    kwargs.setdefault("LineMomentum", flags.Muon.straightLineFitMomentum )
    return iPatFitterCfg(flags, name, **kwargs)

# track cleaner configured to use the same fitter
def MuidTrackCleanerCfg(flags, name='MuidTrackCleaner', **kwargs ):
    from MuonConfig.MuonRecToolsConfig import MuonTrackCleanerCfg
    if flags.Beam.Type == 'cosmics':
        kwargs.setdefault("PullCut"     , 5.0)
        kwargs.setdefault("PullCutPhi"  , 10.0)
    else:
        kwargs.setdefault("PullCut"     , 4.0)
        kwargs.setdefault("PullCutPhi"  , 4.0)
    result = ComponentAccumulator()
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("Iterate", False)
        kwargs.setdefault("RecoverOutliers", False)
        kwargs.setdefault("Fitter", result.getPrimaryAndMerge( iPatFitterCfg(flags, 'iPatFitterClean', MaxIterations=4) ) )
    else:
        kwargs.setdefault("Fitter"      , result.getPrimaryAndMerge( iPatFitterCfg(flags) ) ) 
        kwargs.setdefault("SLFitter"    , result.getPrimaryAndMerge( iPatSLFitterCfg(flags) ) ) 
    return MuonTrackCleanerCfg(flags, name, **kwargs)
    
def MuidCaloEnergyParam(flags, name='MuidCaloEnergyParam', **kwargs ):
    kwargs.setdefault("Cosmics", flags.Beam.Type == 'cosmics' )
    return CompFactory.Rec.MuidCaloEnergyParam(name,**kwargs)

def MuidCaloEnergyMeasCfg(flags, name='MuidCaloEnergyMeas', **kwargs ):
    result = ComponentAccumulator()
    muidcaloenergyparam =  MuidCaloEnergyParam(flags)
    kwargs.setdefault("CaloParamTool",  muidcaloenergyparam)
    # FIXME! Need to setup the folders for CaloNoiseKey (which is why this needs a CA) 
    # Not sure how to do : if flags.haveRIO.Calo_on() but TBH, if the cells aren't there it will abort anyway
    kwargs.setdefault("CellContainerLocation" , "AllCalo" )
    kwargs.setdefault("NoiseThresInSigmas"    , 4. )
    tool = CompFactory.Rec.MuidCaloEnergyMeas(name,**kwargs)
    result.setPrivateTools(tool)
    return result

def MuidCaloEnergyToolParamCfg(flags, name='MuidCaloEnergyToolParam', **kwargs ):
    kwargs.setdefault("CaloParamTool", MuidCaloEnergyParam(flags) )
    kwargs.setdefault("EnergyLossMeasurement",False)
    kwargs.setdefault("Cosmics", flags.Beam.Type == 'cosmics' )

    result = ComponentAccumulator()
    result.setPrivateTools(CompFactory.Rec.MuidCaloEnergyTool(name,**kwargs))
    return result

def MuidCaloEnergyToolCfg(flags, name='MuidCaloEnergyTool', **kwargs ):
    result = MuidCaloEnergyMeasCfg(flags)
    kwargs.setdefault("CaloMeasTool", result.popPrivateTools() )
    kwargs.setdefault("MinFinalEnergy", 1.0*GeV )
    kwargs.setdefault("MinMuonPt", 10.0*GeV )
    kwargs.setdefault("MopParametrization", True )
    if flags.Muon.MuonTrigger:
        # both properties also previously false if DetFlags.haveRIO.Calo_on()
        kwargs.setdefault("EnergyLossMeasurement", False )
        kwargs.setdefault("TrackIsolation", False )
    else:
        kwargs.setdefault("EnergyLossMeasurement", True )
        kwargs.setdefault("TrackIsolation", True )

    tmpAcc = MuidCaloEnergyToolParamCfg(flags, name, **kwargs)
    result.setPrivateTools(tmpAcc.popPrivateTools())
    result.merge(tmpAcc)
    return result

def MuidCaloTrackStateOnSurfaceCfg(flags, name='MuidCaloTrackStateOnSurface', **kwargs ):
    result = ComponentAccumulator()    
    kwargs.setdefault("Propagator", result.popToolsAndMerge( AtlasRKPropagatorCfg(flags) ) )
    kwargs.setdefault("MinRemainingEnergy" , 0.2*GeV )
    kwargs.setdefault("ParamPtCut"         , 3.0*GeV )
    acc = MuidCaloEnergyToolCfg(flags)
    kwargs.setdefault("CaloEnergyDeposit"  , acc.popPrivateTools() )
    result.merge(acc)
    acc = MuidCaloEnergyToolParamCfg(flags)
    kwargs.setdefault("CaloEnergyParam"  , acc.popPrivateTools() )
    result.merge(acc)

    tool = CompFactory.Rec.MuidCaloTrackStateOnSurface(name,**kwargs)
    result.setPrivateTools(tool)
    return result


def MuidCaloTrackStateOnSurfaceParamCfg(flags, name='MuidCaloTrackStateOnSurfaceParam', **kwargs ):
    result=ComponentAccumulator()
    kwargs.setdefault("Propagator", result.popToolsAndMerge( AtlasRKPropagatorCfg(flags) ) )
    kwargs.setdefault("MinRemainingEnergy" , 0.2*GeV )
    kwargs.setdefault("ParamPtCut"         , 3.0*GeV )
    kwargs.setdefault("CaloEnergyDeposit"  , MuidCaloEnergyParam(flags) )
    kwargs.setdefault("CaloEnergyParam"  ,   MuidCaloEnergyParam(flags) )
    tool = CompFactory.Rec.MuidCaloTrackStateOnSurface(name,**kwargs)
    result.setPrivateTools(tool)
    return result


def MuidMaterialEffectsOnTrackProviderCfg(flags, name='MuidMaterialEffectsOnTrackProvider', **kwargs ):
    result = MuidCaloTrackStateOnSurfaceCfg(flags)
    kwargs.setdefault("TSOSTool",      result.popPrivateTools() )
    acc = MuidCaloTrackStateOnSurfaceParamCfg(flags)
    kwargs.setdefault("TSOSToolParam", acc.popPrivateTools() )
    result.merge(acc)
    kwargs.setdefault("Cosmics",flags.Beam.Type == 'cosmics')
    tool = CompFactory.Rec.MuidMaterialEffectsOnTrackProvider(name,**kwargs)
    result.setPrivateTools(tool)
    return result

def MuidMaterialEffectsOnTrackProviderParamCfg(flags, name='MuidMaterialEffectsOnTrackProviderParam', **kwargs ):
    result = MuidCaloTrackStateOnSurfaceParamCfg(flags)
    muidtsosparam = result.popPrivateTools()
    kwargs.setdefault("TSOSTool",      muidtsosparam )
    kwargs.setdefault("TSOSToolParam", muidtsosparam )
    kwargs.setdefault("Cosmics",flags.Beam.Type == 'cosmics')
    tool = CompFactory.Rec.MuidMaterialEffectsOnTrackProvider(name,**kwargs)
    result.setPrivateTools(tool)
    return result

def MuonTrackQueryCfg(flags, name="MuonTrackQuery", **kwargs ):
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg
    result = MdtDriftCircleOnTrackCreatorCfg(flags)
    kwargs.setdefault("MdtRotCreator",   result.popPrivateTools() )
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    result.merge( TrackingGeometryCondAlgCfg(flags) )
    kwargs.setdefault("TrackingGeometryReadKey", "AtlasTrackingGeometry")

    acc = iPatFitterCfg(flags)
    kwargs.setdefault("Fitter", acc.popPrivateTools() )
    result.merge(acc)

    tool = CompFactory.Rec.MuonTrackQuery(name,**kwargs)
    result.setPrivateTools(tool)
    return result

def MuidSegmentRegionRecoveryToolCfg(flags, name ='MuidSegmentRegionRecoveryTool', **kwargs ):
    result = CombinedMuonTrackBuilderFitCfg(flags)
    kwargs.setdefault("Builder",  result.popPrivateTools() )
    tool = CompFactory.Muon.MuonSegmentRegionRecoveryTool(name,**kwargs)
    result.setPrivateTools(tool)
    return result


def MuidErrorOptimisationToolCfg(flags, name='MuidErrorOptimisationToolFit', **kwargs ):
    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryHelperToolCfg
    result = MuonTrackSummaryHelperToolCfg(flags)
    kwargs.setdefault("TrackSummaryTool",  result.popPrivateTools() )
    tool = CompFactory.Muon.MuonErrorOptimisationTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result

def MuonAlignmentUncertToolThetaCfg(flags,name ="MuonAlignmentUncertToolTheta", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("HistoName", "ThetaScattering")
    kwargs.setdefault("InFile", "MuonCombinedBaseTools/AlignmentUncertainties/201029_initial/ID_MS_Uncertainties.root")
    tool = CompFactory.Muon.MuonAlignmentUncertTool(name,**kwargs)
    result.addPublicTool(tool)
    return result

def MuonAlignmentUncertToolPhiCfg(flags,name ="MuonAlignmentUncertToolPhi", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("HistoName", "PhiScattering")
    kwargs.setdefault("InFile", "MuonCombinedBaseTools/AlignmentUncertainties/201029_initial/ID_MS_Uncertainties.root")
    tool =  CompFactory.Muon.MuonAlignmentUncertTool(name,**kwargs)
    result.addPublicTool(tool)
    return result

def CombinedMuonTrackBuilderCfg(flags, name='CombinedMuonTrackBuilder', **kwargs ):
    from AthenaCommon.SystemOfUnits import meter
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import CscClusterOnTrackCreatorCfg,MdtDriftCircleOnTrackCreatorCfg
    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
    result = ComponentAccumulator()
    acc = MuidCaloEnergyToolParamCfg(flags)
    kwargs.setdefault("CaloEnergyParam"               , acc.popPrivateTools() )
    result.merge(acc)
    acc = MuidCaloTrackStateOnSurfaceCfg(flags)
    kwargs.setdefault("CaloTSOS"                      , acc.popPrivateTools() )
    result.merge(acc)
    acc = MuidTrackCleanerCfg(flags)
    kwargs.setdefault("Cleaner"                      , acc.popPrivateTools() )
    result.merge(acc)
    acc = MuonAlignmentUncertToolPhiCfg(flags)
    result.merge(acc)
    kwargs.setdefault("AlignmentUncertToolPhi", result.getPublicTool('MuonAlignmentUncertToolPhi') )      
    acc = MuonAlignmentUncertToolThetaCfg(flags)
    result.merge(acc)
    kwargs.setdefault("AlignmentUncertToolTheta", result.getPublicTool('MuonAlignmentUncertToolTheta') )
      
    if flags.Detector.GeometryCSC and not flags.Muon.MuonTrigger:
        acc = CscClusterOnTrackCreatorCfg(flags)
        kwargs.setdefault("CscRotCreator"                 , acc.popPrivateTools() )
        result.merge(acc)
    else:
        kwargs.setdefault("CscRotCreator"                 , "")

    kwargs.setdefault("Extrapolator", result.popToolsAndMerge(AtlasExtrapolatorCfg(flags)) )

    acc = iPatFitterCfg(flags)
    ipatFitter = acc.popPrivateTools() # possibly used again below
    kwargs.setdefault("Fitter"      , ipatFitter )
    result.merge(acc)

    acc= iPatSLFitterCfg(flags)
    kwargs.setdefault("SLFitter"    , acc.popPrivateTools() )
    result.merge(acc)

    acc = MuidMaterialAllocatorCfg(flags)
    kwargs.setdefault("MaterialAllocator", acc.popPrivateTools() )
    result.merge(acc)

    acc = MdtDriftCircleOnTrackCreatorCfg(flags)
    kwargs.setdefault("MdtRotCreator"                 , acc.popPrivateTools() )
    result.merge(acc)

    ### Tracking Geometry
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
        TrackingGeometryCondAlgCfg)
    acc = TrackingGeometryCondAlgCfg(flags)
    result.merge(TrackingGeometryCondAlgCfg(flags))
    geom_cond_key = acc.getPrimary().TrackingGeometryWriteKey
    kwargs.setdefault("TrackingGeometryReadKey", geom_cond_key)

    kwargs.setdefault("CleanCombined"                 , True )
    kwargs.setdefault("CleanStandalone"               , True )
    kwargs.setdefault("BadFitChi2"                    , 2.5 )
    kwargs.setdefault("LargeMomentumError"            , 0.5 )
    kwargs.setdefault("LineMomentum"                  , flags.Muon.straightLineFitMomentum )
    kwargs.setdefault("LowMomentum"                   , 10.*GeV )
    kwargs.setdefault("MinEnergy"                     , 0.3*GeV )
    kwargs.setdefault("PerigeeAtSpectrometerEntrance" , False )
    kwargs.setdefault("ReallocateMaterial"            , False )
    kwargs.setdefault("Vertex2DSigmaRPhi"             , 100.*mm )
    kwargs.setdefault("Vertex3DSigmaRPhi"             , 6.*mm )
    kwargs.setdefault("Vertex3DSigmaZ"                , 60.*mm)
    kwargs.setdefault("UseCaloTG"                     , True )

    acc = MuonMaterialProviderToolCfg(flags)
    kwargs.setdefault( "CaloMaterialProvider", acc.getPrimary() )
    result.merge(acc)
 
    kwargs.setdefault("TrackQuery",   result.popToolsAndMerge(MuonTrackQueryCfg(flags)) )

    if flags.Muon.SAMuonTrigger:
        acc = MuonTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", acc.popPrivateTools())
        result.merge(acc)
    else:
        acc = MuonCombinedTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool"              , acc.popPrivateTools() )
        result.merge(acc)

    acc = MuonCombinedPropagatorCfg(flags)
    propagator = acc.popPrivateTools()
    kwargs.setdefault("Propagator"                    , propagator )
    kwargs.setdefault("SLPropagator"                  , propagator )
    result.merge(acc)

    if flags.Beam.Type == 'cosmics':
        kwargs.setdefault("MdtRotCreator" ,  "" )
        kwargs.setdefault("LowMomentum"   ,  1.5*GeV )
        kwargs.setdefault("ReallocateMaterial", False )
        kwargs.setdefault("Vertex2DSigmaRPhi" , 100.*mm )
        kwargs.setdefault("Vertex3DSigmaRPhi" , 100.*mm )
        kwargs.setdefault("Vertex3DSigmaZ"    ,  1.*meter )

    # configure tools for data reprocessing 
    if flags.Muon.enableErrorTuning and 'MuonErrorOptimizer' not in kwargs:
        # use alignment effects on track for all algorithms
        useAlignErrs = True
        if flags.IOVDb.DatabaseInstance == 'COMP200' or \
                'HLT'  in flags.IOVDb.GlobalTag or flags.Common.isOnline or flags.Muon.MuonTrigger:
            useAlignErrs = False

        from MuonConfig.MuonRecToolsConfig import MuonRefitToolCfg
        acc = MuonRefitToolCfg(flags, name="MuidRefitTool", AlignmentErrors = useAlignErrs, Fitter = ipatFitter)
        # refitTool = getPublicToolClone("MuidRefitTool", "MuonRefitTool", AlignmentErrors = useAlignErrs, Fitter = getPublicTool("iPatFitter"),
                                                                                                #  CscRotCreator=("Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator" if MuonGeometryFlags.hasCSC() else "")
        refitTool = acc.popPrivateTools()
        result.merge(acc)
        acc = MuidErrorOptimisationToolCfg(flags, name="MuidErrorOptimisationTool", PrepareForFit = False, 
                                                RecreateStartingParameters = False, RefitTool = refitTool)
        kwargs.setdefault("MuonErrorOptimizer", acc.popPrivateTools())
        result.merge(acc)
    else:
        kwargs.setdefault("MuonErrorOptimizer", "")


    if flags.Muon.MuonTrigger:
        kwargs.setdefault("MuonHoleRecovery"                 , "")
    else:
        # Should really be MuidSegmentRegionRecoveryToolCfg, but this includes CombinedMuonTrackBuilderCfg and so we have a circular dependency ATLASRECTS-6745
        from MuonConfig.MuonTrackBuildingConfig import MuonChamberHoleRecoveryToolCfg
        acc = MuonChamberHoleRecoveryToolCfg(flags)
        kwargs.setdefault("MuonHoleRecovery"                 , acc.popPrivateTools())
        result.merge(acc)

    if flags.Muon.doSegmentT0Fit:
        kwargs.setdefault("MdtRotCreator"                 , "" )

    tool = CompFactory.Rec.CombinedMuonTrackBuilder(name,**kwargs)
    result.setPrivateTools(tool)
    return result
   
def CombinedMuonTrackBuilderFitCfg(flags, name='CombinedMuonTrackBuilderFit', **kwargs ):
    # In the old configuration we had duplication between CombinedMuonTrackBuilder and CombinedMuonTrackBuilderFit
    # Here we just call the Combined
    kwargs.setdefault("PerigeeAtSpectrometerEntrance" , True )
    kwargs.setdefault("UseCaloTG"                     , False )
    result = MuidErrorOptimisationToolCfg(flags) #Pass in default
    kwargs.setdefault("MuonErrorOptimizer", result.popPrivateTools())
    acc = CombinedMuonTrackBuilderCfg(flags, name, **kwargs)
    tool = acc.popPrivateTools() #Need to reset this to be the primary tool
    result.merge(acc)
    result.setPrivateTools(tool)
    return result

def MuonMatchQualityCfg(flags, name='MuonMatchQuality', **kwargs ):
    result = CombinedMuonTagTestToolCfg(flags)
    kwargs.setdefault("TagTool", result.popPrivateTools() )
    kwargs.setdefault("TrackQuery", result.popToolsAndMerge(MuonTrackQueryCfg(flags)) )

    tool = CompFactory.Rec.MuonMatchQuality(name,**kwargs)
    result.setPrivateTools(tool)
    return result

def MuidMuonRecoveryCfg(flags, name='MuidMuonRecovery',**kwargs):
    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", result.popPrivateTools() )
    acc = CombinedMuonTrackBuilderCfg(flags)
    kwargs.setdefault("TrackBuilder", acc.popPrivateTools() )
    result.merge(acc)
    tool = CompFactory.Rec.MuidMuonRecovery(name,**kwargs)
    result.setPrivateTools(tool)
    return result

def MuonCombinedTrackFitterCfg(flags, name="MuonCombinedTrackFitter", **kwargs ):
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MuonRotCreatorCfg

    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("ExtrapolationTool", result.popPrivateTools() )
    kwargs.setdefault("NavigatorTool"         , result.popToolsAndMerge(AtlasNavigatorCfg(flags)) )
    kwargs.setdefault("PropagatorTool"        , result.popToolsAndMerge(MuonCombinedPropagatorCfg(flags)) )
    kwargs.setdefault("RotCreatorTool"        , result.popToolsAndMerge(MuonRotCreatorCfg(flags)) )
    kwargs.setdefault("EnergyLossTool"        , result.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags)) )

    kwargs.setdefault("MeasurementUpdateTool" , CompFactory.Trk.KalmanUpdator(name="MuonMeasUpdator") ) 
    #FIXME? Shouldn't this be configured? Was MuonMeasUpdator

    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    result.merge( TrackingGeometryCondAlgCfg(flags) )
    kwargs.setdefault("TrackingGeometryReadKey", "AtlasTrackingGeometry")

    kwargs.setdefault("ExtrapolatorMaterial"  , True )
    acc = MuidMaterialEffectsOnTrackProviderCfg(flags)
    kwargs.setdefault("MuidTool"              , acc.getPrimary() )
    result.merge(acc)
    kwargs.setdefault("MuidToolParam"         , None )
    if flags.Beam.Type =='collisions':
        acc = MuidMaterialEffectsOnTrackProviderParamCfg(flags)
        kwargs.setdefault("MuidToolParam"     , acc.getPrimary() )
        result.merge(acc)
    kwargs.setdefault("MuidMat"               , True )
    kwargs.setdefault("StraightLine"          , flags.Beam.Type == "cosmics" ) 
    # ^ Was: not jobproperties.BField.solenoidOn() and not jobproperties.BField.allToroidOn()
    kwargs.setdefault("MaxIterations"         , 50 )
    kwargs.setdefault("GetMaterialFromTrack"  , flags.Beam.Type != "cosmics" )
    # ^ Was: jobproperties.BField.solenoidOn() and jobproperties.BField.allToroidOn()
    kwargs.setdefault("RecalculateDerivatives", False)
    kwargs.setdefault("UseCaloTG"             , True) #
    tool = CompFactory.Trk.GlobalChi2Fitter(name,**kwargs)
    result.setPrivateTools(tool)
    return result

def CombinedMuonTagTestToolCfg(flags, name='CombinedMuonTagTestTool', **kwargs ):
    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("ExtrapolatorTool",result.popPrivateTools() )
    kwargs.setdefault("Chi2Cut",50000.)
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
    result.merge( TrackingGeometryCondAlgCfg(flags) )
    kwargs.setdefault("TrackingGeometryReadKey", "AtlasTrackingGeometry")
    tool = CompFactory.MuonCombined.MuonTrackTagTestTool(name,**kwargs)
    result.setPrivateTools(tool)
    return result

# From MuonCaloTagTool.py

def CaloTrkMuIdAlgTrackSelectorToolCfg(flags, name='CaloTrkMuIdAlgTrackSelectorTool', **kwargs ):
    kwargs.setdefault("pTMin", 5000.)
    kwargs.setdefault("IPd0Max", 7.)
    kwargs.setdefault("IPz0Max", 130)     # 130 (tuned on Z)
    kwargs.setdefault("nHitBLayer", 0)
    kwargs.setdefault("nHitPix", 1)
    kwargs.setdefault("nHitSct", 5)
    kwargs.setdefault("nHitSi", 7)
    kwargs.setdefault("nHitTrt", 0)
    result = MuonCombinedTrackSummaryToolCfg(flags)
    kwargs.setdefault("TrackSummaryTool", result.popPrivateTools() )
    acc = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", acc.popPrivateTools() )
    result.merge(acc)
    tool = CompFactory.InDet.InDetDetailedTrackSelectorTool(name, **kwargs )
    result.setPrivateTools(tool)
    return result


def TrackDepositInCaloToolCfg(flags, name ='TrackDepositInCaloTool', **kwargs ):
    from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg, ParticleCaloCellAssociationToolCfg
    result = ParticleCaloExtensionToolCfg(flags)
    kwargs.setdefault("ParticleCaloExtensionTool",       result.popPrivateTools() )
    acc = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("ExtrapolatorHandle", acc.popPrivateTools() )
    result.merge(acc)
    acc = ParticleCaloCellAssociationToolCfg(flags)
    kwargs.setdefault("ParticleCaloCellAssociationTool",       acc.popPrivateTools() )
    result.merge(acc)
    tool = CompFactory.TrackDepositInCaloTool(name, **kwargs )
    result.setPrivateTools(tool)
    return result

def CaloMuonLikelihoodToolCfg(flags, name='CaloMuonLikelihoodTool', **kwargs ):
    from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg
    result = ParticleCaloExtensionToolCfg(flags)
    kwargs.setdefault("ParticleCaloExtensionTool",       result.popPrivateTools() )
    tool = CompFactory.CaloMuonLikelihoodTool(name, **kwargs )
    result.setPrivateTools(tool)
    return result

def CaloMuonScoreToolCfg(flags, name='CaloMuonScoreTool', **kwargs ):
    from TrackToCalo.TrackToCaloConfig import ParticleCaloCellAssociationToolCfg
    result = ParticleCaloCellAssociationToolCfg(flags)
    kwargs.setdefault("ParticleCaloCellAssociationTool", result.popPrivateTools())
    tool = CompFactory.CaloMuonScoreTool(name, **kwargs )
    result.setPrivateTools(tool)
    return result

def MuonCaloTagToolCfg(flags, name='MuonCaloTagTool', **kwargs ):  
    result = ComponentAccumulator()
    kwargs.setdefault("CaloMuonTagLoose",       CompFactory.CaloMuonTag(name = "CaloMuonTagLoose", TagMode="Loose") )
    kwargs.setdefault("CaloMuonTagTight",       CompFactory.CaloMuonTag() )
    acc = CaloMuonLikelihoodToolCfg(flags)
    kwargs.setdefault("CaloMuonLikelihoodTool", acc.popPrivateTools() )
    result.merge(acc)
    acc = CaloMuonScoreToolCfg(flags)
    kwargs.setdefault("CaloMuonScoreTool", acc.popPrivateTools() )
    result.merge(acc)
    acc = TrackDepositInCaloToolCfg(flags)
    trackDepositInCaloTool = acc.popPrivateTools()
    kwargs.setdefault("TrackDepositInCaloTool", trackDepositInCaloTool  )
    result.merge(acc)
    acc = CaloTrkMuIdAlgTrackSelectorToolCfg(flags)
    calotrkmuidalgtrackselectortool = acc.popPrivateTools()
    kwargs.setdefault("TrackSelectorTool",   calotrkmuidalgtrackselectortool     )
    result.merge(acc)
    kwargs.setdefault("doCaloLR", True)
    tool = CompFactory.MuonCombined.MuonCaloTagTool(name, **kwargs )
    result.setPrivateTools(tool)
    return result

# Misc 
def MuonLayerSegmentFinderToolCfg(flags, name="MuonLayerSegmentFinderTool", **kwargs ):
    from MuonConfig.MuonSegmentFindingConfig import DCMathSegmentMakerCfg, MuonClusterSegmentFinderToolCfg, MuonClusterSegmentFinderCfg, MuonPRDSelectionToolCfg
    result = ComponentAccumulator() 

    csc2d=""
    csc4d=""
    if flags.Detector.GeometryCSC:
        from MuonConfig.MuonSegmentFindingConfig import Csc2dSegmentMakerCfg, Csc4dSegmentMakerCfg
        acc = Csc2dSegmentMakerCfg(flags)
        csc2d = acc.popPrivateTools()
        result.merge(acc)
        acc = Csc4dSegmentMakerCfg(flags)
        csc4d = acc.popPrivateTools()
        result.merge(acc)
    kwargs.setdefault("Csc2DSegmentMaker",               csc2d )
    kwargs.setdefault("Csc4DSegmentMaker",               csc4d )

    kwargs.setdefault("MuonPRDSelectionTool", result.popToolsAndMerge( MuonPRDSelectionToolCfg(flags) ) )
    if flags.Beam.Type != 'collisions':
        kwargs.setdefault("Key_MuonLayerHoughToolHoughDataPerSectorVec", "")


    acc = DCMathSegmentMakerCfg(flags, name = "DCMathSegmentMaker")
    segmentmaker = acc.popPrivateTools()
    kwargs.setdefault("SegmentMaker",               segmentmaker )
    result.merge(acc)

    acc = MuonClusterSegmentFinderCfg(flags, name = "MuonClusterSegmentFinder")
    clustersegmentfinder = acc.getPrimary() #Already adds it as a public tool.
    kwargs.setdefault("MuonClusterSegmentFinder",               clustersegmentfinder )
    result.merge(acc)
    
    acc = MuonClusterSegmentFinderToolCfg(flags, name = "MuonClusterSegmentFinderTool")
    clustersegmentfindertool = acc.popPrivateTools() 
    kwargs.setdefault("NSWMuonClusterSegmentFinderTool",               clustersegmentfindertool )
    result.merge(acc)

    tool = CompFactory.Muon.MuonLayerSegmentFinderTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result

def MuonLayerSegmentMatchingToolCfg(flags, name="MuonLayerSegmentMatchingTool", **kwargs):
    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", result.popPrivateTools())
    from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuTagMatchingToolCfg
    
    acc = MuTagMatchingToolCfg(flags)
    MuTagTool = acc.getPrimary()
    kwargs.setdefault("MatchTool", MuTagTool) 
    result.merge(acc)

    tool = CompFactory.Muon.MuonLayerSegmentMatchingTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result

def MuonInsideOutRecoToolCfg(flags, name="MuonInsideOutRecoTool", **kwargs ):
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("VertexContainer", "")
    result = MuonLayerSegmentFinderToolCfg(flags, name= "MuonLayerSegmentFinderTool")
    layersegmentfindertool = result.popPrivateTools()
    kwargs.setdefault("MuonLayerSegmentFinderTool", layersegmentfindertool)
    
    acc = MuonLayerSegmentMatchingToolCfg(flags)
    muon_layer_segment_matching = acc.popPrivateTools()
    kwargs.setdefault("MuonLayerSegmentMatchingTool", muon_layer_segment_matching)
    result.merge(acc)

    acc = MuonLayerAmbiguitySolverToolCfg(flags)
    muon_layer_ambiguity_solver = acc.popPrivateTools()
    kwargs.setdefault("MuonLayerAmbiguitySolverTool", muon_layer_ambiguity_solver)
    result.merge(acc)

    acc = MuonCandidateTrackBuilderToolCfg(flags)
    muon_candidate_track_builder = acc.popPrivateTools()
    kwargs.setdefault("MuonCandidateTrackBuilderTool", muon_candidate_track_builder)
    result.merge(acc)

    acc = MuonCombinedTrackSummaryToolCfg(flags)
    kwargs.setdefault("TrackSummaryTool", acc.getPrimary())
    result.merge(acc)

    acc = CombinedMuonTrackBuilderCfg(flags)
    kwargs.setdefault("MuonTrackBuilder", acc.getPrimary())
    result.merge(acc)

    from MuonConfig.MuonRecToolsConfig import MuonAmbiProcessorCfg
    acc = MuonAmbiProcessorCfg(flags)
    kwargs.setdefault("TrackAmbiguityProcessor", acc.getPrimary())
    result.merge(acc)

    kwargs.setdefault("TrackSummaryTool", acc.popToolsAndMerge(  MuonCombinedTrackSummaryToolCfg(flags) ) )


    tool = CompFactory.MuonCombined.MuonInsideOutRecoTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result

def MuonCandidateTrackBuilderToolCfg(flags, name="MuonCandidateTrackBuilderTool", **kwargs):
    from MuonConfig.MuonTrackBuildingConfig import MooTrackBuilderCfg
    result = MooTrackBuilderCfg(flags, name="MooMuonTrackBuilder")
    kwargs.setdefault("MuonSegmentTrackBuilder", result.getPrimary())

    acc = CombinedMuonTrackBuilderCfg(flags)
    combined_muon_track_builder = acc.getPrimary()
    kwargs.setdefault("MuonTrackBuilder", combined_muon_track_builder)
    result.merge(acc)

    muoncandidatetrackbuilder=CompFactory.Muon.MuonCandidateTrackBuilderTool(name, **kwargs)

    result.setPrivateTools(muoncandidatetrackbuilder)

    return result

def MuonSegmentSelectionToolCfg(flags, name="MuonSegmentSelectionTool", **kwargs):
    # Won't explicitly configure MuonIdHelperTool, MuonEDMPrinterTool, MuonSegmentHitSummaryTool
    if flags.Input.isMC is False:
      kwargs.setdefault("GoodADCFractionCut",  0.5 )
      kwargs.setdefault("MinADCPerSegmentCut", 100 )
    result = ComponentAccumulator()
    # kwargs.setdefault("Printer", MuonEDMPrinterTool(flags) ) # FIXME - needs property added.

    result.setPrivateTools(CompFactory.Muon.MuonSegmentSelectionTool(name, **kwargs))
    return result

def MuonLayerAmbiguitySolverToolCfg(flags, name="MuonLayerAmbiguitySolverTool", **kwargs):
    result = MuonSegmentSelectionToolCfg(flags)
    segment_selection_tool = result.popPrivateTools()
    kwargs.setdefault("MuonSegmentSelectionTool",segment_selection_tool)

    from MuonConfig.MuonTrackBuildingConfig import MuonSegmentMatchingToolCfg
    acc = MuonSegmentMatchingToolCfg(flags)
    muon_segment_matching = acc.popPrivateTools()
    kwargs.setdefault("MuonSegmentMatchingTool", muon_segment_matching)
    result.merge(acc)

    from MuonConfig.MuonTrackBuildingConfig import MooTrackBuilderCfg
    acc = MooTrackBuilderCfg(flags)
    muon_segment_track_builder = acc.popPrivateTools()
    kwargs.setdefault("MuonSegmentTrackBuilder", muon_segment_track_builder)
    result.merge(acc)

    kwargs.setdefault("MuonEDMPrinterTool", MuonEDMPrinterTool(flags) ) 

    result.setPrivateTools(CompFactory.Muon.MuonLayerAmbiguitySolverTool(name, **kwargs))
    
    return result

def MdtDriftCircleOnTrackCreatorStauCfg(flags, name="MdtDriftCircleOnTrackCreatorStau",**kwargs ):
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg, MdtCalibWindowNumber
    kwargs.setdefault("TimingMode", 3 )
    kwargs.setdefault("TimeWindowSetting", MdtCalibWindowNumber('Collision_t0fit') )
    return MdtDriftCircleOnTrackCreatorCfg(name,**kwargs)

def MuonStauRecoToolCfg(flags,  name="MuonStauRecoTool", **kwargs ):
    # In the old configuration this was split over several functions. But since these Stau tools are only used here, 
    # trying a new approach. We can always refactorise later if necessary.

    from MuonConfig.MuonSegmentFindingConfig import DCMathSegmentMakerCfg
    from MuonConfig.MuonTrackBuildingConfig import MuonChamberHoleRecoveryToolCfg
    from MuonConfig.MuonRecToolsConfig import MuonAmbiProcessorCfg, MuonSeededSegmentFinderCfg
    from MuonConfig.MuonCalibrationConfig import MdtCalibrationDbToolCfg

    kwargs.setdefault("ConsideredPDGs", [13,-13,1000015,-1000015])
    kwargs.setdefault("DoTruth", flags.Input.isMC )
    kwargs.setdefault("DoSummary", flags.Muon.printSummary )
    result = MdtDriftCircleOnTrackCreatorStauCfg(flags)
    rotcreator = result.popPrivateTools()

    acc = DCMathSegmentMakerCfg(flags, name="DCMathStauSegmentMaker", MdtCreator=rotcreator)
    segmentmaker = acc.popPrivateTools()
    kwargs.setdefault("MuonSegmentMaker", segmentmaker )
    result.merge(acc)

    # Now setup MuonInsideOutRecoTool property of MuonStauRecoTool. Long chain here! Could split for clarity. Another option would be to have a Stau flag on 
    # shared tool functions.
    acc = MuonChamberHoleRecoveryToolCfg(flags)
    chamberholerecoverytool = acc.popPrivateTools()
    result.merge(acc)

    acc = MuonSeededSegmentFinderCfg(flags, name = "MuonStauSeededSegmentFinder", MdtRotCreator=rotcreator, 
                                    SegmentMaker=segmentmaker, SegmentMakerNoHoles=segmentmaker)
    seededsegmentfinder = acc.popPrivateTools()
    result.merge(acc)

    acc = CombinedMuonTrackBuilderFitCfg(flags, name = "CombinedStauTrackBuilderFit", MdtRotCreator = rotcreator )
    fitter = acc.popPrivateTools()
    result.merge(acc)

    acc = MuidSegmentRegionRecoveryToolCfg(flags, name="MuonStauSegmentRegionRecoveryTool", SeededSegmentFinder =seededsegmentfinder, 
                                        ChamberHoleRecoveryTool=chamberholerecoverytool, Fitter= fitter )
    muidsegmentregionrecovery = acc.popPrivateTools()
    result.merge(acc)

    acc = CombinedMuonTrackBuilderCfg(flags, name="CombinedStauTrackBuilder", MdtRotCreator=rotcreator, MuonHoleRecovery = muidsegmentregionrecovery)
    trackbuilder = acc.popPrivateTools()

    muoncandidatetrackbuilder=CompFactory.Muon.MuonCandidateTrackBuilderTool(name="MuonStauCandidateTrackBuilderTool", MuonTrackBuilder=trackbuilder)

    acc = MuonInsideOutRecoToolCfg(flags, MuonCandidateTrackBuilderTool=muoncandidatetrackbuilder)
    kwargs.setdefault("MuonInsideOutRecoTool", acc.popPrivateTools() )
    result.merge(acc)
    acc = MuonAmbiProcessorCfg(flags)
    kwargs.setdefault("TrackAmbiguityProcessor", acc.popPrivateTools() )
    result.merge(acc)
    tool = CompFactory.MuonCombined.MuonStauRecoTool(name, **kwargs)
    result.setPrivateTools(tool)

    kwargs.setdefault("CalibrationDbTool", result.popToolsAndMerge( MdtCalibrationDbToolCfg(flags)))

    return result

def MuonSystemExtensionToolCfg(flags, **kwargs):
    result = ComponentAccumulator()
    
    from TrackToCalo.TrackToCaloConfig import  ParticleCaloExtensionToolCfg
    particle_calo_extension_tool = result.getPrimaryAndMerge(ParticleCaloExtensionToolCfg(flags))

    atlas_extrapolator = result.getPrimaryAndMerge(AtlasExtrapolatorCfg(flags))

    muon_ext_tool = CompFactory.Muon.MuonSystemExtensionTool("MuonSystemExtensionTool", 
                                                             ParticleCaloExtensionTool = particle_calo_extension_tool, 
                                                             Extrapolator = atlas_extrapolator)
    result.setPrivateTools(muon_ext_tool)
    return result
