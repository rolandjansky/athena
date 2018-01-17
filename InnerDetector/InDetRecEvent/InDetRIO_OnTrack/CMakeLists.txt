################################################################################
# Package: InDetRIO_OnTrack
################################################################################

# Declare the package name:
atlas_subdir( InDetRIO_OnTrack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Event/EventContainers
                          PRIVATE
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkDetDescr/TrkSurfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetRIO_OnTrack
                   src/*.cxx
                   PUBLIC_HEADERS InDetRIO_OnTrack
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthLinks Identifier GaudiKernel InDetPrepRawData TrkEventPrimitives TrkRIO_OnTrack EventContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} InDetReadoutGeometry TrkSurfaces )

atlas_add_dictionary( InDetRIO_OnTrackDict
                      InDetRIO_OnTrack/InDetRIO_OnTrackDict.h
                      InDetRIO_OnTrack/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks Identifier GaudiKernel InDetPrepRawData TrkEventPrimitives TrkRIO_OnTrack InDetReadoutGeometry TrkSurfaces InDetRIO_OnTrack EventContainers )

