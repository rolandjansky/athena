################################################################################
# Package: TrkAlignEvent
################################################################################

# Declare the package name:
atlas_subdir( TrkAlignEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          DetectorDescription/Identifier
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_library( TrkAlignEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrkAlignEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps DataModel Identifier xAODTracking GaudiKernel TrkEventPrimitives TrkTrack VxVertex
                   PRIVATE_LINK_LIBRARIES TrkDetElementBase TrkSurfaces TrkCompetingRIOsOnTrack TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkRIO_OnTrack )

