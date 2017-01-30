################################################################################
# Package: TrigConfMuctpi
################################################################################

# Declare the package name:
atlas_subdir( TrigConfMuctpi )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigConfiguration/TrigConfStorage )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( TrigConfMuctpi
                   src/*.cxx
                   PUBLIC_HEADERS TrigConfMuctpi
                   INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${XERCESC_LIBRARIES} TrigConfStorage
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_executable( TrigConfReadWriteMuCTPiGeometry
                      src/test/ReadWriteMuCTPiGeometry.cxx
                      INCLUDE_DIRS ${XERCESC_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${XERCESC_LIBRARIES} ${Boost_LIBRARIES} TrigConfStorage TrigConfMuctpi )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/*.py )
atlas_install_xmls( data/*.dtd )

