# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
atlas_subdir( WorkflowTestRunner )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( scripts/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
