# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Defines the shared tools used in muon identification
# Based on :
# https://gitlab.cern.ch/atlas/athena/blob/release/22.0.8/Reconstruction/MuonIdentification/MuonCombinedRecExample/python/MuonCombinedTools.py
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
from MuonConfig.MuonRecToolsConfig import MuonEDMPrinterTool

#FIXME
GeV = 1000
mm = 1

def MuonCombinedTrackSummaryToolCfg(flags, name="", **kwargs):
    # Based on https://gitlab.cern.ch/atlas/athena/blob/release/22.0.8/Reconstruction/MuonIdentification/MuonCombinedRecExample/python/CombinedMuonTrackSummary.py
    # FIXME - check all of this once the ID configuration is available, because probably we can simplify this a lot
    result = AtlasExtrapolatorCfg(flags)
    extrapolator = result.getPrimary()
    result.addPublicTool(extrapolator)

    from InDetConfig.InDetRecToolConfig import InDetTrackHoleSearchToolCfg
    acc = InDetTrackHoleSearchToolCfg(flags,
                                      name            = "CombinedMuonIDHoleSearch",
                                      Extrapolator    = extrapolator,
                                      CountDeadModulesAfterLastHit = True,
                                      Cosmics         = (flags.Beam.Type=="cosmics"))
    indet_hole_search_tool = acc.getPrimary()
    result.addPublicTool(indet_hole_search_tool)
    result.merge(acc)
    #FIXME - need InDet to provide configuration for PixelConditionsSummaryTool
    # Also assuming we don't use DetailedPixelHoleSearch (since it seems to be off in standard workflows)
    from InDetConfig.InDetRecToolConfig import InDetTrackSummaryHelperToolCfg
    acc = InDetTrackSummaryHelperToolCfg(flags,
                                         name="CombinedMuonIDSummaryHelper", 
                                         AssoTool        = None, 
                                         PixelToTPIDTool = None,
                                         TestBLayerTool  = None,
                                         DoSharedHits    = False,
                                         HoleSearch      = indet_hole_search_tool)
    indet_track_summary_helper_tool = acc.getPrimary()
    result.addPublicTool(indet_track_summary_helper_tool)
    result.merge(acc)


    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryHelperToolCfg
    acc = MuonTrackSummaryHelperToolCfg(flags)
    muon_track_summary_helper_tool = acc.getPrimary()
    track_summary_tool = CompFactory.Trk.TrackSummaryTool(name="CombinedMuonTrackSummary",
                                                          doSharedHits             = False,
                                                          doHolesInDet             = True,
                                                          doHolesMuon              = False,
                                                          AddDetailedInDetSummary  = True,
                                                          AddDetailedMuonSummary   = True,
                                                          InDetSummaryHelperTool   = indet_track_summary_helper_tool,
                                                          TRT_ElectronPidTool      = None,
                                                          PixelToTPIDTool          = None,
                                                          MuonSummaryHelperTool    = muon_track_summary_helper_tool,
                                                          PixelExists              = True )
    result.merge(acc)
    result.addPublicTool(track_summary_tool)
    result.setPrivateTools(track_summary_tool)
    return result


def MuonCombinedInDetDetailedTrackSelectorToolCfg(flags, name="MuonCombinedInDetDetailedTrackSelectorTool",**kwargs):
    if flags.Beam.Type == 'collisions':
        kwargs.setdefault("pTMin", 2000 )
        kwargs.setdefault("IPd0Max", 50.0 )
        kwargs.setdefault("IPz0Max", 9999.0 )
        kwargs.setdefault("z0Max", 9999.0 )
        kwargs.setdefault("useTrackSummaryInfo", True )
        kwargs.setdefault("nHitBLayer", 0 )
        kwargs.setdefault("nHitPix", 1 )
        kwargs.setdefault("nHitBLayerPlusPix", 0 )
        kwargs.setdefault("nHitSct", 3 )
        kwargs.setdefault("nHitSi", 4 )
        kwargs.setdefault("nHitTrt", 0 )
        kwargs.setdefault("useTrackQualityInfo", False )
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

    # FIXME the configuration of TrackSummaryTool should probably be centralised.
    acc = MuonCombinedTrackSummaryToolCfg(flags)
    kwargs.setdefault("TrackSummaryTool", acc.popPrivateTools() )
    result.merge(acc)

    # Has two CondKeys
    # SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

    # FIXME - let's put this someplace central?
    result.merge(addFoldersSplitOnline(flags,"INDET","/Indet/Onl/Beampos","/Indet/Beampos", className='AthenaAttributeList'))
    result.addCondAlgo(CompFactory.BeamSpotCondAlg("BeamSpotCondAlg"))

    # SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
    # FIXME - handle this ^
    
    tool = CompFactory.InDet.InDetDetailedTrackSelectorTool(name,**kwargs)
    result.addPublicTool(tool)
    result.setPrivateTools(tool)
    return result


