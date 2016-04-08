################################################################################
# Package: CLIDSvc
################################################################################

# Declare the package name:
atlas_subdir( CLIDSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/SGTools
                          PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( PythonLibs )

# Install files from the package:
atlas_install_headers( CLIDSvc )

