################################################################################
# Package: PerfMonAna
################################################################################

# Declare the package name:
atlas_subdir( PerfMonAna )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/PerformanceMonitoring/PerfMonEvent )

# External dependencies:
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