def MuonCombinedParticleCreatorCfg(flags, name="MuonCombinedParticleCreator",**kwargs):
    result = ComponentAccumulator()    
    if flags.Muon.MuonTrigger:
        from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
        acc = MuonTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", acc.popPrivateTools())
        result.merge(acc)
    else:
        acc = MuonCombinedTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", acc.getPrimary() ) 

    acc = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", acc.getPrimary() )
    result.addPublicTool(kwargs['Extrapolator'])
    result.merge(acc)
    kwargs.setdefault("KeepAllPerigee",True )
    kwargs.setdefault("UseMuonSummaryTool",True )
    if flags.Beam.Type=="cosmics":
        kwargs.setdefault("PerigeeExpression","Origin")
    tool = CompFactory.Trk.TrackParticleCreatorTool(name,**kwargs)
    result.addPublicTool(tool)
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
    return result


def MuonMaterialProviderToolCfg(flags,  name = "MuonMaterialProviderTool"):
    from TrackToCalo.TrackToCaloConfig import ParticleCaloCellAssociationToolCfg, ParticleCaloExtensionToolCfg

    result = ParticleCaloCellAssociationToolCfg(flags)
    particle_calo_cell_association_tool = result.getPrimary()
    result.addPublicTool( particle_calo_cell_association_tool )

    acc = ParticleCaloExtensionToolCfg(flags)
    particle_calo_extension_tool = acc.getPrimary()
    result.addPublicTool( particle_calo_cell_association_tool )
    result.merge(acc)

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

    # workaround as long as public tool is required
    acc = AtlasExtrapolatorCfg(flags)
    atlas_extrapolator = acc.popPrivateTools()
    result.merge(acc)
    result.addPublicTool(atlas_extrapolator)
    kwargs = dict()
    kwargs["Extrapolator"] = atlas_extrapolator
    if flags.Muon.SAMuonTrigger:
        from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
        acc = MuonTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", acc.popPrivateTools())
        result.merge(acc)
    else:
        acc = MuonCombinedTrackSummaryToolCfg(flags)
        muon_combined_track_summary_tool = acc.popPrivateTools()
        result.merge(acc)
        kwargs["TrackSummaryTool"] = muon_combined_track_summary_tool
    kwargs["KeepAllPerigee"] = True 
    kwargs["PerigeeExpression"] = "Origin"
    track_particle_creator = CompFactory.Trk.TrackParticleCreatorTool(name="MuonCaloParticleCreator",**kwargs)
    result.addPublicTool(track_particle_creator)
  
    muonCaloEnergyTool = CompFactory.Rec.MuonCaloEnergyTool(name="MuonCaloEnergy", ParticleCaloExtensionTool = particle_calo_extension_tool,
                                                 ParticleCaloCellAssociationTool = particle_calo_cell_association_tool,
                                                 TrackParticleCreator = track_particle_creator)

    useCaloEnergyMeas = True
    if flags.Muon.MuonTrigger:
        useCaloEnergyMeas = False

    tool = CompFactory.Trk.TrkMaterialProviderTool(name = name, MuonCaloEnergyTool = muonCaloEnergyTool, UseCaloEnergyMeasurement = useCaloEnergyMeas)
    result.addPublicTool(tool)
    result.setPrivateTools(tool)
    return result 


