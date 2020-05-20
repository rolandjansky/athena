################################################################################
# Package: InDetRecExample
################################################################################

# Declare the package name:
atlas_subdir( InDetRecExample )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaCommon
                          Database/RDBAccessSvc )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/jobOptions.py )

