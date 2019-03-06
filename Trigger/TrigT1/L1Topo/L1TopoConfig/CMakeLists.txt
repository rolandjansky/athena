################################################################################
# Package: L1TopoConfig
################################################################################

# Declare the package name:
atlas_subdir( L1TopoConfig )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigConfiguration/TrigConfBase
                          Trigger/TrigT1/L1Topo/L1TopoCommon )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( cx_Oracle )

# Component(s) in the package:
atlas_add_library( L1TopoConfig
                   Root/*.cxx
                   PUBLIC_HEADERS L1TopoConfig
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfBase L1TopoCommon )

atlas_add_executable( TrigConfTopoReader
                      src/test/TopoReader.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfBase L1TopoCommon L1TopoConfig )

# Install files from the package:
atlas_install_xmls( data/*.xml )