def MuonCreatorToolCfg(flags, name="MuonCreatorTool", **kwargs):
    from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg
    result = MuonMaterialProviderToolCfg(flags)
    kwargs.setdefault( "CaloMaterialProvider", result.getPrimary() )
    if flags.Muon.MuonTrigger:
        kwargs.setdefault('MakeTrackAtMSLink',True)
        kwargs.setdefault("FillTimingInformation",False)
        kwargs.setdefault("MuonSelectionTool", "")
        kwargs.setdefault("UseCaloCells", False)
        kwargs.setdefault("TrackSegmentAssociationTool", "")

    acc = MuonCombinedParticleCreatorCfg(flags)
    kwargs.setdefault("TrackParticleCreator", acc.getPrimary() )
    result.merge(acc)

    acc = ParticleCaloExtensionToolCfg(flags)
    kwargs.setdefault("ParticleCaloExtensionTool", acc.getPrimary() )
    result.merge(acc)

    from MuonConfig.MuonRecToolsConfig import MuonAmbiProcessorCfg, MuonTrackSummaryToolCfg
    acc = MuonAmbiProcessorCfg(flags)
    kwargs.setdefault("AmbiguityProcessor", acc.popPrivateTools())
    result.merge(acc)

    acc = MuonTrackSummaryToolCfg(flags)
    kwargs.setdefault("TrackSummaryTool", acc.popPrivateTools())
    result.merge(acc)

    # This tool needs MuonScatteringAngleSignificanceTool... which in turn needs TrackingVolumeSvc.
    # FIXME - probably this should be someplace central.
    trackingVolSvc = CompFactory.Trk.TrackingVolumesSvc(name="TrackingVolumesSvc")
    result.addService(trackingVolSvc)

    tool = CompFactory.MuonCombined.MuonCreatorTool(name,**kwargs)
    result.addPublicTool(tool)
    result.setPrivateTools(tool)
    return result 

def ExtrapolateMuonToIPToolCfg(flags, name="ExtrapolateMuonToIPTool", **kwargs):
    #FIXME complete this configuration
    if flags.Muon.MuonTrigger:
        from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
        result = MuonTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", result.popPrivateTools())
    else:
        result = MuonCombinedTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", result.popPrivateTools() )
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
        result.addPublicTool(extrapolator)
        kwargs.setdefault("TrackExtrapolationTool", extrapolator )
        result.merge(acc)
#   if cosmics was until here

    acc = MuonAmbiProcessorCfg(flags)
    ambiguityprocessor = acc.popPrivateTools()
    result.addPublicTool(ambiguityprocessor)

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

def MuonCombinedToolCfg(flags, name="MuonCombinedTool",**kwargs):
    tools = []
    result=ComponentAccumulator()
    if flags.MuonCombined.doCombinedFit:
        acc = MuonCombinedFitTagToolCfg(flags)
        tool = acc.popPrivateTools()
        tools.append( tool  )
        result.addPublicTool(tool)
        result.merge(acc)
    if flags.MuonCombined.doStatisticalCombination and flags.Beam.Type != 'cosmics':
        acc = MuonCombinedStacoTagToolCfg(flags)
        tool = acc.popPrivateTools()
        tools.append( tool  )
        result.addPublicTool(tool)
        result.merge(acc)

    kwargs.setdefault("MuonCombinedTagTools", tools )
    tool = CompFactory.MuonCombined.MuonCombinedTool(name,**kwargs)
    result.setPrivateTools(tool)
    return result 

def MuonCombinedFitTagToolCfg(flags, name="MuonCombinedFitTagTool",**kwargs):
    # if TriggerFlags.MuonSlice.doTrigMuonConfig:
    #     from TrkExRungeKuttaIntersector.TrkExRungeKuttaIntersectorConf import Trk.IntersectorWrapper as Propagator
    #     TrigMuonPropagator = Propagator(name = 'TrigMuonPropagator')
    #     ToolSvc += TrigMuonPropagator
    #     kwargs.setdefault("TrackBuilder",         getPublicToolClone("TrigMuonTrackBuilder", "CombinedMuonTrackBuilder", Propagator=TrigMuonPropagator) )
    #     kwargs.setdefault("VertexContainer", "")
    # else:
    result = CombinedMuonTrackBuilderCfg(flags)
    tool = result.popPrivateTools()
    result.addPublicTool(tool)
    kwargs.setdefault("TrackBuilder",  tool )

    acc = MuonTrackQueryCfg(flags)
    kwargs.setdefault("TrackQuery",           acc.popPrivateTools() )
    result.merge(acc)

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
    from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg
    result = ParticleCaloExtensionToolCfg(flags)
    kwargs.setdefault("ParticleCaloExtensionTool", result.getPrimary() )  
    kwargs.setdefault("Printer", MuonEDMPrinterTool(flags) )
    tool = CompFactory.MuonCombined.MuonCombinedStacoTagTool(name,**kwargs)
    result.setPrivateTools(tool)
    return result 

