################################################################################
# Package: CaloTests
################################################################################

# Declare the package name:
atlas_subdir( CaloTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArElecCalib
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloSimEvent
                          Control/AthenaKernel
                          DetectorDescription/AtlasDetDescr
                          Generators/AtlasHepMC
                          Generators/GeneratorObjects
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRawUtils
                          LArCalorimeter/LArSimEvent
                          LArCalorimeter/LArCabling )

# External dependencies:
find_package( AIDA )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( CaloTests
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib CaloDetDescrLib CaloIdentifier AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel CaloEvent CaloGeoHelpers CaloSimEvent AthenaKernel AtlasDetDescr GeneratorObjects LArIdentifier LArRawEvent LArRawUtilsLib LArSimEvent LArCablingLib )

# Install files from the package:
atlas_install_headers( CaloTests )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/CaloTests_TestConfiguration.xml share/*.C share/rttTest.css share/CaloTests_HistoComparison.txt share/*Checks.py )

