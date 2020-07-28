# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloCalibUtils )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO )

# Component(s) in the package:
atlas_add_library( TrigT1CaloCalibUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1CaloCalibUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities CaloIdentifier CaloTriggerToolLib CxxUtils EventInfo GaudiKernel Identifier LArCablingLib LArElecCalib RegistrationServicesLib StoreGateLib TrigConfHLTData TrigConfL1Data TrigT1CaloCalibConditions TrigT1CaloCalibToolInterfaces TrigT1CaloCalibToolsLib TrigT1CaloCondSvcLib TrigT1CaloEventLib TrigT1CaloMonitoringToolsLib TrigT1CaloUtilsLib xAODTrigL1Calo
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CaloDetDescrLib CaloEvent TrigT1CaloToolInterfaces TrigT1Interfaces xAODEventInfo )

atlas_add_component( TrigT1CaloCalibUtils
                     src/components/*.cxx
                     LINK_LIBRARIES TrigT1CaloCalibUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.txt share/*.py )