# From Combined fit tools.py

def MuidMaterialAllocatorCfg(flags, name='MuidMaterialAllocator', **kwargs): 
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    kwargs.setdefault("AggregateMaterial",True)
    kwargs.setdefault("AllowReordering",False)

    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", result.getPrimary() )
    acc  = TrackingGeometrySvcCfg(flags)
    kwargs.setdefault("TrackingGeometrySvc", acc.getPrimary() )
    result.merge(acc)
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
        from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryToolCfg
        acc = MuonTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", acc.popPrivateTools())
        result.merge(acc)
    else:
        acc = MuonCombinedTrackSummaryToolCfg(flags)
        kwargs.setdefault("TrackSummaryTool", acc.getPrimary() )
        result.merge(acc)

    tool = CompFactory.Trk.iPatFitter(name,**kwargs)
    result.setPrivateTools(tool)
    return result 

def iPatSLFitterCfg(flags, name='iPatSLFitter', **kwargs): 
    kwargs.setdefault("LineFit",True)
    kwargs.setdefault("LineMomentum", flags.Muon.straightLineFitMomentum )
    return iPatFitterCfg(flags, **kwargs)

# track cleaner configured to use the same fitter
def MuidTrackCleanerCfg(flags, name='MuidTrackCleaner', **kwargs ):
    if flags.Beam.Type == 'cosmics':
        kwargs.setdefault("PullCut"     , 5.0)
        kwargs.setdefault("PullCutPhi"  , 10.0)
    else:
        kwargs.setdefault("PullCut"     , 4.0)
        kwargs.setdefault("PullCutPhi"  , 4.0)

    if flags.Muon.MuonTrigger:
        kwargs.setdefault("Iterate", False)
        kwargs.setdefault("RecoverOutliers", False)
    result = iPatFitterCfg(flags)
    kwargs.setdefault("Fitter"      , result.popPrivateTools() )
    acc= iPatSLFitterCfg(flags)
    kwargs.setdefault("SLFitter"    , acc.popPrivateTools() )
    result.merge(acc)
  
    kwargs.setdefault("Printer", MuonEDMPrinterTool(flags) )

    tool = CompFactory.Muon.MuonTrackCleaner(name,**kwargs) #FIXME this is also in MuonConfig - check to see whether we can just use this.
    result.setPrivateTools(tool)
    return result
    
def MuidCaloEnergyParam(flags, name='MuidCaloEnergyParam', **kwargs ):
    kwargs.setdefault("Cosmics", flags.Beam.Type == 'cosmics' )
    return CompFactory.Rec.MuidCaloEnergyParam(name,**kwargs)

def MuidCaloEnergyMeasCfg(flags, name='MuidCaloEnergyMeas', **kwargs ):
    result = ComponentAccumulator()
    muidcaloenergyparam =  MuidCaloEnergyParam(flags)
    kwargs.setdefault("CaloParamTool",  muidcaloenergyparam)
    result.addPublicTool(muidcaloenergyparam)
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
    kwargs.setdefault("Propagator", CompFactory.Trk.RungeKuttaPropagator(name = 'AtlasRungeKuttaPropagator'))# FIXME - there should be a CA for this!
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
    kwargs.setdefault("Propagator", CompFactory.Trk.RungeKuttaPropagator(name = 'AtlasRungeKuttaPropagator'))# FIXME - there should be a CA for this!
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


