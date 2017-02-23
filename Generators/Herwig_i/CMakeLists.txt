################################################################################
# Package: Herwig_i
################################################################################

# Declare the package name:
atlas_subdir( Herwig_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Generators/AcerMC_i
   Generators/GeneratorModules
   Generators/Tauola_i
   PRIVATE
   Control/AthenaKernel
   GaudiKernel
   Generators/AlpGen_i
   Generators/Charybdis_i
   Generators/GeneratorFortranCommon
   Generators/GeneratorUtils
   Generators/Lhef_i
   Generators/MadGraph_i
   Generators/McAtNlo_i
   Generators/TruthUtils
   Generators/MadCUP_i
   Generators/Horace_i )

# External dependencies:
find_package( CLHEP )
find_package( HepMC COMPONENTS HepMC HepMCfio )
find_package( Herwig )
find_package( Jimmy )
find_package( Lhapdf )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Herwig_iLib
   Herwig_i/*.h Herwig_i/*.icc
   src/Herwig.cxx src/Address*.cxx src/InitHerwigCommonBlocks_65.cxx
   src/Lhefinfo.cxx src/wgtacp.cxx src/upevnt_hw.F
   src/upinit_hw.F src/upveto_hw.F src/atoher_65.F
   src/hwpdfinfo.F src/HerwigInterface_65.F src/herwig_common_block_address_65.F
   src/glhefinfo.F src/gwgtacp.F src/hwgpdg.F src/hwtpdg.F src/hwrgen.F
   src/gatlastaula_decres_hw.F src/hwhepc.F src/extproc.F src/topdec.F
   src/charybdfix.F src/HerwigDummies/circ*.F src/HerwigDummies/decadd.F
   src/HerwigDummies/eudini.F src/HerwigDummies/fragmt.F
   src/HerwigDummies/hvhbvi.F src/HerwigDummies/ieupdg.F
   src/HerwigDummies/ipdgeu.F src/HerwigDummies/qqinit.F
   src/HerwigDummies/hwhvvj.F src/HerwigDummies/hwupro.F
   src/HerwigDummies/pytime.F src/HerwigDummies/hwirpv.F
   src/HerwigDummies/timel.F src/HerwigModified/hwugup.f
   src/HerwigModified/hwhsnm.f src/HerwigModified/hwdtau.f
   PUBLIC_HEADERS Herwig_i
   INCLUDE_DIRS ${HERWIG_INCLUDE_DIRS} ${JIMMY_INCLUDE_DIRS}
   ${LHAPDF_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS} -DHWVERSION="${HERWIG_VERSION}"
   LINK_LIBRARIES ${HERWIG_LIBRARIES} ${JIMMY_LIBRARIES} ${LHAPDF_LIBRARIES}
   AcerMC_i GeneratorModulesLib Tauola_iLib GeneratorFortranCommonLib
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel
   GaudiKernel AlpGen_i Charybdis_i Lhef_i MadGraph_i McAtNlo_i TruthUtils
   MadCUP_i Horace_i )

atlas_add_component( Herwig_i
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel Herwig_iLib )

atlas_add_component( HerwigRpv_i
   src/HerwigModified/rpv/*.f
   src/componentsRpv/*.cxx
   LINK_LIBRARIES GaudiKernel Herwig_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
