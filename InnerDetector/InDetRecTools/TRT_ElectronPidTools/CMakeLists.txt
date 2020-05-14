################################################################################
# Package: TRT_ElectronPidTools
################################################################################

# Declare the package name:
atlas_subdir( TRT_ElectronPidTools )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TRT_ElectronPidTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaKernel AthenaBaseComps AthenaPoolUtilities GaudiKernel TRT_ConditionsServicesLib InDetPrepRawData TrkEventPrimitives TrkParameters TrkToolInterfaces Identifier InDetIdentifier InDetRawData InDetReadoutGeometry TRT_ReadoutGeometry InDetRIO_OnTrack TrkSurfaces TrkMeasurementBase TrkRIO_OnTrack TrkTrack TRT_ConditionsData xAODEventInfo)

# Install files from the package:
atlas_install_headers( TRT_ElectronPidTools )

