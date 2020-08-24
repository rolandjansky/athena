# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Package is now a rump of a few Python libraries kept around for potential 
# Run 1 compatibility

# Declare the package name:
atlas_subdir( DetectorStatus )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
