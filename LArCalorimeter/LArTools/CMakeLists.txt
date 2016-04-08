################################################################################
# Package: LArTools
################################################################################

# Declare the package name:
atlas_subdir( LArTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArElecCalib
                          PRIVATE
                          Calorimeter/CaloIdentifier
                          Database/AthenaPOOL/AthenaPoolUtilities
                          LArCalorimeter/LArRecConditions )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( LArToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS LArTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel Identifier GaudiKernel LArCablingLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} CaloIdentifier AthenaPoolUtilities LArRecConditions )

atlas_add_component( LArTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel Identifier GaudiKernel LArCablingLib CaloIdentifier AthenaPoolUtilities LArRecConditions LArToolsLib )

atlas_add_dictionary( LArToolsDict
                      LArTools/LArToolsDict.h
                      LArTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel Identifier GaudiKernel LArCablingLib CaloIdentifier AthenaPoolUtilities LArRecConditions LArToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

