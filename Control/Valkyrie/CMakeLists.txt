################################################################################
# Package: Valkyrie
################################################################################

# Declare the package name:
atlas_subdir( Valkyrie )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaCommon
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( valgrind )

# Component(s) in the package:
atlas_add_component( Valkyrie
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${VALGRIND_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${VALGRIND_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/valkyrie.py share/callgrind.py )

