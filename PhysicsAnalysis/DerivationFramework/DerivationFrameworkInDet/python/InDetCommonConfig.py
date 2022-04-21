# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# InDetCommonConfig.py
# Configures all tools needed for ID track object selection 
# and inserts them into a and writes
# results into SG. These may then be accessed along the train
# Component accumulator version
#********************************************************************

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def InDetCommonCfg(ConfigFlags, **kwargs):
    """Main config for common ID track decorations"""

    acc = ComponentAccumulator()    
    CommonAugmentation = CompFactory.DerivationFramework.CommonAugmentation
 
    have_PV_container = ConfigFlags.Beam.Type is not BeamType.Cosmics and (any('PrimaryVertices' in elements for elements in ConfigFlags.Input.TypedCollections))
    if not have_PV_container and kwargs['DoVertexFinding']:
       have_PV_container = any('PixelRDOs' in elements for elements in ConfigFlags.Input.TypedCollections) or any('SCT_RDOs' in elements for elements in ConfigFlags.Input.TypedCollections)
    
    if have_PV_container:
        #====================================================================
        # LABELLING TRACKS WITH OUTCOME OF SELECTOR TOOL
        #====================================================================
        from DerivationFrameworkInDet.InDetToolsConfig import InDetTrackSelectionToolWrapperCfg
        DFCommonTrackSelection = acc.getPrimaryAndMerge(InDetTrackSelectionToolWrapperCfg(
            ConfigFlags,
            name           = "DFCommonTrackSelection",
            ContainerName  = "InDetTrackParticles",
            DecorationName = "DFCommonTightPrimary"))
        DFCommonTrackSelection.TrackSelectionTool.CutLevel = "TightPrimary" 

        if kwargs['AddPseudoTracks']:
            from DerivationFrameworkInDet.InDetToolsConfig import PseudoTrackSelectorCfg
            PseudoTrackSelectorTool = acc.getPrimaryAndMerge(PseudoTrackSelectorCfg(
                ConfigFlags,
                name                                 = "PseudoTrackSelectorTool",
                RecoTrackParticleLocation            = "InDetTrackParticles",
                PseudoTrackParticleLocation          = "InDetPseudoTrackParticles",
                OutputRecoReplacedWithPseudo         = "InDetReplacedWithPseudoTrackParticles",
                OutputRecoReplacedWithPseudoFromB    = "InDetReplacedWithPseudoFromBTrackParticles",
                OutputRecoReplacedWithPseudoNotFromB = "InDetReplacedWithPseudoNotFromBTrackParticles",
                OutputRecoPlusPseudo                 = "InDetPlusPseudoTrackParticles",
                OutputRecoPlusPseudoFromB            = "InDetPlusPseudoFromBTrackParticles",
                OutputRecoPlusPseudoNotFromB         = "InDetPlusPseudoNotFromBTrackParticles",
                OutputRecoNoFakes                    = "InDetNoFakesTrackParticles",
                OutputRecoNoFakesFromB               = "InDetNoFakesFromBTrackParticles",
                OutputRecoNoFakesNotFromB            = "InDetNoFakesNotFromBTrackParticles"))

            acc.addEventAlgo(CommonAugmentation("InDetSelectedPseudo", AugmentationTools = [PseudoTrackSelectorTool]))
           
        #====================================================================
        # EXPRESSION OF Z0 AT THE PRIMARY VERTEX
        #====================================================================
        from DerivationFrameworkInDet.InDetToolsConfig import TrackParametersAtPVCfg
        DFCommonZ0AtPV = acc.getPrimaryAndMerge(TrackParametersAtPVCfg(
            ConfigFlags,
            name                       = "DFCommonZ0AtPV",
            TrackParticleContainerName = "InDetTrackParticles",
            VertexContainerName        = "PrimaryVertices", 
            Z0SGEntryName              = "DFCommonInDetTrackZ0AtPV" ))
    
        #====================================================================
        # DECORATE THE HARDSCATTER VERTEX WITH A FLAG
        #====================================================================
        from InDetHardScatterSelectionTool.InDetHardScatterSelectionToolConfig import InDetHardScatterSelectionToolCfg
        DFCommonHSSelectionTool = acc.popToolsAndMerge(InDetHardScatterSelectionToolCfg(
            ConfigFlags, 
            name = "DFCommonHSSelectionTool"))
        acc.addPublicTool(DFCommonHSSelectionTool)    

        from DerivationFrameworkInDet.InDetToolsConfig import HardScatterVertexDecoratorCfg
        DFCommonHSDecorator = acc.getPrimaryAndMerge(HardScatterVertexDecoratorCfg(
            ConfigFlags,
            name                     = "DFCommonHSDecorator",
            VertexContainerName      = "PrimaryVertices",
            HardScatterDecoName      = "hardScatterVertexLink",
            HardScatterSelectionTool = DFCommonHSSelectionTool))
    
        #====================================================================
        # DECORATE THE TRACKS WITH USED-IN-FIT TTVA VARIABLES
        #====================================================================
        from InDetUsedInFitTrackDecoratorTool.InDetUsedInFitTrackDecoratorToolConfig import InDetUsedInFitTrackDecoratorToolCfg
        DFCommonUsedInFitDecoratorTool = acc.popToolsAndMerge(InDetUsedInFitTrackDecoratorToolCfg(
            ConfigFlags,
            name                 = "DFCommonUsedInFitDecoratorTool",
            AMVFVerticesDecoName = "TTVA_AMVFVertices",
            AMVFWeightsDecoName  = "TTVA_AMVFWeights",
            TrackContainer       = "InDetTrackParticles",
            VertexContainer      = "PrimaryVertices" ))
        acc.addPublicTool(DFCommonUsedInFitDecoratorTool)
    
        from DerivationFrameworkInDet.InDetToolsConfig import UsedInVertexFitTrackDecoratorCfg
        DFCommonUsedInFitDecorator = acc.getPrimaryAndMerge(UsedInVertexFitTrackDecoratorCfg(
            ConfigFlags,
            name                   = "DFCommonUsedInFitDecorator",
            UsedInFitDecoratorTool = DFCommonUsedInFitDecoratorTool ))

        if kwargs['AddPseudoTracks']:
            PseudoTrackContainers = [
                "InDetPseudoTrackParticles",
                "InDetReplacedWithPseudoTrackParticles",
                "InDetReplacedWithPseudoFromBTrackParticles",
                "InDetReplacedWithPseudoNotFromBTrackParticles",
                "InDetPlusPseudoTrackParticles",
                "InDetPlusPseudoFromBTrackParticles",
                "InDetPlusPseudoNotFromBTrackParticles",
                "InDetNoFakesTrackParticles",
                "InDetNoFakesFromBTrackParticles",
                "InDetNoFakesNotFromBTrackParticles"
            ]
            PseudoTrackDecorators = []
            for t in PseudoTrackContainers:
                InDetDecorator = acc.getPrimaryAndMerge(InDetUsedInFitTrackDecoratorToolCfg(
                    ConfigFlags,
                    name = "DFCommonUsedInFitDecoratorTool"+t.replace('InDetPseudo','Pseudo').replace('InDet','Reco').replace('TrackParticles',''),
                    AMVFVerticesDecoName = "TTVA_AMVFVertices",
                    AMVFWeightsDecoName  = "TTVA_AMVFWeights",
                    TrackContainer       = t,
                    VertexContainer      = "PrimaryVertices" ))
                DerivDecorator = acc.getPrimaryAndMerge(UsedInVertexFitTrackDecoratorCfg(
                    ConfigFlags,
                    name = "DFCommonUsedInFitDecorator"+t.replace('InDetPseudo','Pseudo').replace('InDet','Reco').replace('TrackParticles',''),
                    UsedInFitDecoratorTool = InDetDecorator ))
                PseudoTrackDecorators.append(DerivDecorator)

        # Turned off by defult for the LRT tracks as they are not used. Added this option to turn it on for future studies.
        if kwargs['DecoLRTTTVA'] and kwargs['DoR3LargeD0'] and kwargs['StoreSeparateLargeD0Container']:

            #====================================================================
            # DECORATE THE LRT TRACKS WITH USED-IN-FIT TTVA VARIABLES
            #====================================================================
            DFCommonUsedInFitDecoratorToolLRT = acc.getPrimaryAndMerge(InDetUsedInFitTrackDecoratorToolCfg(
                ConfigFlags,
                name                 = "DFCommonUsedInFitDecoratorToolLRT",
                AMVFVerticesDecoName = "TTVA_AMVFVertices",
                AMVFWeightsDecoName  = "TTVA_AMVFWeights",
                TrackContainer       = "InDetLargeD0TrackParticles",
                VertexContainer      = "PrimaryVertices" ))

            DFCommonUsedInFitDecoratorLRT = acc.getPrimaryAndMerge(UsedInVertexFitTrackDecoratorCfg(
                ConfigFlags,
                name                   = "DFCommonUsedInFitDecoratorLRT",
                UsedInFitDecoratorTool = DFCommonUsedInFitDecoratorToolLRT ))
            #=======================================
            # CREATE THE DERIVATION KERNEL ALGORITHM
            #=======================================

            acc.addEventAlgo(CommonAugmentation("InDetCommonKernel",
                                                AugmentationTools = [DFCommonTrackSelection, DFCommonZ0AtPV, DFCommonHSDecorator, DFCommonUsedInFitDecorator, DFCommonUsedInFitDecoratorLRT] ))
        else:
            AugTools = [DFCommonTrackSelection, DFCommonZ0AtPV, DFCommonHSDecorator, DFCommonUsedInFitDecorator]
            if kwargs['AddPseudoTracks']:
                AugTools += PseudoTrackDecorators
            acc.addEventAlgo(CommonAugmentation("InDetCommonKernel", AugmentationTools = AugTools))

    # Add LRT merger job to the sequence when the LRT track particle is supposed to be made already 
    if kwargs['MergeLRT'] and kwargs['DoR3LargeD0'] and kwargs['StoreSeparateLargeD0Container']: 
        #====================================================================
        # Merge the LRT and standard track using track particle merger
        #====================================================================
        from DerivationFrameworkInDet.InDetToolsConfig import TrackParticleMergerCfg 
        LRTAndStandardTrackParticleMerger = acc.getPrimaryAndMerge(TrackParticleMergerCfg(
            ConfigFlags, 
            name                        = "LRTAndStandardTrackParticleMerger",
            TrackParticleLocation       = ["InDetTrackParticles","InDetLargeD0TrackParticles"],
            OutputTrackParticleLocation = "InDetWithLRTTrackParticles",
            CreateViewColllection       = True))
        acc.addEventAlgo(CommonAugmentation("InDetLRTMerge", AugmentationTools = [LRTAndStandardTrackParticleMerger]))
        
    return acc
