################################################################################
# Package: TrigByteStreamCnvSvc
################################################################################

# Declare the package name
atlas_subdir( TrigByteStreamCnvSvc )

# Declare the package's dependencies
atlas_depends_on_subdirs( PRIVATE Event/ByteStreamCnvSvc )

# External dependencies
find_package( tdaq COMPONENTS eformat_write hltinterface )

# Libraries in the package
atlas_add_library(
  TrigByteStreamCnvSvcLib
    TrigByteStreamCnvSvc/*.h
    src/*.cxx
  PUBLIC_HEADERS
    TrigByteStreamCnvSvc
  PRIVATE_INCLUDE_DIRS
    ${TDAQ-COMMON_INCLUDE_DIRS}
  PRIVATE_LINK_LIBRARIES
    ${TDAQ-COMMON_LIBRARIES}
    AthenaBaseComps
    ByteStreamCnvSvc
)

# Components in the package
atlas_add_component(
  TrigByteStreamCnvSvc
    src/components/*.cxx
    LINK_LIBRARIES TrigByteStreamCnvSvcLib
)

# Install files from the package
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
