# $Id: CMakeLists.txt 789457 2016-12-13 12:00:43Z krasznaa $
################################################################################
# Package: xAODTrackingCnv
################################################################################

# Declare the package name:
atlas_subdir( xAODTrackingCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   Event/xAOD/xAODTracking
   Tracking/TrkEvent/TrkTrack
   PRIVATE
   Control/AthenaBaseComps
   Control/AthenaKernel
   Event/EventPrimitives
   Event/xAOD/xAODCore
   Generators/GeneratorObjects
   PhysicsAnalysis/MCTruthClassifier
   Reconstruction/Particle
   Reconstruction/ParticleTruth
   Tracking/TrkEvent/TrkLinks
   Tracking/TrkEvent/TrkParticleBase
   Tracking/TrkEvent/TrkTrackLink
   Tracking/TrkEvent/TrkTruthData
   Tracking/TrkEvent/VxVertex
   Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_library( xAODTrackingCnvLib
   xAODTrackingCnv/*.h
   INTERFACE
   PUBLIC_HEADERS xAODTrackingCnv
   LINK_LIBRARIES xAODTracking TrkTrack GaudiKernel )

atlas_add_component( xAODTrackingCnv
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES xAODTracking TrkTrack AthenaBaseComps AthenaKernel
   EventPrimitives GaudiKernel GeneratorObjects MCTruthClassifierLib Particle
   ParticleTruth TrkLinks TrkParticleBase TrkTruthData VxVertex 
   TrkToolInterfaces xAODCore xAODTrackingCnvLib
   PRIVATE_LINK_LIBRARIES CxxUtils )
