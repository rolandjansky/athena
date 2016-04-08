################################################################################
# Package: CollectionBase
################################################################################

# Declare the package name:
atlas_subdir( CollectionBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/APR/FileCatalog
                          PRIVATE
                          AtlasTest/TestTools
                          Database/APR/POOLCore
                          Database/APR/PersistencySvc
                          Database/PersistentDataModel
                          GaudiKernel )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CollectionBase
                   src/*.cpp
                   PUBLIC_HEADERS CollectionBase
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} FileCatalog
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools POOLCore PersistencySvc PersistentDataModel GaudiKernel )

atlas_add_dictionary( CollectionDict
                      CollectionBase/CollectionDict.h
                      CollectionBase/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} FileCatalog TestTools POOLCore PersistencySvc PersistentDataModel GaudiKernel CollectionBase )

atlas_add_test( Factory_test
                SOURCES
                test/Factory_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} FileCatalog TestTools POOLCore PersistencySvc PersistentDataModel GaudiKernel CollectionBase )

