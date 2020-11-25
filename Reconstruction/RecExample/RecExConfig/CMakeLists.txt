# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecExConfig )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/tests/*.py )

# Test(s) in the package
atlas_add_test( testObjKeyStore
   SCRIPT python -m RecExConfig.testObjKeyStore
   POST_EXEC_SCRIPT nopost.sh )
