# $Id: CMakeLists.txt 785819 2016-11-22 17:50:02Z krasznaa $
################################################################################
# Package: Hephaestus
################################################################################

# Declare the package name:
atlas_subdir( Hephaestus )

# External dependencies:
find_package( Boost )
find_package( PythonLibs )
find_package( libunwind )

# Component(s) in the package:
atlas_add_library( Hephaestus
   Hephaestus/*.h src/hlib/*.c
   PUBLIC_HEADERS Hephaestus
   INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS}
   LINK_LIBRARIES ${PYTHON_LIBRARIES} )

atlas_add_library( MemoryTracker
   src/*.c
   PUBLIC_HEADERS Hephaestus
   PRIVATE_INCLUDE_DIRS ${LIBUNWIND_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${LIBUNWIND_LIBRARIES} Hephaestus )

atlas_add_executable( hephprof
   src/HephProf/HephProf.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} )

# Construct the python loadable modules:
add_custom_command( TARGET MemoryTracker POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E create_symlink
   "${CMAKE_SHARED_LIBRARY_PREFIX}MemoryTracker${CMAKE_SHARED_LIBRARY_SUFFIX}"
   "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/MemoryTracker${CMAKE_SHARED_LIBRARY_SUFFIX}"
   COMMAND ${CMAKE_COMMAND} -E create_symlink
   "${CMAKE_SHARED_LIBRARY_PREFIX}MemoryTracker${CMAKE_SHARED_LIBRARY_SUFFIX}.dbg"
   "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/MemoryTracker${CMAKE_SHARED_LIBRARY_SUFFIX}.dbg"
   COMMAND ${CMAKE_COMMAND} -E create_symlink
   "../../${CMAKE_INSTALL_LIBDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}MemoryTracker${CMAKE_SHARED_LIBRARY_SUFFIX}"
   "${CMAKE_PYTHON_OUTPUT_DIRECTORY}/Hephaestus/MemoryTracker${CMAKE_SHARED_LIBRARY_SUFFIX}"
   COMMAND ${CMAKE_COMMAND} -E create_symlink
   "../../${CMAKE_INSTALL_LIBDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}MemoryTracker${CMAKE_SHARED_LIBRARY_SUFFIX}.dbg"
   "${CMAKE_PYTHON_OUTPUT_DIRECTORY}/Hephaestus/MemoryTracker${CMAKE_SHARED_LIBRARY_SUFFIX}.dbg" )

# ...and also install them:
install( PROGRAMS
   "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/MemoryTracker${CMAKE_SHARED_LIBRARY_SUFFIX}"
   "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/MemoryTracker${CMAKE_SHARED_LIBRARY_SUFFIX}.dbg"
   DESTINATION ${CMAKE_INSTALL_LIBDIR}
   OPTIONAL )
install( PROGRAMS
   "${CMAKE_PYTHON_OUTPUT_DIRECTORY}/Hephaestus/MemoryTracker${CMAKE_SHARED_LIBRARY_SUFFIX}"
   "${CMAKE_PYTHON_OUTPUT_DIRECTORY}/Hephaestus/MemoryTracker${CMAKE_SHARED_LIBRARY_SUFFIX}.dbg"
   DESTINATION ${CMAKE_INSTALL_PYTHONDIR}/Hephaestus
   OPTIONAL )

# Test(s) in the package:
atlas_add_test( stackstash_test
   SOURCES test/stackstash_test.cxx
   LINK_LIBRARIES Hephaestus )

# Install files from the package:
atlas_install_python_modules( python/*.py )
