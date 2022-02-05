# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( StripDigitization )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( StripDigitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib Identifier GaudiKernel SiDigitization InDetCondTools InDetRawData InDetSimEvent HitManagement GeneratorObjects SiPropertiesToolLib InDetIdentifier InDetReadoutGeometry SCT_ReadoutGeometry InDetSimData InDetConditionsSummaryService PathResolver SCT_ConditionsToolsLib SCT_ModuleDistortionsLib MagFieldConditions StoreGateLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

