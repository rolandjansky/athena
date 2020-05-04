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
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArGeoModel/LArHV
                          LArCalorimeter/LArGeoModel/LArReadoutGeometry
                          LArCalorimeter/LArSimEvent 
			  Control/CxxUtils)

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
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} ${CLHEP_LIBRARIES} CaloCondBlobObjs CaloDetDescrLib CaloGeoHelpers 
		     CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel LArCablingLib LArIdentifier TrigDecisionToolLib 
		     CaloConditions CaloEvent CaloUtilsLib AthenaPoolUtilities Identifier xAODEventInfo LArHV LArReadoutGeometry LArSimEvent CxxUtils)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/CaloRescaleNoiseHV.sh share/CaloNoise_fillDB.py share/CaloPedestalShift.sh share/CaloPedestal_fillDB.py share/CaloScaleNoise_jobOptions.py )

atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( flake8_share
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 --ignore=F401,F821,ATL900,ATL901 ${CMAKE_CURRENT_SOURCE_DIR}/share
                POST_EXEC_SCRIPT nopost.sh )
