################################################################################
# Package: TRT_CalibTools
################################################################################

# Declare the package name:
atlas_subdir( TRT_CalibTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetCalibEvent/TRT_CalibData
                          InnerDetector/InDetRawEvent/InDetRawData
                          PRIVATE
                          Commission/CommissionEvent
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Event/EventInfo
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetConditions/TRT_ConditionsData
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/TRT_DriftCircleTool
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkTools/TrkTrackSummaryTool )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

# Component(s) in the package:
atlas_add_component( TRT_CalibTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} Identifier GaudiKernel InDetRawData CommissionEvent AthenaBaseComps StoreGateLib AtlasDetDescr EventInfo EventPrimitives xAODTracking TRT_ConditionsData TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry InDetPrepRawData InDetRIO_OnTrack TrkEventPrimitives TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( TRT_CalibTools )

