################################################################################
# Package: TestTools
################################################################################

# Declare the package name:
atlas_subdir( TestTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaCommon
                          TestPolicy
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( PythonLibs )

# Component(s) in the package:
atlas_add_library( TestTools
                   src/*.cxx
                   PUBLIC_HEADERS TestTools
                   INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${PYTHON_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/runUnitTests.sh share/post.sh )

