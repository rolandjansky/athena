################################################################################
# Package: Hephaestus
################################################################################

# Declare the package name:
atlas_subdir( Hephaestus )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( PythonLibs )
find_package( libunwind )

# Component(s) in the package:
atlas_add_library( Hephaestus
                   src/hlib/*.c
                   PUBLIC_HEADERS Hephaestus
                   INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} ${LIBUNWIND_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${PYTHON_LIBRARIES} ${LIBUNWIND_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} TestTools )

atlas_add_executable( hephprof
                      src/HephProf/HephProf.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${LIBUNWIND_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${LIBUNWIND_LIBRARIES} TestTools Hephaestus )

atlas_add_test( stackstash_test
                SOURCES
                test/stackstash_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${LIBUNWIND_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${LIBUNWIND_LIBRARIES} TestTools Hephaestus )

# Install files from the package:
atlas_install_python_modules( python/*.py )

