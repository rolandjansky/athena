# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoHardware )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( L1TopoHardware
   L1TopoHardware/*.h Root/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   PUBLIC_HEADERS L1TopoHardware
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_executable( TrigConfL1TopoGenPyHardware
   src/test/L1TopoGenPyHardware.cxx
   LINK_LIBRARIES L1TopoHardware )

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Create an auto-generated python description of the L1Topo hardware:
add_custom_command( OUTPUT
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoHardware/L1TopoHardware.py
   COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoHardware
   COMMAND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlas_build_run.sh
   TrigConfL1TopoGenPyHardware
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoHardware/L1TopoHardware.py
   DEPENDS TrigConfL1TopoGenPyHardware )
add_custom_target( L1TopoPyGenHardware ALL SOURCES
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoHardware/L1TopoHardware.py )
add_dependencies( Package_L1TopoHardware L1TopoPyGenHardware )

# Install the auto-generated file:
install( FILES
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoHardware/L1TopoHardware.py
   DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/L1TopoHardware
   OPTIONAL )
