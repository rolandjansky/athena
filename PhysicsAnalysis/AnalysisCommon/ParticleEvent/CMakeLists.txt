################################################################################
# Package: ParticleEvent
################################################################################

# Declare the package name:
atlas_subdir( ParticleEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/Navigation
                          Control/SGTools
                          Event/EventKernel
                          Event/FourMom
                          Event/NavFourMom
                          Reconstruction/Particle
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ParticleEvent
                   src/*.cxx
                   PUBLIC_HEADERS ParticleEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers AthLinks Navigation SGTools EventKernel FourMom NavFourMom Particle VxVertex
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( ParticleEventDict
                      ParticleEvent/ParticleEventDict.h
                      ParticleEvent/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthLinks Navigation SGTools EventKernel FourMom NavFourMom Particle VxVertex ParticleEvent
                      NAVIGABLES  DataVector<ParticleBase> NeutrinoContainer CompositeParticleContainer
                      DATA_LINKS  TrackParticleAssocs INav4MomToTrackParticleAssocs )

