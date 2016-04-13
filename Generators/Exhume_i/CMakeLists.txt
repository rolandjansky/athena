# $Id: CMakeLists.txt 727674 2016-03-04 13:01:37Z krasznaa $
################################################################################
# Package: Exhume_i
################################################################################

# Declare the package name:
atlas_subdir( Exhume_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaKernel
   GaudiKernel
   Generators/GeneratorModules
   Generators/Tauola_i
   PRIVATE
   Control/StoreGate
   Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( HepMC COMPONENTS HepMC HepMCfio )
find_package( Lhapdf )
find_package( Pythia6 )

# Turn off the use of --as-needed:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Exhume_iLib
   src/ExHuME.cxx src/Event.cpp src/GG.cpp src/QQ.cpp
   src/Higgs.cpp src/DiPhoton.cpp src/CrossSection.cpp
   src/Weight.cpp src/TwoSpace.cpp src/pdfset.f
   src/HDecay.f src/PythiaInterface.cpp src/pyinit.f
   src/simps64.F src/pyr.F src/PythiaDummies/visaje.F
   src/PythiaDummies/sugra.F src/PythiaDummies/pytime.F
   src/PythiaDummies/pykcut.F src/PythiaDummies/pyevwt.F
   src/PythiaDummies/pytaud.F src/PythiaDummies/ssmssm.F
   src/PythiaDummies/upveto.F src/PythiaDummies/fhhiggscorr.F
   src/PythiaDummies/fhsetflags.F src/PythiaDummies/fhsetpara.F
   PUBLIC_HEADERS Exhume_i
   INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS}
   ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${PYTHIA6_LIBRARIES} ${LHAPDF_LIBRARIES} ${CLHEP_LIBRARIES}
   ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel GeneratorModulesLib Tauola_iLib
   StoreGateLib SGtests
   PRIVATE_LINK_LIBRARIES TruthUtils )

atlas_add_component( Exhume_i src/components/*.cxx
   LINK_LIBRARIES Exhume_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
