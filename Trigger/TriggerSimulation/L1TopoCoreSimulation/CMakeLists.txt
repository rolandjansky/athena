################################################################################
# Package: L1TopoCoreSimulation
################################################################################

# Declare the package name:
atlas_subdir( L1TopoCoreSimulation )

# Component(s) in the package:
atlas_add_library( L1TopoCoreSimulation
                   src/*.cxx
                   PUBLIC_HEADERS L1TopoCoreSimulation )

atlas_add_executable( TrigConfTopoTest
                      src/test/TopoTest.cxx
                      LINK_LIBRARIES L1TopoCoreSimulation )

