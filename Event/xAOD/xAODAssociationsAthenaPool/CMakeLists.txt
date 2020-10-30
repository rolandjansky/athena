# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODAssociationsAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODAssociationsAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODAssociations/TrackParticleClusterAssociationContainer.h xAODAssociations/TrackParticleClusterAssociationAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrackParticleClusterAssociationContainer xAOD::TrackParticleClusterAssociationAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib xAODAssociations )
