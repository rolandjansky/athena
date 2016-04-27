# $Id: CMakeLists.txt 743301 2016-04-27 14:34:12Z krasznaa $
################################################################################
# Package: TrigConf2COOLOnline
################################################################################

# Declare the package name:
atlas_subdir( TrigConf2COOLOnline )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Trigger/TrigConfiguration/TrigConfHLTData
   Trigger/TrigConfiguration/TrigConfL1Data
   Trigger/TrigConfiguration/TrigConfStorage )

# External dependencies:
find_package( Boost COMPONENTS program_options )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( tdaq COMPONENTS daq-core-dal daq-hlt-dal rc_CmdLine rc_ItemCtrl )
find_package( tdaq-common COMPONENTS ers )
find_package( TBB )

# Names of the auto-generated headers:
set( _autogenHeaders
   ${CMAKE_CURRENT_BINARY_DIR}/TrigConf2COOLOnline/TrigConfSmKeyNamed.h
   ${CMAKE_CURRENT_BINARY_DIR}/TrigConf2COOLOnline/TrigConfL1PsKeyNamed.h
   ${CMAKE_CURRENT_BINARY_DIR}/TrigConf2COOLOnline/TrigConfL1BgKeyNamed.h
   ${CMAKE_CURRENT_BINARY_DIR}/TrigConf2COOLOnline/TrigConfHltPsKeyNamed.h
   ${CMAKE_CURRENT_BINARY_DIR}/TrigConf2COOLOnline/TrigConfReleaseNamed.h )

# The hardcoded location of the Java executable:
set( _javaHome
   /afs/cern.ch/atlas/project/tdaq/inst/sw/lcg/external/Java/JDK/1.8.0/amd64 )

# Construct a functional CLASSPATH from the .jar files in tdaq:
file( GLOB _jarFiles "${TDAQ_ROOT}/installed/share/lib/*.jar" )
set( _classpath "." )
foreach( _jar ${_jarFiles} )
   set( _classpath "${_classpath}:${_jar}" )
endforeach()
unset( _jarFiles )

# Set up a rule generating some header files:
add_custom_command( OUTPUT ${_autogenHeaders}
   COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_CURRENT_BINARY_DIR}/TrigConf2COOLOnline
   COMMAND ${CMAKE_COMMAND} -E env TDAQ_JAVA_HOME=${_javaHome}
   CLASSPATH=${_classpath}
   ${CMAKE_BINARY_DIR}/atlas_build_run.sh is_generator.sh
   -d ${CMAKE_CURRENT_BINARY_DIR}/TrigConf2COOLOnline --cpp
   -n ${CMAKE_CURRENT_SOURCE_DIR}/schema/is_trigconf_hlt.schema.xml
   ${CMAKE_CURRENT_SOURCE_DIR}/schema/is_trigconf_l1.schema.xml )
unset( _classpath )

# Clean up the files on "make clean":
set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
   "${_autogenHeaders}" )

# Component(s) in the package:
atlas_add_library( TrigConf2COOLOnline
   src/*.cxx ${_autogenHeaders}
   PUBLIC_HEADERS TrigConf2COOLOnline
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
   ${TBB_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES}
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${TBB_LIBRARIES}
   TrigConfHLTData TrigConfL1Data TrigConfStorage )

atlas_add_executable( TrigConf2CoolOnline
   src/TrigConf2CoolOnline.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
   ${TDAQ_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES}
   TrigConf2COOLOnline )

# Install files from the package:
atlas_install_generic( schema/*.xml
   DESTINATION share/data/daq/schema
   TYPENAME Schema )
