# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VrtSecInclusive )

# External dependencies:
find_package( LAPACK )
find_package( BLAS )
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( VrtSecInclusiveLib
                   src/*.cxx
                   PUBLIC_HEADERS VrtSecInclusive
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIRS} ${BLAS_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES} AthenaBaseComps xAODEventInfo xAODTracking xAODTruth xAODMuon xAODEgamma xAODJet GaudiKernel GeneratorObjects ITrackToVertex TrkDetDescrInterfaces TrkSurfaces TrkExInterfaces InDetConditionsSummaryService TrkToolInterfaces TrkVertexFitterInterfaces StoreGateLib SGtests TrkVKalVrtFitterLib AtlasDetDescr InDetIdentifier MVAUtils PathResolver
                   PRIVATE_LINK_LIBRARIES xAODEventInfo TrkTrackSummary VxVertex )

atlas_add_component( VrtSecInclusive
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIRS} ${BLAS_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES} AthenaBaseComps StoreGateLib SGtests xAODEventInfo xAODTracking xAODTruth xAODMuon xAODEgamma xAODJet GaudiKernel GeneratorObjects ITrackToVertex TrkDetDescrInterfaces TrkSurfaces TrkExInterfaces InDetConditionsSummaryService TrkToolInterfaces TrkVKalVrtFitterLib TrkVertexFitterInterfaces xAODEventInfo TrkTrackSummary VxVertex VrtSecInclusiveLib AtlasDetDescr InDetIdentifier )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8})
atlas_install_joboptions( share/*.py )
