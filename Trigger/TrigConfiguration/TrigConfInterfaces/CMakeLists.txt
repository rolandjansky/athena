# $Id: CMakeLists.txt 749937 2016-05-26 15:34:24Z krasznaa $
################################################################################
# Package: TrigConfInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrigConfInterfaces )

# Extra dependencies, for non-standalone builds:
if( NOT XAOD_STANDALONE )
   set( extra_deps GaudiKernel )
   set( extra_libs GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   ${extra_deps} )

# Component(s) in the package:
atlas_add_library( TrigConfInterfaces
   TrigConfInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS TrigConfInterfaces
   LINK_LIBRARIES AsgTools ${extra_libs} )

atlas_add_dictionary( TrigConfInterfacesDict
   TrigConfInterfaces/TrigConfInterfacesDict.h
   TrigConfInterfaces/selection.xml
   LINK_LIBRARIES TrigConfInterfaces )
