################################################################################
# Package: SiCombinatorialTrackFinderTool_xk
################################################################################

# Declare the package name:
atlas_subdir( SiCombinatorialTrackFinderTool_xk )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/SiSPSeededTrackFinderData
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkPatternParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# Component(s) in the package:
atlas_add_component( SiCombinatorialTrackFinderTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetReadoutGeometry InDetPrepRawData SiSPSeededTrackFinderData InDetRecToolInterfaces TrkGeometry TrkEventPrimitives TrkPatternParameters TrkTrack TrkExInterfaces TrkToolInterfaces InDetRIO_OnTrack TrkSurfaces TrkMaterialOnTrack TrkMeasurementBase TrkRIO_OnTrack )

# Install files from the package:
atlas_install_headers( SiCombinatorialTrackFinderTool_xk )