def MuonCombinedPropagatorCfg(flags, name='MuonCombinedPropagator', **kwargs ):
    result = ComponentAccumulator()
    if not flags.Muon.MuonTrigger:
        kwargs.setdefault("AccuracyParameter",   .000001 )
        kwargs.setdefault("IncludeBgradients",   True )
        kwargs.setdefault("MaxHelixStep",        .001 )
        kwargs.setdefault("MaxStraightLineStep", .001 )
    tool = CompFactory.Trk.RungeKuttaPropagator(name,**kwargs)
    result.setPrivateTools(tool)
    return result

def MuonTrackQueryCfg(flags, name="MuonTrackQuery", **kwargs ):
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg
    result = MdtDriftCircleOnTrackCreatorCfg(flags)
    kwargs.setdefault("MdtRotCreator",   result.popPrivateTools() )

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
    result = MuonCombinedTrackSummaryToolCfg(flags)
    kwargs.setdefault("TrackSummaryTool",  result.popPrivateTools() )
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

    if flags.Detector.GeometryCSC and not flags.Muon.MuonTrigger:
        acc = CscClusterOnTrackCreatorCfg(flags)
        kwargs.setdefault("CscRotCreator"                 , acc.popPrivateTools() )
        result.merge(acc)
    else:
        kwargs.setdefault("CscRotCreator"                 , "")

    acc = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", acc.getPrimary() )
    result.merge(acc)

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
        # FIXME - handle this.
        #    if conddb.dbdata == 'COMP200' or conddb.dbmc == 'COMP200' or 'HLT' in globalflags.ConditionsTag() or conddb.isOnline or TriggerFlags.MuonSlice.doTrigMuonConfig:
        #         useAlignErrs = False
        from MuonConfig.MuonRecToolsConfig import MuonRefitToolCfg
        acc = MuonRefitToolCfg(flags, name="MuidRefitTool", AlignmentErrors = useAlignErrs, Fitter = ipatFitter)
        # refitTool = getPublicToolClone("MuidRefitTool", "MuonRefitTool", AlignmentErrors = useAlignErrs, Fitter = getPublicTool("iPatFitter"),
                                                                                                #  CscRotCreator=("Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator" if MuonGeometryFlags.hasCSC() else "")
        refitTool = acc.popPrivateTools()
        result.merge(acc)
        acc = MuidErrorOptimisationToolCfg(flags, name="MuidErrorOptimisationToolTuning", PrepareForFit = False, 
                                                RecreateStartingParameters = False, RefitTool = refitTool)
        kwargs.setdefault("MuonErrorOptimizer", acc.popPrivateTools())
        result.merge(acc)
    else:
        kwargs.setdefault("MuonErrorOptimizer", "")


    if flags.Muon.MuonTrigger:
        kwargs.setdefault("MuonHoleRecovery"                 , "")
    else:
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
    tool = CompFactory.Rec.MuonMatchQuality(name,**kwargs)
    result.setPrivateTools(tool)
    return result

def MuidMuonRecoveryCfg(flags, name='MuidMuonRecovery',**kwargs):
    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", result.getPrimary() )
    acc = CombinedMuonTrackBuilderCfg(flags)
    kwargs.setdefault("TrackBuilder", acc.popPrivateTools() )
    result.merge(acc)
    tool = CompFactory.Rec.MuidMuonRecovery(name,**kwargs)
    result.setPrivateTools(tool)
    return result

def MuonCombinedTrackFitterCfg(flags, name="MuonCombinedTrackFitter", **kwargs ):
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    from MuonConfig.MuonRecToolsConfig import MuonNavigatorCfg
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MuonRotCreatorCfg

    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("ExtrapolationTool", result.getPrimary() )
    
    acc = MuonNavigatorCfg(flags)
    kwargs.setdefault("NavigatorTool", acc.popPrivateTools())
    result.merge(acc)

    acc = MuonCombinedPropagatorCfg(flags)
    kwargs.setdefault("PropagatorTool"        , acc.popPrivateTools() )
    result.merge(acc)

    acc = MuonRotCreatorCfg(flags)
    kwargs.setdefault("RotCreatorTool"        , acc.popPrivateTools() )
    result.merge(acc)

    kwargs.setdefault("MeasurementUpdateTool" , CompFactory.Trk.KalmanUpdator() ) 
    #FIXME? Shouldn't this be configured? Was MuonMeasUpdator

    acc  = TrackingGeometrySvcCfg(flags)
    kwargs.setdefault("TrackingGeometrySvc", acc.getPrimary() )
    result.merge(acc)
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
    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    result = AtlasExtrapolatorCfg(flags)
    kwargs.setdefault("ExtrapolatorTool",result.getPrimary() )
    acc  = TrackingGeometrySvcCfg(flags)
    kwargs.setdefault("TrackingGeometrySvc", acc.getPrimary() )
    result.merge(acc)
    kwargs.setdefault("Chi2Cut",50000.)
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

