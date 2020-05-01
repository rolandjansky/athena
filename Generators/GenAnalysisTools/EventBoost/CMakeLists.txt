################################################################################
# Package: EventBoost
################################################################################

# Declare the package name:
atlas_subdir( EventBoost )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Generators/AtlasHepMC
                          Generators/GeneratorObjects
                           )

# External dependencies:
find_package( CLHEP )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_component( EventBoost
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel StoreGateLib SGtests GeneratorObjects )

# Install files from the package:
atlas_install_headers( EventBoost )

