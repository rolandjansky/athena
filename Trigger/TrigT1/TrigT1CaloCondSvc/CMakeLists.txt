# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CaloCondSvc )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_library( TrigT1CaloCondSvcLib
   TrigT1CaloCondSvc/*.h TrigT1CaloCondSvc/*.icc src/*.cxx 
   PUBLIC_HEADERS TrigT1CaloCondSvc
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities CxxUtils RegistrationServicesLib StoreGateLib TrigT1CaloCalibConditions xAODTrigL1Calo )

atlas_add_component( TrigT1CaloCondSvc
   src/components/*.cxx
   LINK_LIBRARIES TrigT1CaloCondSvcLib )

# Install files from the package: 
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
