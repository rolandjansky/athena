# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SCT_CalibAlgs )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS MathCore Core Tree Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( SCT_CalibAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib AthenaPoolUtilities Identifier EventInfo GaudiKernel InDetIdentifier SCT_CablingLib InDetRawData InDetReadoutGeometry SCT_ReadoutGeometry PathResolver InDetConditionsSummaryService SCT_ConditionsToolsLib RegistrationServicesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

