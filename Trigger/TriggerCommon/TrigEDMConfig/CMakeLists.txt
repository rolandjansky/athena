# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigEDMConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )

atlas_add_test( testEDM
   SCRIPT python/testEDM.py
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( testEDMRun3
   SCRIPT python/testEDMRun3.py
   POST_EXEC_SCRIPT nopost.sh )
