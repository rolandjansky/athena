################################################################################
# Package: LArAffectedRegion
################################################################################

# Declare the package name:
atlas_subdir( LArAffectedRegion )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloGeoHelpers
                          Database/IOVDbMetaDataTools
                          Event/EventInfo
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArGeoModel/LArHV
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArTools )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( LArAffectedRegion
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} CaloConditions CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities GaudiKernel LArIdentifier CaloDetDescrLib CaloGeoHelpers EventInfo LArHV LArRecConditions LArToolsLib )

# Install files from the package:
atlas_install_headers( LArAffectedRegion )
atlas_install_joboptions( share/*.py )

