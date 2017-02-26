# $Id: CMakeLists.txt $
################################################################################
# Package: xAODTrackCaloClusterCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTrackCaloClusterCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   Event/xAOD/xAODTrackCaloCluster
   PRIVATE
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/CxxUtils
   Event/EventPrimitives
   Event/xAOD/xAODCore)

# Component(s) in the package:
atlas_add_library( xAODTrackCaloClusterCnvLib
                   xAODTrackCaloClusterCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTrackCaloClusterCnv
                   LINK_LIBRARIES xAODTrackCaloCluster  GaudiKernel )

atlas_add_component( xAODTrackCaloClusterCnv
                     src/*.h src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES xAODTrackCaloCluster AthenaBaseComps AthenaKernel
                     EventPrimitives GaudiKernel xAODCore xAODTrackCaloClusterCnvLib )