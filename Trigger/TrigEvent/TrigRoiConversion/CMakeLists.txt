# $Id: CMakeLists.txt 789262 2016-12-12 13:15:37Z krasznaa $
################################################################################
# Package: TrigRoiConversion
################################################################################

# Declare the package name:
atlas_subdir( TrigRoiConversion )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Trigger/TrigEvent/TrigSteeringEvent
   PRIVATE
   Event/xAOD/xAODTrigger
   ${extra_deps} )

# Component(s) in the package:
atlas_add_library( TrigRoiConversionLib
   TrigRoiConversion/*.h Root/*.cxx
   PUBLIC_HEADERS TrigRoiConversion
   LINK_LIBRARIES AsgTools TrigSteeringEvent
   PRIVATE_LINK_LIBRARIES xAODTrigger )

if( NOT XAOD_STANDALONE )
   atlas_add_component( TrigRoiConversion
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps GaudiKernel xAODTrigger
      TrigRoiConversionLib )
endif()
