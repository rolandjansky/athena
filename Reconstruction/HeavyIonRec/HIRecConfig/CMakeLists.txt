# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HIRecConfig )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( HIRec_test
   SCRIPT python -m HIRecConfig.HIRecConfig
   POST_EXEC_SCRIPT nopost.sh )
