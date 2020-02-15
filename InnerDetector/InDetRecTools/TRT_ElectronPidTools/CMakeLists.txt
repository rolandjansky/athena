################################################################################
# Package: TRT_ElectronPidTools
################################################################################

# Declare the package name:
atlas_subdir( TRT_ElectronPidTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool
                          Tracking/TrkEvent/TrkEventPrimitives
			  Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
			  Tracking/TrkEvent/TrkTrack
                          InnerDetector/InDetConditions/TRT_ConditionsData
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
			  Event/xAOD/xAODEventInfo
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRawEvent/InDetRawData
			  InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkRIO_OnTrack )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TRT_ElectronPidTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel AthenaBaseComps AthenaPoolUtilities GaudiKernel TRT_ConditionsServicesLib InDetPrepRawData TrkEventPrimitives TrkParameters TrkToolInterfaces Identifier InDetIdentifier InDetRawData InDetReadoutGeometry InDetRIO_OnTrack TrkSurfaces TrkMeasurementBase TrkRIO_OnTrack TrkTrack TRT_ConditionsData xAODEventInfo)

# Install files from the package:
atlas_install_headers( TRT_ElectronPidTools )

