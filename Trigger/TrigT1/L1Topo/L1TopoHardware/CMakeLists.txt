# $Id: CMakeLists.txt 730705 2016-03-17 12:59:16Z krasznaa $
################################################################################
# Package: L1TopoHardware
################################################################################

# Declare the package name:
atlas_subdir( L1TopoHardware )

# Component(s) in the package:
atlas_add_library( L1TopoHardware
   L1TopoHardware/*.h Root/*.cxx
   PUBLIC_HEADERS L1TopoHardware )

atlas_add_executable( TrigConfL1TopoGenPyHardware
   src/test/L1TopoGenPyHardware.cxx
   LINK_LIBRARIES L1TopoHardware )

# Install files from the package:
atlas_install_python_modules( python/*.py )

# In release recompilation mode only parse the following if the package is
# being rebuilt:
if( ATLAS_RELEASE_RECOMPILE_DRYRUN )
   return()
endif()
if( ATLAS_RELEASE_MODE AND NOT ATLAS_PACKAGE_RECOMPILE )
   return()
endif()

# Create an auto-generated python description of the L1Topo hardware:
add_custom_command( OUTPUT
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoHardware/L1TopoHardware.py
   COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoHardware
   COMMAND ${CMAKE_BINARY_DIR}/atlas_build_run.sh TrigConfL1TopoGenPyHardware
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoHardware/L1TopoHardware.py
   DEPENDS TrigConfL1TopoGenPyHardware )
add_custom_target( L1TopoPyGenHardware ALL SOURCES
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoHardware/L1TopoHardware.py )
add_dependencies( Package_L1TopoHardware L1TopoPyGenHardware )

# Install the auto-generated file:
install( FILES
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoHardware/L1TopoHardware.py
   DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/L1TopoHardware )
