# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetAlignExample )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py share/InDetAlignExampleFlags/*.py share/Data/*.txt )
atlas_install_runtime( macros/*.cc html/*.html )
atlas_install_scripts( share/RunIterator_Run2Rel19.py )

