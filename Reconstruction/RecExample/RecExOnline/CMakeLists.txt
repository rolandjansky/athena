################################################################################
# Package: RecExOnline
################################################################################

# Declare the package name:
atlas_subdir( RecExOnline )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/RecExOnline_TestConfiguration.xml share/RootHis*.C share/RecExOnline_Test*.C )
atlas_install_scripts( scripts/*.sh )

