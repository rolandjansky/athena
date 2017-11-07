################################################################################
# Package: PythiaExo_i
################################################################################

# Declare the package name:
atlas_subdir( PythiaExo_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   Generators/AcerMC_i
   Generators/GeneratorModules
   Generators/Pythia_i
   Generators/Tauola_i
   PRIVATE
   Control/AthenaKernel
   Control/StoreGate
   Generators/AlpGen_i
   Generators/ExoGraviton_i
   Generators/GeneratorFortranCommon
   Generators/GeneratorUtils
   Generators/GravADD_i
   Generators/MadGraph_i
   Generators/TruthUtils
   Generators/CompHep_i
   Generators/MadCUP_i
   Generators/Matchig_i
   Generators/HvGen_i
   Tools/PathResolver )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( GSL )
find_package( HepMC COMPONENTS HepMC HepMCfio )
find_package( Lhapdf )
find_package( Pythia6 )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_component( PythiaTau3Mu_i
   src/PythiaTau3Mu.cxx src/Pydat1.cxx src/Pydatr.cxx src/Pydat2.cxx
   src/Pydat3_62.cxx src/Pyssmt.cxx src/Pysubs.cxx src/Pypars.cxx
   src/Pymssm.cxx src/Pyint1.cxx src/Pyint2.cxx src/Pyint5.cxx
   src/Pytcsm.cxx src/upinit_py.F src/upevnt_py.F src/upveto_py.F
   src/cldcay.F src/gpydat2.F src/gpydatr.F src/gpypars.F
   src/gpysubs.F src/gpydat1.F src/gpydat3_62.F src/gpymssm.F
   src/gpyssmt.F src/gpyint1.F src/gpyint2.F src/gpyint5.F
   src/gpytcsm.F src/initpyblock.F src/openrandom.F
   src/opdcay.F src/pyr.F src/extproc.F src/gatlastaula_decres_py.F
   src/atopyt.F src/opensusyfile.F src/PythiaDummies/pyevwt.F
   src/PythiaDummies/pykcut.F src/PythiaDummies/pytime.F
   src/PythiaDummies/sugra.F src/PythiaDummies/visaje.F
   src/PythiaDummies/ssmssm.F src/PythiaDummies/fhhiggscorr.F
   src/PythiaDummies/fhsetflags.F src/PythiaDummies/fhsetpara.F
   src/PythiaModified/pytaud.F src/PythiaTau3Mu_i_entries.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${GSL_LIBRARIES} ${CLHEP_LIBRARIES}
   ${PYTHIA6_LIBRARIES} ${LHAPDF_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel
   AcerMC_i GeneratorModulesLib Pythia_iLib Tauola_iLib AthenaKernel StoreGateLib
   AlpGen_i ExoGraviton_i GeneratorFortranCommonLib GravADD_i MadGraph_i
   TruthUtils PathResolver CompHep_i MadCUP_i Matchig_i HvGen_i )

atlas_add_component( PythiaMono_i
   src/PythiaMono.cxx src/Pydat1.cxx src/Pydatr.cxx src/Pydat2.cxx
   src/Pydat3_62.cxx src/Pyssmt.cxx src/Pysubs.cxx src/Pypars.cxx
   src/Pymssm.cxx src/Pyint1.cxx src/Pyint2.cxx src/Pyint5.cxx
   src/Pytcsm.cxx src/upinit_py.F src/upevnt_py.F src/upveto_py.F
   src/cldcay.F src/gpydat2.F src/gpydatr.F src/gpypars.F
   src/gpysubs.F src/gpydat1.F src/gpydat3_62.F src/gpymssm.F
   src/gpyssmt.F src/gpyint1.F src/gpyint2.F src/gpyint5.F
   src/gpytcsm.F src/initpyblock.F src/openrandom.F src/opdcay.F
   src/pyr.F src/extproc.F src/gatlastaula_decres_py.F src/atopyt.F
   src/opensusyfile.F src/PythiaDummies/pyevwt.F src/PythiaDummies/pykcut.F
   src/PythiaDummies/pytaud.F src/PythiaDummies/pytime.F
   src/PythiaDummies/sugra.F src/PythiaDummies/visaje.F
   src/PythiaDummies/ssmssm.F src/PythiaDummies/fhhiggscorr.F
   src/PythiaDummies/fhsetflags.F src/PythiaDummies/fhsetpara.F
   src/PythiaModified/pysgwz.F src/PythiaMono_i_entries.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${GSL_LIBRARIES} ${CLHEP_LIBRARIES}
   ${PYTHIA6_LIBRARIES} ${LHAPDF_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel
   AcerMC_i GeneratorModulesLib Pythia_iLib Tauola_iLib AthenaKernel StoreGateLib
   AlpGen_i ExoGraviton_i GeneratorFortranCommonLib GravADD_i MadGraph_i
   TruthUtils PathResolver CompHep_i MadCUP_i Matchig_i HvGen_i )

atlas_add_component( PythiaGS_i
   src/PythiaGS.cxx src/Pydat1.cxx src/Pydatr.cxx src/Pydat2.cxx
   src/Pydat3_62.cxx src/Pyssmt.cxx src/Pysubs.cxx src/Pypars.cxx
   src/Pymssm.cxx src/Pyint1.cxx src/Pyint2.cxx src/Pyint5.cxx
   src/Pytcsm.cxx src/upinit_py.F src/upevnt_py.F src/upveto_py.F
   src/cldcay.F src/gpydat2.F src/gpydatr.F src/gpypars.F
   src/gpysubs.F src/gpydat1.F src/gpydat3_62.F src/gpymssm.F
   src/gpyssmt.F src/gpyint1.F src/gpyint2.F src/gpyint5.F
   src/gpytcsm.F src/initpyblock.F src/openrandom.F src/opdcay.F
   src/pyr.F src/extproc.F src/gatlastaula_decres_py.F src/atopyt.F
   src/opensusyfile.F src/PythiaDummies/pytime.F src/PythiaDummies/sugra.F
   src/PythiaDummies/visaje.F src/PythiaDummies/ssmssm.F
   src/PythiaDummies/fhhiggscorr.F src/PythiaDummies/fhsetflags.F
   src/PythiaDummies/fhsetpara.F src/PythiaModified/pytaud.F
   src/PythiaGS_i_entries.cxx
   src/PythiaGravADD/Aeik.c src/PythiaGravADD/pyeffevt.F
   src/PythiaGravADD/GravScat.F
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${GSL_LIBRARIES} ${CLHEP_LIBRARIES}
   ${PYTHIA6_LIBRARIES} ${LHAPDF_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel
   AcerMC_i GeneratorModulesLib Pythia_iLib Tauola_iLib AthenaKernel StoreGateLib
   AlpGen_i ExoGraviton_i GeneratorFortranCommonLib GravADD_i MadGraph_i
   TruthUtils PathResolver CompHep_i MadCUP_i Matchig_i HvGen_i )

atlas_add_component( PythiaLLP_i
   src/PythiaLLP.cxx src/Pydat1.cxx src/Pydatr.cxx src/Pydat2.cxx
   src/Pydat3_62.cxx src/Pyssmt.cxx src/Pysubs.cxx src/Pypars.cxx
   src/Pymssm.cxx src/Pyint1.cxx src/Pyint2.cxx src/Pyint5.cxx
   src/Pytcsm.cxx src/upinit_py.F src/upevnt_py.F src/upveto_py.F
   src/cldcay.F src/gpydat2.F src/gpydatr.F src/gpypars.F src/gpysubs.F
   src/gpydat1.F src/gpydat3_62.F src/gpymssm.F src/gpyssmt.F
   src/gpyint1.F src/gpyint2.F src/gpyint5.F src/gpytcsm.F
   src/initpyblock.F src/openrandom.F src/opdcay.F src/pyr.F src/extproc.F
   src/gatlastaula_decres_py.F src/atopyt.F src/opensusyfile.F
   src/PythiaDummies/pyevwt.F src/PythiaDummies/pykcut.F
   src/PythiaDummies/pytaud.F src/PythiaDummies/pytime.F
   src/PythiaDummies/sugra.F src/PythiaDummies/visaje.F
   src/PythiaDummies/ssmssm.F src/PythiaDummies/fhhiggscorr.F
   src/PythiaDummies/fhsetflags.F src/PythiaDummies/fhsetpara.F
   src/PythiaModified/initpyupda.F src/PythiaLLP_i_entries.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${GSL_LIBRARIES} ${CLHEP_LIBRARIES}
   ${PYTHIA6_LIBRARIES} ${LHAPDF_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel
   AcerMC_i GeneratorModulesLib Pythia_iLib Tauola_iLib AthenaKernel StoreGateLib
   AlpGen_i ExoGraviton_i GeneratorFortranCommonLib GravADD_i MadGraph_i
   TruthUtils PathResolver CompHep_i MadCUP_i Matchig_i HvGen_i )

atlas_add_component( PythiaResMod_i
   src/PythiaResMod.cxx src/atlasTuneResMod.cxx src/setPythiaTuneResMod.cxx
   src/Pydat1.cxx src/Pydatr.cxx src/Pydat2.cxx src/Pydat3_62.cxx
   src/Pyssmt.cxx src/Pysubs.cxx src/Pypars.cxx src/Pymssm.cxx
   src/Pyint1.cxx src/Pyint2.cxx src/Pyint5.cxx src/Pytcsm.cxx
   src/upinit_py.F src/upevnt_py.F src/upveto_py.F src/cldcay.F
   src/gpydat2.F src/gpydatr.F src/gpypars.F src/gpysubs.F src/gpydat1.F
   src/gpydat3_62.F src/gpymssm.F src/gpyssmt.F src/gpyint1.F
   src/gpyint2.F src/gpyint5.F src/gpytcsm.F src/initpyblock.F
   src/openrandom.F src/opdcay.F src/pyr.F src/extproc.F
   src/setecm.F src/gatlastaula_decres_py.F src/atopyt.F
   src/opensusyfile.F src/parout.F src/special_exo.F
   src/PythiaDummies/pyevwt.F src/PythiaDummies/pykcut.F
   src/PythiaDummies/pytaud.F src/PythiaDummies/pytime.F
   src/PythiaDummies/sugra.F src/PythiaDummies/visaje.F
   src/PythiaDummies/ssmssm.F src/PythiaDummies/fhhiggscorr.F
   src/PythiaDummies/fhsetflags.F src/PythiaDummies/fhsetpara.F
   src/PythiaModified/pyrhad.F src/PythiaModified/pysthad.F
   src/PythiaModified/pysgex.F src/PythiaResMod_i_entries.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${GSL_LIBRARIES} ${CLHEP_LIBRARIES}
   ${PYTHIA6_LIBRARIES} ${LHAPDF_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel
   AcerMC_i GeneratorModulesLib Pythia_iLib Tauola_iLib AthenaKernel StoreGateLib
   AlpGen_i ExoGraviton_i GeneratorFortranCommonLib GravADD_i MadGraph_i
   TruthUtils PathResolver CompHep_i MadCUP_i Matchig_i HvGen_i )

atlas_add_component( PythiaTopMdiff_i
   src/PythiaTopMdiff.cxx src/atlasTuneTopMdiff.cxx
   src/setPythiaTuneTopMdiff.cxx src/Pydat1.cxx src/Pydatr.cxx src/Pydat2.cxx
   src/Pydat3_62.cxx src/Pyssmt.cxx src/Pysubs.cxx src/Pypars.cxx
   src/Pymssm.cxx src/Pyint1.cxx src/Pyint2.cxx src/Pyint5.cxx
   src/Pytcsm.cxx src/upinit_py.F src/upevnt_py.F src/upveto_py.F
   src/cldcay.F src/gpydat2.F src/gpydatr.F src/gpypars.F src/gpysubs.F
   src/gpydat1.F src/gpydat3_62.F src/gpymssm.F src/gpyssmt.F src/gpyint1.F
   src/gpyint2.F src/gpyint5.F src/gpytcsm.F src/initpyblock.F src/openrandom.F
   src/opdcay.F src/pyr.F src/extproc.F src/setecm.F src/gatlastaula_decres_py.F
   src/atopyt.F src/opensusyfile.F src/parout.F src/special_exo.F
   src/PythiaDummies/pyevwt.F src/PythiaDummies/pykcut.F
   src/PythiaDummies/pytaud.F src/PythiaDummies/pytime.F
   src/PythiaDummies/sugra.F src/PythiaDummies/visaje.F
   src/PythiaDummies/ssmssm.F src/PythiaDummies/fhhiggscorr.F
   src/PythiaDummies/fhsetflags.F src/PythiaDummies/fhsetpara.F
   src/PythiaModified/pyscat.F src/PythiaModified/pyofsh.F
   src/PythiaTopMdiff_i_entries.cxx 
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${GSL_LIBRARIES} ${CLHEP_LIBRARIES}
   ${PYTHIA6_LIBRARIES} ${LHAPDF_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel
   AcerMC_i GeneratorModulesLib Pythia_iLib Tauola_iLib AthenaKernel StoreGateLib
   AlpGen_i ExoGraviton_i GeneratorFortranCommonLib GravADD_i MadGraph_i
   TruthUtils PathResolver CompHep_i MadCUP_i Matchig_i HvGen_i )

# Install files from the package:
atlas_install_headers( PythiaExo_i )
atlas_install_joboptions( share/*.py )
