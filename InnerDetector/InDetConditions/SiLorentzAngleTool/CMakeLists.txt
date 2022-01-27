# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiLorentzAngleTool )

# Component(s) in the package:
atlas_add_component( SiLorentzAngleTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib GeoPrimitives GaudiKernel SiPropertiesToolLib AthenaPoolUtilities Identifier InDetCondTools InDetIdentifier InDetReadoutGeometry PixelConditionsData PixelReadoutGeometryLib MagFieldElements MagFieldConditions InDetConditionsSummaryService SCT_ConditionsData )

# Run tests:
atlas_add_test( TestSCTLorentzAngle
                SCRIPT athena.py --threads=5 SiLorentzAngleTool/testSCTLorentzAngle.py 
                POST_EXEC_SCRIPT nopost.sh
                ENVIRONMENT THREADS=5 )

atlas_add_test( TestSCTLorentzAngleNewConf
                SCRIPT python -m SiLorentzAngleTool.SCTSiLorentzAngleTestAlgConfig
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( SiLorentzAngleConfig_test
                SCRIPT test/SiLorentzAngleConfig_test.py
                POST_EXEC_SCRIPT nopost.sh )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
