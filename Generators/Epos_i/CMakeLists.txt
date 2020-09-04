################################################################################
# Package: Epos_i
################################################################################

# Declare the package name:
atlas_subdir( Epos_i )

#set( CRMC_LCGVERSION 1.5.6 )
#set( CRMC_LCGROOT /afs/cern.ch/user/e/ewelina/workarea/AthGeneration/epos/crmc.v1.5.6.r4962/installed )
# /cvmfs/sft.cern.ch/lcg/releases/LCG_88/MCGenerators/pythia8/${PYTHIA8_VERSION}/${LCG_PLATFORM} )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Generators/GeneratorModules
                          PRIVATE
                          Control/AthenaKernel
                          GaudiKernel
                          Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( Crmc COMPONENTS Epos )
find_package( HepMC COMPONENTS HepMC HepMCfio )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Epos_iLib
                   src/Epos.cxx
                   src/Ranfini.F
                   src/DRanf.F
                   src/Rangen.F
                   PUBLIC_HEADERS Epos_i
                   INCLUDE_DIRS ${CRMC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CRMC_LIBRARIES} GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel TruthUtils )

atlas_add_component( Epos_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${CRMC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CRMC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GeneratorModulesLib AthenaKernel GaudiKernel TruthUtils Epos_iLib )

# Install files from the package:
atlas_install_joboptions( share/common/*.py )
atlas_install_runtime( share/file/epos_crmc.param
                       ${CRMC_LCGROOT}/tabs/dpmjet.dat
                       ${CRMC_LCGROOT}/tabs/epos.initl
                       ${CRMC_LCGROOT}/tabs/epos.iniev
                       ${CRMC_LCGROOT}/tabs/epos.inirj
                       ${CRMC_LCGROOT}/tabs/epos.inirj.lhc
                       ${CRMC_LCGROOT}/tabs/epos.inics
                       ${CRMC_LCGROOT}/tabs/epos.inics.lhc
                       ${CRMC_LCGROOT}/tabs/epos.inidi )

set( EposEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of EposEnvironment.cmake" )
find_package( EposEnvironment )
