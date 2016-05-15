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
                          Control/CxxUtils
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArSimEvent
                          PRIVATE
                          Calorimeter/CaloTriggerTool
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTrigL1Calo
                          Generators/GeneratorObjects
                          LArCalorimeter/LArRecEvent
                          LArCalorimeter/LArTools
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
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} CaloConditions CaloDetDescrLib CaloEvent CaloIdentifier CaloRecLib CaloUtilsLib AthAllocators AthenaBaseComps AthenaKernel CxxUtils StoreGateLib SGtests AtlasDetDescr Identifier GaudiKernel LArIdentifier LArRawEvent LArRecConditions LArSimEvent CaloTriggerToolLib AthenaPoolUtilities xAODEventInfo xAODTrigL1Calo GeneratorObjects LArRecEvent LArToolsLib TrigT1CaloCalibConditions )

# Install files from the package:
atlas_install_headers( LArCellRec )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

