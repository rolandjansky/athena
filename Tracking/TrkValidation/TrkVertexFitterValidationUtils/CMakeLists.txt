# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVertexFitterValidationUtils )

# Component(s) in the package:
atlas_add_library( TrkVertexFitterValidationUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkVertexFitterValidationUtils
                   PRIVATE_INCLUDE_DIRS 
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel
                   PRIVATE_LINK_LIBRARIES AtlasHepMCLib GeneratorObjects ParticleTruth TrkParticleBase VxVertex )

atlas_add_component( TrkVertexFitterValidationUtils
                     src/components/*.cxx
                     LINK_LIBRARIES TrkVertexFitterValidationUtilsLib )
