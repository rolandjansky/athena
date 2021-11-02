# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SCT_CalibAlgs )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS MathCore Core Tree Hist RIO pthread )
find_package( Boost )

# Component(s) in the package:
atlas_add_component( SCT_CalibAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib AthenaPoolUtilities Identifier EventInfo GaudiKernel InDetIdentifier SCT_CablingLib InDetRawData InDetReadoutGeometry SCT_ReadoutGeometry PathResolver InDetConditionsSummaryService SCT_ConditionsToolsLib RegistrationServicesLib )

atlas_add_test( SCT_CalibUtilities_test
   SOURCES test/SCT_CalibUtilities_test.cxx src/SCT_CalibUtilities.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} EventInfo InDetIdentifier
   POST_EXEC_SCRIPT "nopost.sh"
)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

