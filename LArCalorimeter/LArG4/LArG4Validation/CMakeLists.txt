################################################################################
# Package: LArG4Validation
################################################################################

# Declare the package name:
atlas_subdir( LArG4Validation )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( LArG4Validation
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel CaloDetDescrLib CaloIdentifier AthenaBaseComps StoreGateLib SGtests GeoAdaptors GeneratorObjects LArSimEvent MagFieldInterfaces egammaEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
atlas_install_runtime( test/*.xml )
atlas_install_scripts( share/AODElectronContainerReader.py share/LArG4ValidationGenerate.py share/LArG4AODNtuplePlotter.py share/LArG4ValidationPlotter.py )
