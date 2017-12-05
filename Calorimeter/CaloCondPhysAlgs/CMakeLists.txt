################################################################################
# Package: CaloCondPhysAlgs
################################################################################

# Declare the package name:
atlas_subdir( CaloCondPhysAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloCondBlobObjs
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRecUtils
                          LArCalorimeter/LArTools
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Calorimeter/CaloConditions
                          Calorimeter/CaloEvent
                          Calorimeter/CaloUtils
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/Identifier
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArGeoModel/LArHV
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          LArCalorimeter/LArSimEvent )

# External dependencies:
find_package( CLHEP )
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( CaloCondPhysAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} ${CLHEP_LIBRARIES} CaloCondBlobObjs CaloDetDescrLib CaloGeoHelpers CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel LArCablingLib LArIdentifier LArRecUtilsLib TrigDecisionToolLib CaloConditions CaloEvent CaloUtilsLib AthenaPoolUtilities Identifier EventInfo xAODEventInfo LArHV LArReadoutGeometry LArSimEvent )

# Install files from the package:
atlas_install_headers( CaloCondPhysAlgs )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/CaloRescaleNoiseHV.sh share/CaloNoise_fillDB.py share/CaloPedestalShift.sh share/CaloPedestal_fillDB.py share/CaloScaleNoise_jobOptions.py )

