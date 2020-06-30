################################################################################
# Package: SiSPSeededTrackFinderData
################################################################################

# Declare the package name:
atlas_subdir( SiSPSeededTrackFinderData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkPatternParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecEvent/SiSpacePointsSeed
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSpacePoint
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( SiSPSeededTrackFinderData
                   src/*.cxx
                   PUBLIC_HEADERS SiSPSeededTrackFinderData
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES InDetPrepRawData InDetReadoutGeometry TrkEventPrimitives TrkExInterfaces TrkGeometry TrkPatternParameters TrkTrack TrkToolInterfaces TrkEventUtils MagFieldElements MagFieldConditions InDetConditionsSummaryService
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel InDetRIO_OnTrack SiSpacePointsSeed TrkSurfaces TrkMaterialOnTrack TrkPrepRawData TrkRIO_OnTrack TrkSpacePoint)
