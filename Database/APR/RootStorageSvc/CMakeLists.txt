################################################################################
# Package: RootStorageSvc
################################################################################

# Declare the package name:
atlas_subdir( RootStorageSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools
                          Control/AthContainers
                          Control/AthContainersInterfaces
                          Control/RootUtils
                          Database/APR/POOLCore
                          Database/APR/StorageSvc
                          Database/AthenaRoot/RootAuxDynIO
                          Database/PersistentDataModel
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core RIO TreePlayer Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( RootStorageSvc
                   src/*.cpp
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} RootUtilsPyROOT
                   PRIVATE_LINK_LIBRARIES TestTools AthContainers
                   RootUtils POOLCore StorageSvc RootAuxDynIO
                   PersistentDataModel GaudiKernel )


# Component list generation:
atlas_generate_componentslist( RootStorageSvc )
