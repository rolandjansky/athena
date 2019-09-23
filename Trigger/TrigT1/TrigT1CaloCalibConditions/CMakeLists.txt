################################################################################
# Package: TrigT1CaloCalibConditions
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloCalibConditions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthContainers
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PRIVATE )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigT1CaloCalibConditions
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1CaloCalibConditions
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} AthContainers AthenaPoolUtilities GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( TrigT1CaloCalibConditionsDict
                      TrigT1CaloCalibConditions/TrigT1CaloCalibConditionsDict.h
                      TrigT1CaloCalibConditions/selection.xml
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthenaPoolUtilities GaudiKernel TrigT1CaloCalibConditions )

# Install files from the package:
atlas_install_joboptions( share/*.py )

