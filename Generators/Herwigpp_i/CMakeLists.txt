# $Id$
################################################################################
# Package: Herwigpp_i
################################################################################

# Declare the package name:
atlas_subdir( Herwigpp_i )

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
atlas_add_component( Herwigpp_i
   Herwigpp_i/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${HERWIG3_INCLUDE_DIRS}
   ${THEPEG_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS -DHWVERSION=\"${HERWIGPP_VERSION}\"
   LINK_LIBRARIES ${Boost_LIBRARIES} ${GSL_LIBRARIES} ${HERWIGPP_LIBRARIES} ${THEPEG_LIBRARIES}
   GeneratorModulesLib EventInfo GaudiKernel PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
