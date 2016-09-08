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
   Control/CxxUtils
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

# Component(s) in the package:
atlas_add_library( xAODEgamma
   xAODEgamma/*.h xAODEgamma/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODEgamma
   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthContainers AthLinks CxxUtils
   GeoPrimitives EventPrimitives xAODBase xAODCaloEvent xAODCore
   xAODPrimitives xAODTracking xAODTruth )

atlas_add_dictionary( xAODEgammaDict
   xAODEgamma/xAODEgammaDict.h
   xAODEgamma/selection.xml
   LINK_LIBRARIES xAODEgamma
   EXTRA_FILES Root/dict/*.cxx )
