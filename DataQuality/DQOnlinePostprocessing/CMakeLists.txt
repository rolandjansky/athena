# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DQOnlinePostprocessing )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist Tree )
find_package( tdaq COMPONENTS ispy )

# Install files from the package:
atlas_install_python_modules( python/*.py
   POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select E11,E402,W1 )
