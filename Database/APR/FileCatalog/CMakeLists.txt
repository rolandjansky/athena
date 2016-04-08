################################################################################
# Package: FileCatalog
################################################################################

# Declare the package name:
atlas_subdir( FileCatalog )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/APR/POOLCore
                          PRIVATE
                          AtlasTest/TestTools
                          GaudiKernel )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( FileCatalog
                   src/*.cpp
                   PUBLIC_HEADERS FileCatalog
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore
                   PRIVATE_LINK_LIBRARIES TestTools GaudiKernel )

atlas_add_test( FCEntry_test
                SOURCES
                test/FCEntry_test.cxx
                INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_test( QueryParser_test
                SOURCES
                test/QueryParser_test.cxx
                INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_test( URIParser_test
                SOURCES
                test/URIParser_test.cxx
                INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCaddMetaData
                      utilities/FCaddMetaData.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCaddReplica
                      utilities/FCaddReplica.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCcreateMetaDataSpec
                      utilities/FCcreateMetaDataSpec.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCdeleteEntry
                      utilities/FCdeleteEntry.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCdeleteLFN
                      utilities/FCdeleteLFN.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCdeleteMetaData
                      utilities/FCdeleteMetaData.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCdeletePFN
                      utilities/FCdeletePFN.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCdescribeMetaData
                      utilities/FCdescribeMetaData.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCdropMetaData
                      utilities/FCdropMetaData.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FClistGUID
                      utilities/FClistGUID.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FClistLFN
                      utilities/FClistLFN.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FClistMetaData
                      utilities/FClistMetaData.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FClistPFN
                      utilities/FClistPFN.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCpublish
                      utilities/FCpublish.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCregisterLFN
                      utilities/FCregisterLFN.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCregisterPFN
                      utilities/FCregisterPFN.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCrenamePFN
                      utilities/FCrenamePFN.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

atlas_add_executable( FCupdateMetaDataSpec
                      utilities/FCupdateMetaDataSpec.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} POOLCore TestTools GaudiKernel FileCatalog )

