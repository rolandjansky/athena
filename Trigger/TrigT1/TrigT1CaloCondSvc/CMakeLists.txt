# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloCondSvc )

# Component(s) in the package:
atlas_add_library( TrigT1CaloCondSvcLib
   TrigT1CaloCondSvc/*.h TrigT1CaloCondSvc/*.icc src/*.cxx 
   PUBLIC_HEADERS TrigT1CaloCondSvc
   LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaPoolUtilities RegistrationServicesLib StoreGateLib TrigT1CaloCalibConditions xAODTrigL1Calo CoralUtilitiesLib nlohmann_json::nlohmann_json )

atlas_add_component( TrigT1CaloCondSvc
   src/components/*.cxx
   LINK_LIBRARIES LINK_LIBRARIES TrigT1CaloCondSvcLib  nlohmann_json::nlohmann_json)

# Install files from the package: 
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
