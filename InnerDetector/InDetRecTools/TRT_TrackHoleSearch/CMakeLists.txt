################################################################################
# Package: TRT_TrackHoleSearch
################################################################################

# Declare the package name:
atlas_subdir( TRT_TrackHoleSearch )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigEvent/TrigDecisionInterface
                          PRIVATE
                          DetectorDescription/Identifier
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          Event/EventInfo
                          Event/EventPrimitives
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( TRT_TrackHoleSearch
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel TrkParameters TrkToolInterfaces Identifier EventInfo EventPrimitives xAODEventInfo xAODTracking TRT_ConditionsServicesLib InDetIdentifier InDetRIO_OnTrack TrkSurfaces TrkEventPrimitives TrkEventUtils TrkTrack TrkTrackSummary VxVertex TrkExInterfaces )

# Install files from the package:
atlas_install_headers( TRT_TrackHoleSearch )
atlas_install_joboptions( share/*.py )

