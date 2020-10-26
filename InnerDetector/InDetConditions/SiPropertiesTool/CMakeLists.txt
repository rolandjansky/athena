# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiPropertiesTool )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( SiPropertiesToolLib
                   src/*.cxx
                   PUBLIC_HEADERS SiPropertiesTool
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaKernel GaudiKernel Identifier InDetConditionsSummaryService InDetReadoutGeometry PixelConditionsData SCT_ConditionsData
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps InDetIdentifier StoreGateLib )

atlas_add_component( SiPropertiesTool
                     src/components/*.cxx
                     LINK_LIBRARIES SiPropertiesToolLib )

# Run tests:
atlas_add_test( TestSCTProperties
                SCRIPT athena.py --threads=5 SiPropertiesTool/testSCTProperties.py
                PROPERTIES TIMEOUT 300
                ENVIRONMENT THREADS=5 )

atlas_add_test( TestSCTPropertiesNewConf
                SCRIPT python -m SiPropertiesTool.SCTSiPropertiesTestAlgConfig
                PROPERTIES TIMEOUT 300 )

atlas_add_test( SiPropertiesConfig_test
                SCRIPT test/SiPropertiesConfig_test.py
                PROPERTIES TIMEOUT 300 )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
