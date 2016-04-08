################################################################################
# Package: InDetCompetingRIOsOnTrackTool
################################################################################

# Declare the package name:
atlas_subdir( InDetCompetingRIOsOnTrackTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( InDetCompetingRIOsOnTrackTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives GaudiKernel InDetCompetingRIOsOnTrack TrkParameters TrkToolInterfaces InDetPrepRawData TrkSurfaces TrkEventPrimitives TrkExInterfaces )

# Install files from the package:
atlas_install_headers( InDetCompetingRIOsOnTrackTool )

