# $Id: CMakeLists.txt 744492 2016-05-03 14:20:44Z krasznaa $
################################################################################
# Package: xAODCaloEvent
################################################################################

# Declare the package name:
atlas_subdir( xAODCaloEvent )

# Extra dependencies, based on what environment we're in:
if( NOT XAOD_ANALYSIS AND NOT SIMULATIONBASE )
   set( extra_deps Calorimeter/CaloEvent )
   set( extra_libs CaloEvent )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Calorimeter/CaloGeoHelpers
   Control/AthContainers
   Control/CxxUtils
   DetectorDescription/GeoPrimitives
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   Control/AthLinks
   ${extra_deps} )

# External dependencies:
find_package( Eigen )

# Extra source(s), based on what environment we are in:
if( NOT XAOD_ANALYSIS AND NOT SIMULATIONBASE )
   set( extra_sources src/*.cxx )
endif()

# Component(s) in the package:
atlas_add_library( xAODCaloEvent
   xAODCaloEvent/*.h xAODCaloEvent/versions/*.h Root/*.cxx ${extra_sources}
   PUBLIC_HEADERS xAODCaloEvent
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} CaloGeoHelpers AthContainers
   CxxUtils GeoPrimitives xAODBase xAODCore AthLinks ${extra_libs} )

atlas_add_dictionary( xAODCaloEventDict
   xAODCaloEvent/xAODCaloEventDict.h
   xAODCaloEvent/selection.xml
   LINK_LIBRARIES xAODCaloEvent
   EXTRA_FILES Root/dict/*.cxx )
