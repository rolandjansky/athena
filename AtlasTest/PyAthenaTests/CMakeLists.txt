################################################################################
# Package: PyAthenaTests
################################################################################

# Declare the package name:
atlas_subdir( PyAthenaTests )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaExamples/AthExThinning
                          Control/AthenaPython
                          PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.ref )
atlas_install_scripts( test/*.py )

