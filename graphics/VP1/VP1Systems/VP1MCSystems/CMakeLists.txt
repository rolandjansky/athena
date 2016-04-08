################################################################################
# Package: VP1MCSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1MCSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          Generators/GeneratorObjects
                          graphics/VP1/VP1Utils )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_library( VP1MCSystems
                   src/*.c*
                   PUBLIC_HEADERS VP1MCSystems
                   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES VP1Base StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel GeneratorObjects VP1Utils )

