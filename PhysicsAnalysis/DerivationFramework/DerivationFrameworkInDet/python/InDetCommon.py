# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#********************************************************************
# InDetCommon.py 
# Schedules all tools needed for ID track object selection and writes
# results into SG. These may then be accessed along the train   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *

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
                                                                         AugmentationTools = [DFCommonZ0AtPV]
                                                                         )
