# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ITkConditionsUtils )

# Install files from the package:
atlas_install_scripts( scripts/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
