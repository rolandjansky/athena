################################################################################
# Package: PythiaB
################################################################################

# Declare the package name:
atlas_subdir( PythiaB )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          GaudiKernel
                          Generators/GeneratorModules
                          Generators/GeneratorObjects
                          Generators/Pythia_i
                          PRIVATE
                          Control/AthenaKernel
                          Generators/GeneratorUtils
                          Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( HepMC COMPONENTS HepMC HepMCfio )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( PythiaBLib
                   src/PythiaB.cxx
                   src/charm.F
                   src/children.F
                   src/qualify.F
                   src/anti_beauty.F
                   src/find_children.F
                   src/anti_charm.F
                   src/finsel.F
                   src/trig1.F
                   src/trig2.F
                   src/bbbb_luexec.F
                   src/swop_ludat3.F
                   src/copy_pydat3.F
                   src/forced.F
                   src/uinit.F
                   src/bbcc_luexec.F
                   src/multi_flav_luexec.F
                   src/user_finsel.F
                   src/bcopy.F
                   src/off_line.F
                   src/beauty.F
                   src/parsel.F
                   src/comons.F
                   src/fill_ntup.F
                   src/ulast.F
                   src/uf_Skeleton.F
                   src/uf_DsPhiX.F
                   src/uf_LambdabJpsimumuLambda.F
                   src/uf_LambdabmumuLambda.F
                   src/uf_BsJpsimumuphiKK.F
                   src/uf_BdJpsimumuK0starKpi.F
                   src/uf_BsAngles.F
                   src/uf_BsAnglesCDF.F
                   src/uf_BdAnglesCDF.F
                   src/uf_bbmumu.F
                   src/uf_KstarMuMu.F
                   src/uf_PhiMuMu.F
                   src/uf_GammaMuMu.F
                   src/uf_DK1.F
                   src/uf_ChHadr.F
                   src/btreedel.F
                   src/coef.F
                   src/comfunc.F
                   src/formfactors_KstMuMu.F
                   src/formfactors_PhiMuMu.F
                   src/formfactors_b2gmm.F
                   src/daughterline.F
                   src/matrix_and_kinematics_KstMuMu.F
                   src/matrix_and_kinematics_PhiMuMu.F
                   src/matrix_and_kinematics_b2gmm.F
                   src/matrix_for_fortran_KstMuMu.F
                   src/matrix_for_fortran_PhiMuMu.F
                   src/matrix_for_fortran_b2gmm.F
                   src/rotat.F
                   PUBLIC_HEADERS PythiaB
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GaudiKernel GeneratorObjects StoreGateLib SGtests GeneratorModulesLib Pythia_iLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel TruthUtils )

atlas_add_component( PythiaB
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} StoreGateLib SGtests GaudiKernel GeneratorModulesLib GeneratorObjects Pythia_iLib AthenaKernel TruthUtils PythiaBLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/Bdecays0.dat )

