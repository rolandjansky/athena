################################################################################
# Package: TRT_ElectronPidTools
################################################################################

# Declare the package name:
atlas_subdir( TRT_ElectronPidTools )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( TRT_ElectronPidToolsLib
                   TRT_ElectronPidTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS TRT_ElectronPidTools
                   LINK_LIBRARIES GaudiKernel AthenaBaseComps AthenaKernel TrkTrack InDetRawData TrkParameters InDetRIO_OnTrack TrkToolInterfaces TRT_ConditionsData TrkEventPrimitives TRT_ConditionsServicesLib InDetPrepRawData StoreGateLib TRT_ConditionsData )

# Component(s) in the package:
atlas_add_component( TRT_ElectronPidTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} TRT_ElectronPidToolsLib AthenaPoolUtilities Identifier InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry TrkSurfaces TrkMeasurementBase TrkRIO_OnTrack xAODEventInfo)

