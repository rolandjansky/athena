# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecExCommon )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( data/*.ascii.gz )
atlas_install_scripts( share/RecExCommon_links.sh share/recexcommon-links.py )
