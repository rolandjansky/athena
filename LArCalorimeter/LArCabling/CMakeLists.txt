################################################################################
# Package: LArCabling
################################################################################

# Declare the package name:
atlas_subdir( LArCabling )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloDetDescr
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          PRIVATE
                          Tools/PathResolver )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( LArCablingLib
                   src/*.cxx
                   PUBLIC_HEADERS LArCabling
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloIdentifier AthenaBaseComps AthenaKernel AthenaPoolUtilities Identifier GaudiKernel LArIdentifier StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} PathResolver )

atlas_add_component( LArCabling
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel LArIdentifier PathResolver LArCablingLib )

atlas_add_dictionary( LArCablingDict
                      LArCabling/LArCablingDict.h
                      LArCabling/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities Identifier GaudiKernel LArIdentifier PathResolver LArCablingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/FEBtoRODfred_v10.data share/HVLineToDetector.data share/HVLineToElectrode.data )

