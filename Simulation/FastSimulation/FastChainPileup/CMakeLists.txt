################################################################################
# Package: FastChainPileup
################################################################################

# Declare the package name:
atlas_subdir( FastChainPileup )

# External dependencies:
find_package( CLHEP )
find_package( Lhapdf )
find_package( Pythia8 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( FastChainPileup
                     src/MultiPy8Pileup.cxx
                     src/MultiParticleGunPileup.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${PYTHIA8_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${LHAPDF_LIBRARIES} ${CLHEP_LIBRARIES} ${PYTHIA8_LIBRARIES} GaudiKernel GeneratorModulesLib Pythia8_iLib )

# Install files from the package:
atlas_install_headers( FastChainPileup )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/sim_reg_test_fastchain.py )
atlas_install_python_modules( python/*.py )

