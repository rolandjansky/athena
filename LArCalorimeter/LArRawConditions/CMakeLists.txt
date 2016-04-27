################################################################################
# Package: LArRawConditions
################################################################################

# Declare the package name:
atlas_subdir( LArRawConditions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LArRawConditions
                   src/*.cxx
                   PUBLIC_HEADERS LArRawConditions
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloIdentifier AthenaKernel DataModel SGTools AthenaPoolUtilities Identifier GaudiKernel LArIdentifier StoreGateLib SGtests LArToolsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( LArRawConditions1Dict
                      LArRawConditions/LArRawConditionsDict1.h
                      LArRawConditions/selection1.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier AthenaKernel DataModel SGTools StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel LArIdentifier LArToolsLib LArRawConditions )

atlas_add_dictionary( LArRawConditions2Dict
                      LArRawConditions/LArRawConditionsDict2.h
                      LArRawConditions/selection2.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier AthenaKernel DataModel SGTools StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel LArIdentifier LArToolsLib LArRawConditions )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

