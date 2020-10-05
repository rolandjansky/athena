# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TriggerJobOpts )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( TriggerConfigFlagsTest
   SCRIPT python -m unittest TriggerJobOpts.TriggerConfigFlags
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TriggerMenuFlagsTest
   SCRIPT python -m unittest TriggerJobOpts.MenuConfigFlags
   POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( TriggerConfigTest
   SCRIPT python -m TriggerJobOpts.TriggerConfig
   POST_EXEC_SCRIPT nopost.sh )
