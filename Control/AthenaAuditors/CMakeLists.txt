################################################################################
# Package: AthenaAuditors
################################################################################

# Declare the package name:
atlas_subdir( AthenaAuditors )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Control/AthenaBaseComps
   Control/CoWTools
   Control/AthenaKernel
   Control/CxxUtils
   GaudiKernel )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree )
find_package( gdb )
find_package( gperftools )
find_package( libunwind )
find_package( ZLIB )

# Skip building the package if an external is not available.
if( ( NOT Boost_FOUND ) OR ( NOT ROOT_FOUND ) OR ( NOT GDB_FOUND ) OR
    ( NOT GPERFTOOLS_FOUND ) OR ( NOT LIBUNWIND_FOUND ) OR ( NOT ZLIB_FOUND ) )
  message( WARNING "Not all externals available, not building AthenaAuditors." )
  return()
endif()

# Component(s) in the package:
atlas_add_component( AthenaAuditors
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${GDB_INCLUDE_DIRS}
   ${LIBUNWIND_INCLUDE_DIRS} ${GPERFTOOLS_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${GDB_LIBRARIES}
   ${LIBUNWIND_LIBRARIES} ${ZLIB_LIBRARIES} ${CMAKE_DL_LIBS}
   AthenaBaseComps CoWTools GaudiKernel )
