################################################################################
# Package: QGSJet_i
################################################################################

# Declare the package name:
atlas_subdir( QGSJet_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Generators/GeneratorModules
                          PRIVATE
                          Control/AthenaKernel
                          GaudiKernel
                          Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( Crmc COMPONENTS QgsjetII04 )
find_package( HepMC COMPONENTS HepMC HepMCfio )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( QGSJet_iLib
                   src/QGSJet.cxx
                   src/Ranfini.F
                   src/DRanf.F
                   src/Rangen.F
                   PUBLIC_HEADERS QGSJet_i
                   INCLUDE_DIRS ${CRMC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CRMC_LIBRARIES} GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel TruthUtils )

atlas_add_component( QGSJet_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${CRMC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CRMC_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GeneratorModulesLib AthenaKernel GaudiKernel TruthUtils QGSJet_iLib )

# Install files from the package:
atlas_install_runtime( share/qgsjet_crmc.param )

atlas_install_runtime( ${CRMC_LCGROOT}/tabs/sectnu-II-04
   ${CRMC_LCGROOT}/tabs/qgsdat-II-04.lzma )
