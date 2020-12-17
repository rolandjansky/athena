# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecJobTransforms )

# Install python modules
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
# Install RDOtoRDOtrigger job opts with flake8 check
atlas_install_joboptions( share/skeleton.RDOtoRDOtrigger*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
# Install other job opts without flake8 check
atlas_install_joboptions( share/*.py EXCLUDE share/*RDOtoRDOtrigger*.py )
# Install scripts
atlas_install_runtime( scripts/*.py )

