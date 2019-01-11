################################################################################
# Package: Pythia8_i
################################################################################

# Declare the package name:
atlas_subdir( Pythia8_i )

# set( PYTHIA8_VERSION 226 )
# set( PYTHIA8_ROOT    
# /cvmfs/sft.cern.ch/lcg/releases/LCG_88/MCGenerators/pythia8/${PYTHIA8_VERSION}/${LCG_PLATFORM} )
# MESSAGE (STATUS "Pythia8 version  " ${PYTHIA8_VERSION})
# MESSAGE (STATUS "PYTHIA8 ROOT " ${PYTHIA8_ROOT})

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Generators/GeneratorModules
                          PRIVATE
                          GaudiKernel
                          Generators/GeneratorObjects
                          Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( Lhapdf )
find_package( Pythia8 )

add_definitions(-DPY8VERSION=\"${PYTHIA8_VERSION}\")

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

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
                   src/UserHooks/JetMergingaMCatNLO.cxx
                   src/UserHooks/SuppressMPI.cxx
                   src/UserHooks/EnhanceMPI.cxx
                   src/UserHooks/ISRVetoedShower.cxx
                   src/UserHooks/PTRelVetoedShower.cxx
                   src/UserHooks/WprimeFlat.cxx
                   src/UserHooks/ZprimeFlatpT.cxx
                   src/UserHooks/WprimeWZFlat.cxx
		   src/UserHooks/WZPolarization.cxx
                   src/UserHooks/mHatReweight.cxx
                   src/UserHooks/main31.cxx
                   src/UserResonances/ResonanceExcitedCI.cxx
                   src/UserResonances/ResonanceLQ.cxx
		   src/Pythia8Custom/CheckForFinalPartons.cxx
                   src/Pythia8Custom/HllgamRepeatTimeShower.cxx
                   src/Pythia8Custom/UserPhotonFlux.cxx
                   PUBLIC_HEADERS Pythia8_i
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${LHAPDF_LIBRARIES} ${CLHEP_LIBRARIES} ${PYTHIA8_LIBRARIES} AthenaKernel GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES GaudiKernel GeneratorObjects PathResolver )

atlas_add_component( Pythia8_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${LHAPDF_LIBRARIES} ${CLHEP_LIBRARIES} ${PYTHIA8_LIBRARIES} AthenaKernel GeneratorModulesLib GaudiKernel GeneratorObjects PathResolver Pythia8_iLib )

# Install files from the package:
atlas_install_joboptions( share/common/*.py
                          share/example/*.py )

set( Pythia8Environment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of Pythia8Environment.cmake" )
find_package( Pythia8Environment )
