################################################################################
# Package: RootCollection
################################################################################

# Declare the package name:
atlas_subdir( RootCollection )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/RootUtils
                          Database/APR/CollectionBase
                          Database/APR/FileCatalog
                          Database/APR/POOLCore
                          Database/APR/PersistencySvc
                          Database/PersistentDataModel
                          GaudiKernel )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS MathCore Hist Tree TreePlayer Net RIO Cint Core pthread )

# Component(s) in the package:
atlas_add_root_dictionary( RootCollection
                           RootCollectionDictSource
                           ROOT_HEADERS RootCollection/AttributeListLayout.h RootCollection/LinkDef.h
                           EXTERNAL_PACKAGES ROOT CORAL )

atlas_add_library( RootCollection
                   src/*.cpp
                   ${RootCollectionDictSource}
                   PUBLIC_HEADERS RootCollection
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} RootUtilsPyROOT
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} TestTools AthenaKernel RootUtils CollectionBase FileCatalog POOLCore PersistencySvc PersistentDataModel GaudiKernel )

atlas_add_library( RootCollectionComponents
                   src/components/*.cpp
                   PUBLIC_HEADERS RootCollection
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} RootUtilsPyROOT RootCollection
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} TestTools AthenaKernel RootUtils CollectionBase FileCatalog POOLCore PersistencySvc PersistentDataModel GaudiKernel )

atlas_add_test( read_test
                SOURCES
                test/read_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} TestTools AthenaKernel RootUtils RootUtilsPyROOT CollectionBase FileCatalog POOLCore PersistencySvc PersistentDataModel GaudiKernel RootCollection RootCollectionComponents )

atlas_add_test( update_test
                SOURCES
                test/update_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} TestTools AthenaKernel RootUtils RootUtilsPyROOT CollectionBase FileCatalog POOLCore PersistencySvc PersistentDataModel GaudiKernel RootCollection RootCollectionComponents )

# Component list generation:
atlas_generate_componentslist( RootCollectionComponents )
