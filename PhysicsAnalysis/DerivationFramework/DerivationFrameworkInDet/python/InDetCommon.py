# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# InDetCommon.py
# Schedules all tools needed for ID track object selection and writes
# results into SG. These may then be accessed along the train
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob

# Check file contains PrimaryVertices container (protect against non-collision data)
# If running from RAW the eventdata_items are none or emtpy
from RecExConfig.InputFilePeeker import inputFileSummary
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.BeamFlags import jobproperties
from InDetRecExample.InDetJobProperties import InDetFlags

have_PV_container = jobproperties.Beam.beamType()!="cosmics" \
    and (not inputFileSummary['eventdata_items'] or any('PrimaryVertices' in elements for elements in inputFileSummary['eventdata_items']))

if not have_PV_container and InDetFlags.doVertexFinding() and inputFileSummary['eventdata_items']:
   have_PV_container = any('PixelRDOs' in elements for elements in inputFileSummary['eventdata_items']) \
        or any('SCT_RDOs' in elements for elements in inputFileSummary['eventdata_items'])

if have_PV_container:
#====================================================================
# LABELLING TRACKS WITH OUTCOME OF SELECTOR TOOL
#====================================================================
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__InDetTrackSelectionToolWrapper
    DFCommonTrackSelection = DerivationFramework__InDetTrackSelectionToolWrapper(name           = "DFCommonTrackSelection",
                                                                                 ContainerName  = "InDetTrackParticles",
                                                                                 DecorationName = "DFCommonTightPrimary" )
    DFCommonTrackSelection.TrackSelectionTool.CutLevel = "TightPrimary"
    ToolSvc += DFCommonTrackSelection

#====================================================================
# EXPRESSION OF Z0 AT THE PRIMARY VERTEX
#====================================================================
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParametersAtPV
    DFCommonZ0AtPV = DerivationFramework__TrackParametersAtPV(name                       = "DFCommonZ0AtPV",
                                                              TrackParticleContainerName = "InDetTrackParticles",
                                                              VertexContainerName        = "PrimaryVertices", 
                                                              Z0SGEntryName              = "DFCommonInDetTrackZ0AtPV" )
    ToolSvc += DFCommonZ0AtPV

#====================================================================
# DECORATE THE HARDSCATTER VERTEX WITH A FLAG
#====================================================================
    from InDetHardScatterSelectionTool.InDetHardScatterSelectionToolConf import InDet__InDetHardScatterSelectionTool
    DFCommonHSSelectionTool = InDet__InDetHardScatterSelectionTool(name = "DFCommonHSSelectionTool")
    ToolSvc += DFCommonHSSelectionTool

    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__HardScatterVertexDecorator
    DFCommonHSDecorator = DerivationFramework__HardScatterVertexDecorator(name                     = "DFCommonHSDecorator",
                                                                          VertexContainerName      = "PrimaryVertices",
                                                                          HardScatterDecoName      = "hardScatterVertexLink",
                                                                          HardScatterSelectionTool = ToolSvc.DFCommonHSSelectionTool )
    ToolSvc += DFCommonHSDecorator

#====================================================================
# DECORATE THE TRACKS WITH USED-IN-FIT TTVA VARIABLES
#====================================================================
    from InDetUsedInFitTrackDecoratorTool.InDetUsedInFitTrackDecoratorToolConf import InDet__InDetUsedInFitTrackDecoratorTool
    DFCommonUsedInFitDecoratorTool = InDet__InDetUsedInFitTrackDecoratorTool(name                 = "DFCommonUsedInFitDecoratorTool",
                                                                              AMVFVerticesDecoName = "TTVA_AMVFVertices",
                                                                              AMVFWeightsDecoName  = "TTVA_AMVFWeights",
                                                                              TrackContainer       = "InDetTrackParticles",
                                                                              VertexContainer      = "PrimaryVertices" )
    ToolSvc += DFCommonUsedInFitDecoratorTool

    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__UsedInVertexFitTrackDecorator
    DFCommonUsedInFitDecorator = DerivationFramework__UsedInVertexFitTrackDecorator(name                   = "DFCommonUsedInFitDecorator",
                                                                                    UsedInFitDecoratorTool = ToolSvc.DFCommonUsedInFitDecoratorTool )
    ToolSvc += DFCommonUsedInFitDecorator

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

    DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("InDetCommonKernel",
                                                                             AugmentationTools = [DFCommonTrackSelection, DFCommonZ0AtPV, DFCommonHSDecorator, DFCommonUsedInFitDecorator] )
