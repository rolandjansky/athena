# $Id: CMakeLists.txt 778348 2016-10-13 18:53:21Z krasznaa $
################################################################################
# Package: egammaRecEvent
################################################################################

# Declare the package name:
atlas_subdir( egammaRecEvent )

# Extra dependencies based on the environment:
set( extra_deps )
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/SGTools )
   set( extra_libs SGTools )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODTracking
   ${extra_deps} )

# Component(s) in the package:
atlas_add_library( egammaRecEvent
   egammaRecEvent/*.h Root/*.cxx
   PUBLIC_HEADERS egammaRecEvent
   LINK_LIBRARIES AthContainers AthLinks xAODCaloEvent xAODTracking
   ${extra_libs} )

atlas_add_dictionary( egammaRecEventDict
   egammaRecEvent/egammaRecEventDict.h
   egammaRecEvent/selection.xml
   LINK_LIBRARIES egammaRecEvent )
