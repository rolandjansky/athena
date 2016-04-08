################################################################################
# Package: TrkExInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkExInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParametersBase
                          Tracking/TrkExtrapolation/TrkExUtils )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkExInterfaces
                   PUBLIC_HEADERS TrkExInterfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives xAODTracking GaudiKernel TrkDetDescrUtils TrkSurfaces TrkVolumes TrkEventPrimitives TrkMaterialOnTrack TrkNeutralParameters TrkParameters TrkParametersBase TrkExUtils )

