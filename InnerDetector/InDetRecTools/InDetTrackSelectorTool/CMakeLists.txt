################################################################################
# Package: InDetTrackSelectorTool
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackSelectorTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/AthContainers
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetTrackSelectorTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel InDetRecToolInterfaces TrkEventPrimitives TrkParameters TrkToolInterfaces AthContainers GeoPrimitives EventPrimitives TRT_ConditionsServicesLib InDetRIO_OnTrack MagFieldInterfaces TrkSurfaces TrkParticleBase TrkRIO_OnTrack TrkTrack TrkTrackSummary VxVertex TrkExInterfaces )

# Install files from the package:
atlas_install_headers( InDetTrackSelectorTool )

