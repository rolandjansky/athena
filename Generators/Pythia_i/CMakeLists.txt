################################################################################
# Package: Pythia_i
################################################################################

# Declare the package name:
atlas_subdir( Pythia_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   Generators/AcerMC_i
   Generators/GeneratorModules
   Generators/Lhef_i
   Generators/Tauola_i
   PRIVATE
   Control/AthenaKernel
   Generators/AlpGen_i
   Generators/ExoGraviton_i
   Generators/GeneratorFortranCommon
   Generators/GeneratorUtils
   Generators/MadGraph_i
   Generators/TruthUtils
   Generators/CompHep_i
   Generators/MadCUP_i
   Generators/Matchig_i
   Generators/HvGen_i
   Generators/Protos_i
   Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( HepMC COMPONENTS HepMC HepMCfio )
find_package( Lhapdf )
find_package( Pythia6 )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Pythia_iLib
   src/setPythiaTune.cxx src/atlasTune.cxx
   src/Pythia.cxx src/Lhefinfo.cxx src/Pydat1.cxx
   src/Pydatr.cxx src/Pydat2.cxx src/Pydat3_62.cxx
   src/Pyssmt.cxx src/Pysubs.cxx src/Pypars.cxx
   src/Pymssm.cxx src/Pypued.cxx src/Pypevwt.cxx
   src/Pymsrv.cxx src/Pyint1.cxx src/Pyint2.cxx
   src/Pyint5.cxx src/Pytcsm.cxx src/getaddr.cxx
   src/upinit_py.F src/upevnt_py.F src/upveto_py.F
   src/cldcay.F src/glhefinfo.F src/gpydat2.F
   src/gpydatr.F src/gpypars.F src/gpysubs.F
   src/gpydat1.F src/gpydat3_62.F src/gpymssm.F
   src/gpypued.F src/gpypevwt.F src/gpymsrv.F
   src/gpyssmt.F src/gpyint1.F src/gpyint2.F
   src/gpyint5.F src/gpytcsm.F src/initpyblock.F
   src/openrandom.F src/opdcay.F src/pyr.F
   src/extproc.F src/gatlastaula_decres_py.F src/atopyt.F
   src/opensusyfile.F src/addParticle.F src/parout.F
   src/setecm.F src/PythiaDummies/pyevwt.F src/PythiaDummies/pykcut.F
   src/PythiaDummies/pytaud.F src/PythiaDummies/pytime.F
   src/PythiaDummies/sugra.F src/PythiaDummies/visaje.F
   src/PythiaDummies/ssmssm.F src/PythiaDummies/fhhiggscorr.F
   src/PythiaDummies/fhsetflags.F src/PythiaDummies/fhsetpara.F
   PUBLIC_HEADERS Pythia_i
   INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   ${HEPMC_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS} -DPY6VERSION="${PYTHIA6_VERSION}"
   LINK_LIBRARIES ${PYTHIA6_LIBRARIES} ${PYTHIA6_LIBRARY_DIRS}/pydata.o ${LHAPDF_LIBRARIES} GaudiKernel
   AcerMC_i Lhef_i GeneratorModulesLib Tauola_iLib GeneratorFortranCommonLib
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES}
   ${HEPMC_LIBRARIES} AthenaKernel AlpGen_i ExoGraviton_i MadGraph_i
   TruthUtils PathResolver CompHep_i MadCUP_i Matchig_i HvGen_i Protos_i )

atlas_add_component( Pythia_i
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel Pythia_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
