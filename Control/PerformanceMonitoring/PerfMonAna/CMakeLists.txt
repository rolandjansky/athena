# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PerfMonAna )

# External dependencies:
find_package( matplotlib )
find_package( numpy )
find_package( pandas )
find_package( sqlalchemy )
find_package( ROOT COMPONENTS Core PyROOT Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( bin/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Aliases:
atlas_add_alias( perfgrind "perfgrind.py" )
atlas_add_alias( pmontree "pmontree.py" )
atlas_add_alias( perfmon "perfmon.py" )
atlas_add_alias( perf-dpmon "perf-dpmon.py" )
