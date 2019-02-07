################################################################################
# Package: TRT_ElectronPidTools
################################################################################

# Declare the package name:
atlas_subdir( TRT_ElectronPidTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/TRT_DriftFunctionTool
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRawEvent/InDetRawData
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          InnerDetector/InDetRecTools/TRT_ToT_Tools
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( TRT_ElectronPidTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaPoolUtilities GaudiKernel TRT_ConditionsServicesLib InDetPrepRawData TrkEventPrimitives TrkToolInterfaces Identifier InDetIdentifier InDetRawData InDetRIO_OnTrack TrkSurfaces TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( TRT_ElectronPidTools )

