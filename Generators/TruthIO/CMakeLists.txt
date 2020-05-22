################################################################################
# Package: TruthIO
################################################################################

# Declare the package name:
atlas_subdir( TruthIO )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Generators/GeneratorModules
                          Generators/AtlasHepMC
                          PRIVATE
                          Control/StoreGate
                          Event/EventInfo
                          Generators/GeneratorObjects )

# External dependencies:
find_package( HepPDT )

# Component(s) in the package:
atlas_add_component( TruthIO
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} AtlasHepMCLib AtlasHepMCfioLib AthenaBaseComps GaudiKernel GeneratorModulesLib StoreGateLib SGtests EventInfo GeneratorObjects )

# Install files from the package:
atlas_install_headers( TruthIO )
atlas_install_joboptions( share/*.py )

