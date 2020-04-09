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
                          MagneticField/MagFieldInterfaces
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

# Component(s) in the package:
atlas_add_library( SiSPSeededTrackFinderData
                   src/*.cxx
                   PUBLIC_HEADERS SiSPSeededTrackFinderData
                   LINK_LIBRARIES InDetPrepRawData InDetReadoutGeometry MagFieldInterfaces TrkEventPrimitives TrkExInterfaces TrkGeometry TrkPatternParameters TrkTrack TrkToolInterfaces TrkEventUtils MagFieldElements MagFieldConditions
                   PRIVATE_LINK_LIBRARIES GaudiKernel InDetRIO_OnTrack SiSpacePointsSeed TrkSurfaces TrkMaterialOnTrack TrkPrepRawData TrkRIO_OnTrack TrkSpacePoint)
