# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VrtSecInclusive )

# External dependencies:
find_package( BLAS )
find_package( LAPACK )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( VrtSecInclusiveLib
                   src/*.cxx
                   PUBLIC_HEADERS VrtSecInclusive
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIRS} ${BLAS_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES} AthenaBaseComps xAODTracking xAODTruth GaudiKernel GeneratorObjects ITrackToVertex TrkDetDescrInterfaces TrkSurfaces TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces TrkVKalVrtFitterLib xAODEventInfo
                   PRIVATE_LINK_LIBRARIES TrkTrackSummary TrkVKalVrtCore VxVertex )

atlas_add_component( VrtSecInclusive
                     src/components/*.cxx
                     LINK_LIBRARIES VrtSecInclusiveLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

