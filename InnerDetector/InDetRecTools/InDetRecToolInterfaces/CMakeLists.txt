################################################################################
# Package: InDetRecToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( InDetRecToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/SiSpacePointsSeed
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkSpacePoint
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( InDetRecToolInterfaces
                   PUBLIC_HEADERS InDetRecToolInterfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives IRegionSelector Identifier xAODCaloEvent xAODTracking GaudiKernel InDetPrepRawData SiSpacePointsSeed TrkEventPrimitives TrkMeasurementBase TrkParameters TrkParticleBase TrkSegment TrkSpacePoint TrkTrack VxVertex )

