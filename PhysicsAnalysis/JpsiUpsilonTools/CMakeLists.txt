################################################################################
# Package: JpsiUpsilonTools
################################################################################

# Declare the package name:
atlas_subdir( JpsiUpsilonTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthLinks
                          Control/AthContainers
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetConversionFinderTools
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          PRIVATE
                          Event/EventPrimitives
                          Event/xAOD/xAODBPhys
                          Tracking/TrkEvent/TrkParametersBase
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkV0Fitter
                          Tracking/TrkVertexFitter/TrkVertexAnalysisUtils
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterUtils )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( JpsiUpsilonToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS JpsiUpsilonTools
                   INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${HEPPDT_LIBRARIES} AthenaBaseComps AthLinks AthContainers xAODMuon xAODTracking GaudiKernel InDetConversionFinderToolsLib TrkVKalVrtFitterLib TrkV0FitterLib TrkVertexAnalysisUtilsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} EventPrimitives TrkParametersBase VxVertex TrkToolInterfaces TrkVertexFitterInterfaces xAODBPhys )

atlas_add_component( JpsiUpsilonTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthLinks AthContainers xAODMuon xAODTracking GaudiKernel InDetConversionFinderToolsLib TrkVKalVrtFitterLib EventPrimitives TrkParametersBase VxVertex TrkToolInterfaces TrkV0FitterLib TrkVertexAnalysisUtilsLib TrkVertexFitterInterfaces JpsiUpsilonToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

