################################################################################
# Package: LArOnlDbPrep
################################################################################

# Declare the package name:
atlas_subdir( LArOnlDbPrep )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTriggerTool
                          Control/AthenaBaseComps
                          GaudiKernel
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArCabling
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloInterface
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          LArCalorimeter/LArCOOLConditions
                          LArCalorimeter/LArIdentifier )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( LArOnlDbPrep
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CaloIdentifier CaloTriggerToolLib AthenaBaseComps GaudiKernel LArRecConditions LArCablingLib CaloDetDescrLib StoreGateLib SGtests AthenaPoolUtilities LArCOOLConditions LArIdentifier CaloInterfaceLib )

# Install files from the package:
atlas_install_headers( LArOnlDbPrep )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/*.sh )

