################################################################################
# Package: XMLCatalog
################################################################################

# Declare the package name:
atlas_subdir( XMLCatalog )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/APR/FileCatalog
                          GaudiKernel
                          PRIVATE
                          Database/PersistentDataModel
                          TestPolicy )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( CppUnit )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( XMLCatalog
                   src/*.cpp
                   PUBLIC_HEADERS XMLCatalog
                   PRIVATE_INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${CPPUNIT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES FileCatalog GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${XERCESC_LIBRARIES} ${CPPUNIT_LIBRARIES} ${CORAL_LIBRARIES} PersistentDataModel )

# Component list generation:
atlas_generate_componentslist( XMLCatalog )
