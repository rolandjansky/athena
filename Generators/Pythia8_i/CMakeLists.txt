################################################################################
# Package: Pythia8_i
################################################################################

# Declare the package name:
atlas_subdir( Pythia8_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Generators/GeneratorModules
                          Generators/AtlasHepMC
                          PRIVATE
                          GaudiKernel
                          Generators/GeneratorObjects
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Lhapdf )
find_package( Pythia8 )

# The _VERSION variable is unreliable.
add_definitions(-DPY8INCLUDE_DIR=\"${PYTHIA8_INCLUDE_DIR}\")

# Remove the --as-needed linker flags:
atlas_disable_as_needed()
if (HEPMC3_USE)
set( HEPMC_PYTHIA8_INTERFACE_DIR ${HEPMC3_INCLUDE_DIR}/../share/HepMC3/interfaces/pythia8/include/Pythia8)
else()
set( HEPMC_PYTHIA8_INTERFACE_DIR ${PYTHIA8_INCLUDE_DIRS})
endif()
# Component(s) in the package:
atlas_add_library( Pythia8_iLib
                   src/Pythia8_i.cxx
                   src/UserProcessFactory.cxx
                   src/UserHooksFactory.cxx
                   src/UserResonanceFactory.cxx
                   src/UserProcesses/Sigma2qqbar2emu.cxx
                   src/UserProcesses/Sigma2qqbar2lStarlBar.cxx
                   src/UserProcesses/Sigma2qqbar2lStarlStarBar.cxx
                   src/UserHooks/WZVetoedShower.cxx
                   src/UserHooks/QCDVetoedShower.cxx
                   src/UserHooks/PoWHEGVetoedShower.cxx
                   src/UserHooks/GravFlat.cxx
                   src/UserHooks/JetMatchingMadGraphHook.cxx
                   src/UserHooks/SuppressMPI.cxx
                   src/UserHooks/EnhanceMPI.cxx
                   src/UserHooks/ISRVetoedShower.cxx
                   src/UserHooks/PTRelVetoedShower.cxx
                   src/UserHooks/WprimeFlat.cxx
                   src/UserHooks/WprimeWZFlat.cxx
                   src/UserHooks/mHatReweight.cxx
                   src/UserHooks/main31.cxx
                   src/UserHooks/SingleTopWideEta.cxx
                   src/UserHooks/SettableColourReconnection.cxx
                   src/UserHooks/PowhegBB4L.cxx
                   src/UserResonances/ResonanceExcitedCI.cxx
                   src/UserResonances/ResonanceLQ.cxx
                   PUBLIC_HEADERS Pythia8_i
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS} ${HEPMC_PYTHIA8_INTERFACE_DIR}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${LHAPDF_LIBRARIES} ${CLHEP_LIBRARIES} ${PYTHIA8_LIBRARIES} AthenaKernel GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES GaudiKernel GeneratorObjects PathResolver )

atlas_add_component( Pythia8_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS} ${HEPMC_PYTHIA8_INTERFACE_DIR}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${LHAPDF_LIBRARIES} ${CLHEP_LIBRARIES} ${PYTHIA8_LIBRARIES} AthenaKernel GeneratorModulesLib GaudiKernel GeneratorObjects PathResolver Pythia8_iLib )

# Install files from the package:
atlas_install_joboptions( share/common/*.py
                          share/example/*.py )

