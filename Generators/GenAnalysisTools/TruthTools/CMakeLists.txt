################################################################################
# Package: TruthTools
################################################################################

# Declare the package name:
atlas_subdir( TruthTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Generators/GenInterfaces
                          Generators/GeneratorObjects
                          Generators/TruthUtils )

# External dependencies:
find_package( HepMC )
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( TruthTools
                     src/TruthSelector.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps GaudiKernel GeneratorObjects TruthUtils )

# Install files from the package:
atlas_install_headers( TruthTools )

