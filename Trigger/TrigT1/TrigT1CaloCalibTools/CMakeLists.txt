# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloCalibTools )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Graf Gpad Core Hist MathCore Tree )

# Component(s) in the package:
atlas_add_library( TrigT1CaloCalibToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1CaloCalibTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaBaseComps AthenaKernel AthenaPoolUtilities CaloDetDescrLib CaloEvent CaloIdentifier CaloTriggerToolLib CxxUtils DerivationFrameworkInterfaces GaudiKernel Identifier LArCablingLib LArElecCalib LArIdentifier LArRawEvent LArRecConditions StoreGateLib TileConditionsLib TileEvent TrigT1CaloCalibConditions TrigT1CaloCalibToolInterfaces TrigT1CaloCondSvcLib TrigT1CaloEventLib TrigT1CaloMonitoringToolsLib TrigT1CaloToolInterfaces TrigT1CaloUtilsLib TrigT1Interfaces xAODTrigL1Calo
                   PRIVATE_LINK_LIBRARIES AthenaMonitoringLib EventInfo LWHists )

atlas_add_component( TrigT1CaloCalibTools
                     src/components/*.cxx
                     LINK_LIBRARIES TrigT1CaloCalibToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

