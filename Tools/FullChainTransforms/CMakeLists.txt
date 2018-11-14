################################################################################
# Package: FullChainTransforms
################################################################################

# Declare the package name:
atlas_subdir( FullChainTransforms )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PhysicsAnalysis/PATJobTransforms
                          Reconstruction/RecJobTransforms
                          Simulation/SimuJobTransforms
                          Tools/PyJobTransforms )

# External dependencies:
find_package( PythonLibs )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( scripts/*.py )

