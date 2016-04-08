################################################################################
# Package: GaudiSequencer
################################################################################

# Declare the package name:
atlas_subdir( GaudiSequencer )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          GaudiKernel )

# External dependencies:
find_package( valgrind )

# Component(s) in the package:
atlas_add_component( GaudiSequencer
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${VALGRIND_INCLUDE_DIRS}
                     LINK_LIBRARIES ${VALGRIND_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils GaudiKernel )

# Install files from the package:
atlas_install_headers( GaudiSequencer )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