def TrackEnergyInCaloToolCfg(flags, name ='TrackEnergyInCaloTool', **kwargs ):
    result = AtlasExtrapolatorCfg(flags)
    extrapolator = result.popPrivateTools()
    result.addPublicTool( extrapolator )
    kwargs.setdefault("ExtrapolatorHandle", extrapolator )
    tool = CompFactory.TrackEnergyInCaloTool(name, **kwargs )
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
    caloMuonScoreSvc = CompFactory.CaloMuonScoreONNXRuntimeSvc(name="CaloMuonScoreONNXRuntimeSvc")
    result.addService(caloMuonScoreSvc)
    tool = CompFactory.CaloMuonScoreTool(name, **kwargs )
    result.setPrivateTools(tool)
    return result

def MuonCaloTagToolCfg(flags, name='MuonCaloTagTool', **kwargs ):  
    result = ComponentAccumulator()
    kwargs.setdefault("CaloMuonTagLoose",       CompFactory.CaloMuonTag(name = "CaloMuonTagLoose", TagMode="Loose") )
    kwargs.setdefault("CaloMuonTagTight",       CompFactory.CaloMuonTag() )
    result.addPublicTool(kwargs['CaloMuonTagLoose'])
    result.addPublicTool(kwargs['CaloMuonTagTight'])

    acc = CaloMuonLikelihoodToolCfg(flags)
    kwargs.setdefault("CaloMuonLikelihoodTool", acc.popPrivateTools() )
    result.addPublicTool(kwargs['CaloMuonLikelihoodTool'])

    acc = CaloMuonScoreToolCfg(flags)
    kwargs.setdefault("CaloMuonScoreTool", acc.popPrivateTools() )
    result.addPublicTool(kwargs['CaloMuonScoreTool'])

    result.merge(acc)
    acc = TrackDepositInCaloToolCfg(flags)
    trackDepositInCaloTool = acc.popPrivateTools()
    result.addPublicTool(trackDepositInCaloTool)
    kwargs.setdefault("TrackDepositInCaloTool", trackDepositInCaloTool  )
    result.merge(acc)
    acc = CaloTrkMuIdAlgTrackSelectorToolCfg(flags)
    calotrkmuidalgtrackselectortool = acc.popPrivateTools()
    result.addPublicTool(calotrkmuidalgtrackselectortool)
    kwargs.setdefault("TrackSelectorTool",   calotrkmuidalgtrackselectortool     )
    result.merge(acc)
    kwargs.setdefault("doCaloLR", True)
    tool = CompFactory.MuonCombined.MuonCaloTagTool(name, **kwargs )
    result.setPrivateTools(tool)
    return result

