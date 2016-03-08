# $Id: CMakeLists.txt 727753 2016-03-04 15:00:55Z krasznaa $
################################################################################
# Package: Tauola_i
################################################################################

# Declare the package name:
atlas_subdir( Tauola_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/CLIDSvc
   Control/SGTools
   GaudiKernel
   Generators/GeneratorModules
   PRIVATE
   Control/AthenaKernel
   Generators/GeneratorFortranCommon
   Generators/GeneratorObjects
   Generators/GeneratorUtils
   Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( HepMC COMPONENTS HepMC HepMCfio )
find_package( Tauola )

# Disable the usage of the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Tauola_iLib
   src/Tauola.cxx src/Atlas_HEPEVT.cxx src/gatlas_hepevt.F
   src/Fftau.cxx src/Fftaudet.cxx src/Ffinout.cxx
   src/gfftau.F src/gffinout.F src/gfftaudet.F
   src/getaddr.cxx src/TauDetails.cxx src/tauface-jetset.F
   src/UserGamprt.cxx src/usergamprt.F src/phyfix.F
   src/tauola_photos_ini.F
   PUBLIC_HEADERS Tauola_i
   INCLUDE_DIRS ${TAUOLA_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${TAUOLA_LIBRARIES} ${HEPMC_LIBRARIES} SGTools GaudiKernel
   GeneratorModulesLib GeneratorFortranCommonLib
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GeneratorObjects
   TruthUtils )

atlas_add_component( Tauola_i src/components/*.cxx
   LINK_LIBRARIES Tauola_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
