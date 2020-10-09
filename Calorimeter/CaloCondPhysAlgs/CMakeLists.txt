# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloCondPhysAlgs )

# External dependencies:
find_package( CLHEP )
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_component( CaloCondPhysAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${COOL_LIBRARIES} ${CLHEP_LIBRARIES} CaloCondBlobObjs CaloDetDescrLib CaloGeoHelpers 
		             CaloIdentifier AthenaBaseComps AthenaKernel StoreGateLib GaudiKernel LArCablingLib LArElecCalib LArIdentifier TrigDecisionToolLib LArRawConditions
		             CaloConditions CaloEvent CaloInterfaceLib CaloUtilsLib AthenaPoolUtilities Identifier xAODEventInfo LArHV LArReadoutGeometry LArSimEvent CxxUtils)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
atlas_install_scripts( share/CaloRescaleNoiseHV.sh share/CaloNoise_fillDB.py share/CaloPedestalShift.sh share/CaloPedestal_fillDB.py share/CaloScaleNoise_jobOptions.py )
