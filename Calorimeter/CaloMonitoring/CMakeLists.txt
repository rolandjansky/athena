################################################################################
# Package: CaloMonitoring
################################################################################

# Declare the package name:
atlas_subdir( CaloMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist MathCore Gpad )

# Component(s) in the package:
atlas_add_component( CaloMonitoring
   CaloMonitoring/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloIdentifier CommissionEvent
   AthenaMonitoringLib StoreGateLib Identifier xAODCaloEvent GaudiKernel
   LArIdentifier LArRecConditions LArCablingLib MissingETEvent egammaEvent
   ElectronPhotonSelectorToolsLib LWHists TrigDecisionToolLib CaloDetDescrLib
   CaloGeoHelpers AthenaKernel xAODEventInfo LArRecEvent JetEvent
   RecBackgroundEvent CaloInterfaceLib )

# Install files from the package:
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Tests:
atlas_add_test( flake8_rootMacros
                SCRIPT ${ATLAS_FLAKE8} ${CMAKE_CURRENT_SOURCE_DIR}/rootMacros
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TileCaloCellMonAlg_test
                SCRIPT python -m CaloMonitoring.TileCalCellMonAlg
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)
