# $Id$
################################################################################
# Package: Herwig7_i
################################################################################

# Declare the package name:
atlas_subdir( Herwig7_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Generators/GeneratorModules
   PRIVATE
   Event/EventInfo
   GaudiKernel
   Tools/PathResolver )

# External dependencies:
find_package( Boost )
find_package( Herwig3 )
find_package( ThePEG )
find_package( GSL )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_component( Herwig7_i
   Herwig7_i/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HERWIG3_INCLUDE_DIRS}
   ${THEPEG_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS -DHWVERSION=\"${HERWIG7_VERSION}\"
   LINK_LIBRARIES ${HERWIG3_ROOT}/lib/Herwig/libHerwigAPI.so  ${Boost_LIBRARIES} ${HERWIG7_LIBRARIES} ${THEPEG_LIBRARIES} ${GSL_LIBRARIES}
   GeneratorModulesLib EventInfo GaudiKernel PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/common/*.py )

set( Herwig7Environment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of HerwigppEnvironment.cmake" )
find_package( Herwig7Environment )
