# $Id: CMakeLists.txt 732183 2016-03-24 13:52:44Z krasznaa $
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
   Control/StoreGate
   Event/xAOD/EventInfo
   GaudiKernel )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree )
find_package( gdb )
find_package( gperftools )
find_package( libunwind )
find_package( ZLIB )

# Component(s) in the package:
atlas_add_component( AthenaAuditors
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${GDB_INCLUDE_DIRS}
   ${LIBUNWIND_INCLUDE_DIRS} ${GPERFTOOLS_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${GDB_LIBRARIES}
   ${LIBUNWIND_LIBRARIES} ${ZLIB_LIBRARIES}
   AthenaBaseComps CoWTools GaudiKernel xAODEventInfo)
