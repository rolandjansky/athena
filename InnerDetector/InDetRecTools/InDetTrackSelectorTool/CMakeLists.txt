################################################################################
# Package: InDetTrackSelectorTool
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackSelectorTool )


# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetTrackSelectorTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel InDetRecToolInterfaces TrkEventPrimitives TrkParameters TrkToolInterfaces AthContainers GeoPrimitives EventPrimitives TRT_ConditionsServicesLib InDetRIO_OnTrack TrkSurfaces TrkParticleBase TrkRIO_OnTrack TrkTrack TrkTrackSummary VxVertex TrkExInterfaces MagFieldElements MagFieldConditions BeamSpotConditionsData )

# Install files from the package:
atlas_install_headers( InDetTrackSelectorTool )

