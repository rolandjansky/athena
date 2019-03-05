################################################################################
# Package: PyJobTransforms
################################################################################

# Declare the package name:
atlas_subdir( PyJobTransforms )

# External dependencies:
find_package( PythonLibs )
find_package( cx_Oracle )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( scripts/*.py )
atlas_install_runtime( test/PyJobTransforms_TestConfiguration.xml )
atlas_install_generic( share/*.db
                       DESTINATION share
                       EXECUTABLE )
atlas_install_generic( test/test_*.py
                       DESTINATION share/JobTransforms/test
                       EXECUTABLE )

