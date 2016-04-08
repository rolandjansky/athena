################################################################################
# Package: PATJobTransforms
################################################################################

# Declare the package name:
atlas_subdir( PATJobTransforms )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Reconstruction/RecJobTransforms
                          Tools/PyJobTransforms
                          Tools/PyJobTransformsCore )

# External dependencies:
find_package( PythonLibs )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( scripts/*.py )

# Aliases:
atlas_add_alias( Reco_trf "Reco_trf.py" )
atlas_add_alias( Merging_trf "Merging_trf.py" )

