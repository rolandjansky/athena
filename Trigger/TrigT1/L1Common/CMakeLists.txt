# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( L1Common )

# Find the necessary externals:
find_package( Python COMPONENTS Interpreter )

# The generator used to produce header files:
set( _generator ${CMAKE_CURRENT_SOURCE_DIR}/scripts/XMLtoHeader.py )

# Set up its installation:
atlas_install_scripts( ${_generator} )

# Generate the files:
add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/L1Common
   COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_CURRENT_BINARY_DIR}/L1Common
   COMMAND ${Python_EXECUTABLE} ${_generator}
   -o ${CMAKE_CURRENT_BINARY_DIR}/L1Common/L1Common
   -i ${CMAKE_CURRENT_SOURCE_DIR}/schema/L1Specifications.xml
   COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/L1Common
   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/L1Common/*.h
   ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/L1Common
   COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1Common
   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/L1Common/*.py
   ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1Common
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/schema/L1Specifications.xml )
add_custom_target( L1CommonGen ALL DEPENDS
   ${CMAKE_CURRENT_BINARY_DIR}/L1Common )
add_dependencies( Package_L1Common L1CommonGen )

# And install them:
install( DIRECTORY ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/L1Common
   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   OPTIONAL )
install( DIRECTORY ${CMAKE_PYTHON_OUTPUT_DIRECTORY}/L1Common
   DESTINATION ${CMAKE_INSTALL_PYTHONDIR}
   OPTIONAL
   PATTERN "__init__.py" EXCLUDE )

# Install the python code of the package:
atlas_install_python_modules( python/__init__.py )
