################################################################################
# Package: PMGSystematicsTools
################################################################################

# Declare the package name:
atlas_subdir( PMGSystematicsTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tools/PyCmt )

# External dependencies:
find_package( PythonLibs )
find_package( ROOT COMPONENTS Core PyROOT Tree MathCore Hist RIO pthread )
find_package( Rivet )
find_package( YODA )



# Install files from the package:
atlas_install_python_modules( local/bin/*.py )
atlas_install_data( data/*.yaml )
atlas_install_scripts( local/bin/*.py )

# Aliases:
atlas_add_alias( PMG_systematicsTool "systematicsTool.py" )
atlas_add_alias( PMG_submissionTool "submissionTool.py" )
