# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVertexAnalysisUtils )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrkVertexAnalysisUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkVertexAnalysisUtils
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps EventKernel EventPrimitives xAODTracking GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} TrkParametersBase TrkParticleBase VxVertex TrkExInterfaces )

atlas_add_component( TrkVertexAnalysisUtils
                     src/components/*.cxx
                     LINK_LIBRARIES TrkVertexAnalysisUtilsLib )
