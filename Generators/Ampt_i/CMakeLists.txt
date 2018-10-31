################################################################################
# Package: Ampt_i
################################################################################

# Declare the package name:
atlas_subdir( Ampt_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Generators/GeneratorFortranCommon
                          Generators/GeneratorModules
                          Generators/Tauola_i
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          Generators/GeneratorObjects
                          Generators/GeneratorUtils
                          Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( HepMC COMPONENTS HepMC HepMCfio )
#find_package( Hijing )
#find_package( ROOT COMPONENTS minicern Core Tree MathCore Hist RIO pthread )

#add_definitions(-DAMPTVERSION=\"${AMPT_VERSION}\")

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Ampt_iLib
                   src/*.cxx
                   src/*.F
                   src/Ampt_modified/*.f
                   src/Ampt_modified/*.cxx
                   PUBLIC_HEADERS Ampt_i
                   INCLUDE_DIRS ${AMPT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${AMPT_LIBRARIES} ${HERWIG_LIBRARIES} ${JIMMY_LIBRARIES} ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} GeneratorFortranCommonLib GeneratorModulesLib Tauola_iLib StoreGateLib SGtests GeneratorFortranCommonLib
                   PRIVATE_LINK_LIBRARIES ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel GeneratorObjects TruthUtils )

atlas_add_component( Ampt_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${AMPT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${AMPT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GeneratorFortranCommonLib GeneratorModulesLib AthenaKernel StoreGateLib SGtests GaudiKernel GeneratorObjects TruthUtils Ampt_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

