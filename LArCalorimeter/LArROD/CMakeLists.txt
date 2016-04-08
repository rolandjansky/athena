################################################################################
# Package: LArROD
################################################################################

# Declare the package name:
atlas_subdir( LArROD )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawConditions
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRawUtils
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArRecUtils
                          TestBeam/TBEvent
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Commission/CommissionEvent
                          Control/AthContainers
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          LArCalorimeter/LArCOOLConditions )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LArRODLib
                   src/*.cxx
                   src/tests/*.cxx
                   PUBLIC_HEADERS LArROD
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloIdentifier AthenaBaseComps Identifier GaudiKernel LArIdentifier LArRawConditions LArRawEvent LArRecConditions TBEvent StoreGateLib SGtests LArCablingLib LArRawUtilsLib LArRecUtilsLib CaloDetDescrLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CommissionEvent AthContainers AthenaPoolUtilities EventInfo LArCOOLConditions )

atlas_add_component( LArROD
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel LArCablingLib LArIdentifier LArRawConditions LArRawEvent LArRawUtilsLib LArRecConditions LArRecUtilsLib TBEvent CaloDetDescrLib CaloEvent CommissionEvent AthContainers AthenaPoolUtilities EventInfo LArCOOLConditions LArRODLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

