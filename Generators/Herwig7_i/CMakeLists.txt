################################################################################
# Package: Herwig7_i
################################################################################

# Declare the package name:
atlas_subdir( Herwig7_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Generators/GeneratorModules
   Generators/AtlasHepMC
   PRIVATE
   Event/EventInfo
   GaudiKernel
   Tools/PathResolver )

# External dependencies:
find_package( Boost )
find_package( Herwig3 COMPONENTS HerwigAPI )
find_package( ThePEG )
find_package( GSL )
# find_package( OpenLoops )
find_package( VBFNLO )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_component( Herwig7_i
   Herwig7_i/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HERWIG3_INCLUDE_DIRS}
   ${THEPEG_INCLUDE_DIRS} ${GSL_INCLUDE_DIRS} ${VBFNLO_INCLUDE_DIRS}
   DEFINITIONS -DHWVERSION=\"${HERWIG3_LCGVERSION}\"
   -DHWVER_IS_72=\"${HW3_VER_IS_72}\"
   LINK_LIBRARIES ${HERWIG3_LIBRARIES}  ${Boost_LIBRARIES} 
   ${THEPEG_LIBRARIES} ${GSL_LIBRARIES} ${VBFNLO_LIBRARIES}
   GeneratorModulesLib EventInfo GaudiKernel PathResolver AtlasHepMCLib )
# ${OPENLOOPS_INCLUDE_DIRS} ${OPENLOOPS_LIBRARIES}

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/common/*.py )
atlas_install_runtime( share/file/*.pdt share/file/*.dat )

# Set up some environment variables for Herwig.
set( Herwig7Environment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of Herwig7Environment.cmake" )
find_package( Herwig7Environment )
