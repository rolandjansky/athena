################################################################################
# Package: JiveXMLServer
################################################################################

# Declare the package name:
atlas_subdir( JiveXMLServer )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          graphics/JiveXML )

# External dependencies:
find_package( tdaq COMPONENTS ipc cmdline owl omniORB4 omnithread)
find_package( tdaq-common COMPONENTS ers)

# Component(s) in the package:
atlas_add_library( JiveXMLServer
                   src/*.cxx
                   PUBLIC_HEADERS JiveXMLServer
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} GaudiKernel JiveXMLLib )

atlas_add_executable( jivexmlserver
                      bin/jivexmlserver.cc
                      INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                      LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} GaudiKernel JiveXMLLib JiveXMLServer )

