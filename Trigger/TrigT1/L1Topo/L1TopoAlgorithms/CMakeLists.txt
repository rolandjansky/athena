# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( L1TopoAlgorithms )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )

# Make sure that the executable of the package gets linked against all of its
# dependencies:
string( REPLACE "-Wl,--as-needed" "" CMAKE_EXE_LINKER_FLAGS
   "${CMAKE_EXE_LINKER_FLAGS}" )

# The component(s) in the package:
atlas_add_library( L1TopoAlgorithms
   L1TopoAlgorithms/*.h Root/*.cxx
   PUBLIC_HEADERS L1TopoAlgorithms
   LINK_LIBRARIES L1TopoEvent L1TopoInterfaces
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} L1TopoCommon L1TopoSimulationUtils )

atlas_add_executable( TrigConfL1TopoGenPyAlg
   src/test/L1TopoGenPyAlg.cxx
   LINK_LIBRARIES L1TopoAlgorithms )

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

# Create an auto-generated python description of the available algorithms:
add_custom_command( OUTPUT
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoAlgorithms/L1TopoAlgConfig.py
   COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoAlgorithms
   COMMAND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlas_build_run.sh
   TrigConfL1TopoGenPyAlg
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoAlgorithms/L1TopoAlgConfig.py
   DEPENDS TrigConfL1TopoGenPyAlg )
add_custom_target( L1TopoPyGenAlg ALL SOURCES
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoAlgorithms/L1TopoAlgConfig.py )
add_dependencies( Package_L1TopoAlgorithms L1TopoPyGenAlg )

# Install the auto-generated file:
install( FILES
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1TopoAlgorithms/L1TopoAlgConfig.py
   DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/L1TopoAlgorithms OPTIONAL )
