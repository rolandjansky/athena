# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetV0Finder )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( InDetV0FinderLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetV0Finder
                   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthenaBaseComps BeamSpotConditionsData GaudiKernel StoreGateLib xAODTracking
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ITrackToVertex InDetConversionFinderToolsLib TrkExInterfaces TrkToolInterfaces TrkV0FitterLib TrkVKalVrtFitterLib TrkVertexAnalysisUtilsLib TrkVertexFitterInterfaces )

atlas_add_component( InDetV0Finder
                     src/components/*.cxx
                     LINK_LIBRARIES InDetV0FinderLib )
