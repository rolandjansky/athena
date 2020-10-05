# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PerfMonAna )

# External dependencies:
find_package( pandas )
find_package( sqlalchemy )
find_package( ROOT COMPONENTS Core PyROOT Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( bin/*.py )

# Aliases:
atlas_add_alias( perfgrind "perfgrind.py" )
atlas_add_alias( pmontree "pmontree.py" )
atlas_add_alias( perfrtt "perfrtt.py" )
atlas_add_alias( perfmon "perfmon.py" )
atlas_add_alias( perf-dpmon "perf-dpmon.py" )
