################################################################################
# Package: TRT_CalibTools
################################################################################

# Declare the package name:
atlas_subdir( TRT_CalibTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          GaudiKernel
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetCalibEvent/TRT_CalibData
                          InnerDetector/InDetRawEvent/InDetRawData
                          PRIVATE
                          Commission/CommissionEvent
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetConditions/TRT_ConditionsData
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
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
                          Tracking/TrkTools/TrkToolInterfaces)

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )

atlas_add_library( TRT_CalibToolsLib
                   TRT_CalibTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS TRT_CalibTools
                   LINK_LIBRARIES GaudiKernel TRT_CalibData Identifier xAODEventInfo xAODTracking )

# Component(s) in the package:
atlas_add_component( TRT_CalibTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} TRT_CalibToolsLib InDetRawData CommissionEvent AthenaBaseComps CxxUtils StoreGateLib AtlasDetDescr EventPrimitives TRT_ConditionsData TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry InDetPrepRawData InDetRIO_OnTrack TrkEventPrimitives TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkTrack TrkTrackSummary TrkToolInterfaces TRT_DriftFunctionToolLib TRT_DriftCircleToolLib )


