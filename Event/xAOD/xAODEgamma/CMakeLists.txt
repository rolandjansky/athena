# $Id$
################################################################################
# Package: xAODEgamma
################################################################################

# Declare the package name:
atlas_subdir( xAODEgamma )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   DetectorDescription/GeoPrimitives
   Event/EventPrimitives
   Event/xAOD/xAODBase
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODCore
   Event/xAOD/xAODPrimitives
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core GenVector )

# Extra dependencies, based on what environment we are in:
if (BUILDVP1LIGHT)
    if( BUILDVP1LIGHT_DIST STREQUAL "ubuntu")
        set( extra_libs GenVector )
    endif()
endif()

# Component(s) in the package:
atlas_add_library( xAODEgamma
   xAODEgamma/*.h xAODEgamma/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODEgamma
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthLinks
   GeoPrimitives EventPrimitives xAODBase xAODCaloEvent xAODCore
   xAODPrimitives xAODTracking xAODTruth ${extra_libs} )

atlas_add_dictionary( xAODEgammaDict
   xAODEgamma/xAODEgammaDict.h
   xAODEgamma/selection.xml
   LINK_LIBRARIES xAODEgamma
   EXTRA_FILES Root/dict/*.cxx )
