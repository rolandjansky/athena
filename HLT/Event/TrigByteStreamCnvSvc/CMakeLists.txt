################################################################################
# Package: TrigByteStreamCnvSvc
################################################################################

# Declare the package name
atlas_subdir( TrigByteStreamCnvSvc )

# External dependencies
find_package( tdaq-common COMPONENTS eformat eformat_write hltinterface )

# Components in the package
atlas_add_component(
  TrigByteStreamCnvSvc
    src/*.h src/*.cxx src/components/*.cxx
  INCLUDE_DIRS
    ${TDAQ-COMMON_INCLUDE_DIRS}
  LINK_LIBRARIES
    ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel ByteStreamData
    AthenaMonitoringKernelLib ByteStreamCnvSvcBaseLib ByteStreamCnvSvcLib
    CxxUtils GaudiKernel xAODEventInfo StoreGateLib TrigKernel TrigSteeringEvent )

# Install files from the package
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
