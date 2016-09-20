################################################################################
# Package: InDetTrackSummaryHelperTool
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackSummaryHelperTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack )

# Component(s) in the package:
atlas_add_component( InDetTrackSummaryHelperTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TRT_ConditionsServicesLib TrkEventPrimitives TrkTrackSummary TrkToolInterfaces Identifier InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetRecToolInterfaces TrkCompetingRIOsOnTrack TrkParameters TrkRIO_OnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( InDetTrackSummaryHelperTool )

