# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEDMConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )

atlas_add_test( UnitTest SCRIPT python/testEDM.py )
atlas_add_test( UnitTestRun3 SCRIPT python/testEDMRun3.py )
