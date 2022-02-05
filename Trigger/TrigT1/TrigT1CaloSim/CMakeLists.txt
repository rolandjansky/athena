# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloSim )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigT1CaloSim
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     DEFINITIONS ${CLHEP_DEFINITIONS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers AthenaBaseComps AthenaKernel CaloIdentifier GaudiKernel Identifier LArRawEvent PathResolver StoreGateLib TileConditionsLib TileEvent TrigConfData TrigConfL1Data TrigT1CaloByteStreamLib TrigT1CaloCalibConditions TrigT1CaloCondSvcLib TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1CaloUtilsLib TrigT1Interfaces xAODEventInfo xAODTrigL1Calo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Tests:
atlas_add_test( TrigT1CaloSimRun2Config
   SCRIPT python -m TrigT1CaloSim.TrigT1CaloSimRun2Config
   POST_EXEC_SCRIPT nopost.sh )
