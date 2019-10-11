################################################################################
# Package: TrigByteStreamCnvSvc
################################################################################

# Declare the package name
atlas_subdir( TrigByteStreamCnvSvc )

# Declare the package's dependencies
atlas_depends_on_subdirs(
  PUBLIC
    Control/AthenaBaseComps
    Event/ByteStreamData
  PRIVATE
    Control/AthenaMonitoring
    Event/ByteStreamCnvSvcBase
    Event/ByteStreamCnvSvc
    Event/xAOD/xAODEventInfo
    GaudiKernel
    HLT/Trigger/TrigControl/TrigKernel
)

# External dependencies
find_package( tdaq-common COMPONENTS eformat eformat_write hltinterface )

# Libraries in the package
atlas_add_library(
  TrigByteStreamCnvSvcLib
    TrigByteStreamCnvSvc/*.h
    src/*.cxx
  NO_PUBLIC_HEADERS
  LINK_LIBRARIES
    AthenaBaseComps
    ByteStreamData
  PRIVATE_INCLUDE_DIRS
    ${TDAQ-COMMON_INCLUDE_DIRS}
  PRIVATE_LINK_LIBRARIES
    ${TDAQ-COMMON_LIBRARIES}
    AthenaMonitoringLib
    ByteStreamCnvSvcBaseLib
    ByteStreamCnvSvcLib
    GaudiKernel
    xAODEventInfo
    TrigKernel
)

# Components in the package
atlas_add_component(
  TrigByteStreamCnvSvc
    src/components/*.cxx
    LINK_LIBRARIES TrigByteStreamCnvSvcLib
)

# Install files from the package
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
