################################################################################
# Package: LArCellRec
################################################################################

# Declare the package name:
atlas_subdir( LArCellRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloRec
                          Calorimeter/CaloUtils
                          Control/AthAllocators
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArSimEvent
                          Event/xAOD/xAODTrigL1Calo
                          PRIVATE
                          Calorimeter/CaloTriggerTool
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          LArCalorimeter/LArRecEvent
                          LArCalorimeter/LArCabling
                          Trigger/TrigT1/TrigT1CaloCalibConditions
                          Trigger/TrigT1/TrigT1CaloCondSvc )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( LArCellRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} CaloConditions CaloDetDescrLib CaloEvent CaloIdentifier CaloRecLib CaloUtilsLib AthAllocators AthenaBaseComps AthenaKernel StoreGateLib SGtests AtlasDetDescr Identifier GaudiKernel LArIdentifier LArRawEvent LArRecConditions LArSimEvent CaloTriggerToolLib AthenaPoolUtilities xAODEventInfo xAODTrigL1Calo GeneratorObjects LArRecEvent LArCablingLib TrigT1CaloCalibConditions TrigT1CaloCondSvcLib )

# Install files from the package:
atlas_install_headers( LArCellRec )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

