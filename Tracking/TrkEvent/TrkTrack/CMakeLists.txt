################################################################################
# Package: TrkTrack
################################################################################

# Declare the package name:
atlas_subdir( TrkTrack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthLinks
                          Control/AthContainers
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrackLink
                          DetectorDescription/Identifier
                          PRIVATE
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkTrackSummary )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkTrack
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthLinks AthContainers TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase TrkNeutralParameters TrkParameters TrkTrackLink
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} Identifier GaudiKernel TrkSurfaces TrkTrackSummary )

atlas_add_dictionary( TrkTrackDict
                      TrkTrack/TrkTrackDict.h
                      TrkTrack/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks AthContainers TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase TrkNeutralParameters TrkParameters Identifier GaudiKernel TrkSurfaces TrkTrackSummary TrkTrack
                      ELEMENT_LINKS TrackCollection )

