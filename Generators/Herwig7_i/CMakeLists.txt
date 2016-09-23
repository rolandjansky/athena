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

# Set the version of ThePEG to use:
set( THEPEG_VERSION 2.0.3 )
set( THEPEG_ROOT
   ${LCG_RELEASE_DIR}/MCGenerators/thepeg/${THEPEG_VERSION}/${ATLAS_PLATFORM} )

# Set the version of Herwigpp to use:
set( HERWIG3_VERSION 7.0.3 )
set( HERWIG3_ROOT
   ${LCG_RELEASE_DIR}/MCGenerators/herwig++/${HERWIG3_VERSION}/${ATLAS_PLATFORM} )

# External dependencies:
find_package( Boost )
find_package( Herwig3 )
find_package( ThePEG )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_component( Herwig7_i
   Herwig7_i/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HERWIG3_INCLUDE_DIRS}
   ${THEPEG_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS -DHWVERSION=\"${HERWIG7_VERSION}\"
   LINK_LIBRARIES ${Boost_LIBRARIES} ${HERWIG7_LIBRARIES} ${THEPEG_LIBRARIES}
   GeneratorModulesLib EventInfo GaudiKernel PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
