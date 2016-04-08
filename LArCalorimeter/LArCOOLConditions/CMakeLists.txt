################################################################################
# Package: LArCOOLConditions
################################################################################

# Declare the package name:
atlas_subdir( LArCOOLConditions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LArCOOLConditions
                   src/*.cxx
                   PUBLIC_HEADERS LArCOOLConditions
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES Identifier GaudiKernel LArIdentifier LArCablingLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel AthenaPoolUtilities )

atlas_add_dictionary( LArCOOLConditionsDict
                      LArCOOLConditions/LArCOOLConditionsDict.h
                      LArCOOLConditions/selection.xml
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} Identifier GaudiKernel LArCablingLib LArIdentifier AthenaKernel StoreGateLib SGtests AthenaPoolUtilities LArCOOLConditions )

