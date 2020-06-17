# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# InDetCommon.py 
# Schedules all tools needed for ID track object selection and writes
# results into SG. These may then be accessed along the train   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

# Check file contains PrimaryVertices container (protect against non-collision data)
# If running from RAW the eventdata_items are none or emtpy
from RecExConfig.InputFilePeeker import inputFileSummary
from AthenaCommon.BeamFlags import jobproperties
from InDetRecExample.InDetJobProperties import InDetFlags

have_PV_container = jobproperties.Beam.beamType()!="cosmics" \
    and (not inputFileSummary['eventdata_items'] or any('PrimaryVertices' in elements for elements in inputFileSummary['eventdata_items']))

if not have_PV_container and InDetFlags.doVertexFinding() and inputFileSummary['eventdata_items']:
   have_PV_container = any('PixelRDOs' in elements for elements in inputFileSummary['eventdata_items']) \
          or any('SCT_RDOs' in elements for elements in inputFileSummary['eventdata_items'])

if have_PV_container :
#====================================================================
# LABELLING TRACKS WITH OUTCOME OF SELECTOR TOOL
#====================================================================

    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__InDetTrackSelectionToolWrapper
    DFCommonTrackSelection = DerivationFramework__InDetTrackSelectionToolWrapper(name = "DFCommonTrackSelection",
                                                                                 ContainerName = "InDetTrackParticles",
                                                                                 DecorationName = "DFCommonTightPrimary" )
    DFCommonTrackSelection.TrackSelectionTool.CutLevel = "TightPrimary"
    ToolSvc += DFCommonTrackSelection

#====================================================================
# EXPRESSION OF Z0 AT THE PRIMARY VERTEX
#====================================================================
    from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParametersAtPV
    DFCommonZ0AtPV = DerivationFramework__TrackParametersAtPV(name = "DFCommonZ0AtPV",
                                                              TrackParticleContainerName = "InDetTrackParticles",
                                                              VertexContainerName = "PrimaryVertices", 
                                                              Z0SGEntryName = "DFCommonInDetTrackZ0AtPV" )
    ToolSvc += DFCommonZ0AtPV
    


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
    
    from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
    DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("InDetCommonKernel",
                                                                             AugmentationTools = [DFCommonTrackSelection,DFCommonZ0AtPV]
                                                                             )