# Misc 
def MuonLayerSegmentFinderToolCfg(flags, name="MuonLayerSegmentFinderTool", **kwargs ):
    from MuonConfig.MuonSegmentFindingConfig import DCMathSegmentMakerCfg, MuonClusterSegmentFinderToolCfg
    result = ComponentAccumulator() 

    csc2d=""
    csc4d=""
    if flags.Detector.GeometryCSC:
        from MuonConfig.MuonSegmentFindingConfig import Csc2dSegmentMakerCfg, MuonClusterSegmentFinderCfg, Csc4dSegmentMakerCfg
        acc = Csc2dSegmentMakerCfg(flags)
        csc2d = acc.popPrivateTools()
        result.addPublicTool(csc2d)
        result.merge(acc)
        acc = Csc4dSegmentMakerCfg(flags)
        csc4d = acc.popPrivateTools()
        result.addPublicTool(csc4d)
        result.merge(acc)
    kwargs.setdefault("Csc2DSegmentMaker",               csc2d )
    kwargs.setdefault("Csc4DSegmentMaker",               csc4d )

    acc = DCMathSegmentMakerCfg(flags, name = "DCMathSegmentMaker")
    segmentmaker = acc.popPrivateTools()
    kwargs.setdefault("SegmentMaker",               segmentmaker )
    result.addPublicTool(segmentmaker)
    result.merge(acc)

    acc = MuonClusterSegmentFinderCfg(flags, name = "MuonClusterSegmentFinderEJWM")
    clustersegmentfinder = acc.getPrimary() #Already adds it as a public tool.
    kwargs.setdefault("MuonClusterSegmentFinder",               clustersegmentfinder )
    result.merge(acc)
    
    acc = MuonClusterSegmentFinderToolCfg(flags, name = "MuonClusterSegmentFinderTool")
    clustersegmentfindertool = acc.popPrivateTools() 
    kwargs.setdefault("NSWMuonClusterSegmentFinderTool",               clustersegmentfindertool )
    result.addPublicTool(clustersegmentfindertool)
    result.merge(acc)

    tool = CompFactory.Muon.MuonLayerSegmentFinderTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result

def MuonInsideOutRecoToolCfg(flags, name="MuonInsideOutRecoTool", **kwargs ):
#    if TriggerFlags.MuonSlice.doTrigMuonConfig:
#       kwargs.setdefault("VertexContainer", "")
    result = MuonLayerSegmentFinderToolCfg(flags, name= "MuonLayerSegmentFinderToolEJWM")
    layersegmentfindertool = result.popPrivateTools()
    kwargs.setdefault("MuonLayerSegmentFinderTool", layersegmentfindertool)
    result.addPublicTool(layersegmentfindertool)

    acc = MuonLayerAmbiguitySolverToolCfg(flags)
    muon_layer_ambiguity_solver = acc.popPrivateTools()
    kwargs.setdefault("MuonLayerAmbiguitySolverTool", muon_layer_ambiguity_solver)
    acc.addPublicTool(muon_layer_ambiguity_solver)
    result.merge(acc)

    muon_layer_segment_matching = CompFactory.Muon.MuonLayerSegmentMatchingTool(name="MuonLayerSegmentMatchingTool")
    kwargs.setdefault("MuonLayerSegmentMatchingTool", muon_layer_segment_matching)
    result.addPublicTool(muon_layer_segment_matching)

    acc = MuonCandidateTrackBuilderToolCfg(flags)
    muon_candidate_track_builder = acc.popPrivateTools()
    kwargs.setdefault("MuonCandidateTrackBuilderTool", muon_candidate_track_builder)
    acc.addPublicTool(muon_candidate_track_builder)
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

    tool = CompFactory.MuonCombined.MuonInsideOutRecoTool(name, **kwargs)
    result.setPrivateTools(tool)
    return result

def MuonCandidateTrackBuilderToolCfg(flags, name="MuonCandidateTrackBuilderTool", **kwargs):
    from MuonConfig.MuonTrackBuildingConfig import MooTrackBuilderCfg
    result = MooTrackBuilderCfg(flags)
    kwargs.setdefault("MuonSegmentTrackBuilder", result.getPrimary())

    acc = CombinedMuonTrackBuilderCfg(flags)
    combined_muon_track_builder = acc.getPrimary()
    acc.addPublicTool(combined_muon_track_builder)
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
    result.addPublicTool(segment_selection_tool)

    from MuonConfig.MuonTrackBuildingConfig import MuonSegmentMatchingToolCfg
    acc = MuonSegmentMatchingToolCfg(flags)
    muon_segment_matching = acc.popPrivateTools()
    kwargs.setdefault("MuonSegmentMatchingTool", muon_segment_matching)
    acc.addPublicTool(muon_segment_matching)
    result.merge(acc)

    from MuonConfig.MuonTrackBuildingConfig import MooTrackBuilderCfg
    acc = MooTrackBuilderCfg(flags)
    muon_segment_track_builder = acc.popPrivateTools()
    kwargs.setdefault("MuonSegmentTrackBuilder", muon_segment_track_builder)
    acc.addPublicTool(muon_segment_track_builder)
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
    return result
