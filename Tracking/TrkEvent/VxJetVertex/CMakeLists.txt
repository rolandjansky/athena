# $Id: CMakeLists.txt 769408 2016-08-23 09:14:14Z krasznaa $
################################################################################
# Package: VxJetVertex
################################################################################

# Declare the package name:
atlas_subdir( VxJetVertex )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/EventPrimitives
   GaudiKernel
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/TrkParticleBase
   Tracking/TrkEvent/TrkTrack
   Tracking/TrkEvent/TrkTrackLink
   Tracking/TrkEvent/VxVertex
   Event/xAOD/xAODTracking
   PRIVATE
   Control/AthLinks )

# Component(s) in the package:
atlas_add_library( VxJetVertex
   VxJetVertex/*.h src/*.cxx
   PUBLIC_HEADERS VxJetVertex
   LINK_LIBRARIES EventPrimitives GaudiKernel TrkEventPrimitives TrkParticleBase
   TrkTrack VxVertex xAODTracking
   PRIVATE_LINK_LIBRARIES AthLinks )

atlas_add_dictionary( VxJetVertexDict
   VxJetVertex/VxJetVertexDict.h
   VxJetVertex/selection.xml
   LINK_LIBRARIES VxJetVertex )
