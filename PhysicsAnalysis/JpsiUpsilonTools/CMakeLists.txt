################################################################################
# Package: JpsiUpsilonTools
################################################################################

# Declare the package name:
atlas_subdir( JpsiUpsilonTools )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core MathCore pthread )

# Component(s) in the package:
atlas_add_library( JpsiUpsilonToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS JpsiUpsilonTools
                   INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${HEPPDT_LIBRARIES} AthenaBaseComps xAODMuon xAODTracking GaudiKernel InDetConversionFinderToolsLib TrkVKalVrtFitterLib TrkV0FitterLib xAODEgamma
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} VxVertex TrkToolInterfaces TrkVertexFitterInterfaces xAODBPhysLib AthLinks TrkVertexFitterUtilsLib )

atlas_add_component( JpsiUpsilonTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthLinks xAODMuon xAODTracking GaudiKernel InDetConversionFinderToolsLib TrkVKalVrtFitterLib VxVertex TrkToolInterfaces TrkV0FitterLib TrkVertexFitterInterfaces JpsiUpsilonToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

