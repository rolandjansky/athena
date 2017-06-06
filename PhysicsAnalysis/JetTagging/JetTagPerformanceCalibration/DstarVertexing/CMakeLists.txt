################################################################################
# Package: DstarVertexing
################################################################################

# Declare the package name:
atlas_subdir( DstarVertexing )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PRIVATE
                          Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTruth
                          InnerDetector/InDetRecTools/InDetConversionFinderTools
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkV0Fitter
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          Tracking/TrkVertexFitter/TrkVertexAnalysisUtils
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( DstarVertexing
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps DataModel xAODTracking GaudiKernel EventPrimitives xAODEventInfo xAODJet xAODTruth InDetConversionFinderToolsLib TrkToolInterfaces TrkV0FitterLib TrkVKalVrtFitterLib TrkVertexAnalysisUtilsLib TrkVertexFitterInterfaces)

# Install files from the package:
atlas_install_headers( DstarVertexing )

