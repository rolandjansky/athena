################################################################################
# Package: LArG4Validation
################################################################################

# Declare the package name:
atlas_subdir( LArG4Validation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/GeoModel/GeoAdaptors
                          Generators/GeneratorObjects
                          LArCalorimeter/LArSimEvent
                          MagneticField/MagFieldInterfaces
                          Reconstruction/egamma/egammaEvent )

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
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.xml )
atlas_install_scripts( share/AODElectronContainerReader.py share/LArG4ValidationGenerate.py share/LArG4AODNtuplePlotter.py share/LArG4ValidationPlotter.py )

atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( flake8_share
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --ignore=F401,F821,ATL900 ${CMAKE_CURRENT_SOURCE_DIR}/share
                POST_EXEC_SCRIPT nopost.sh )
