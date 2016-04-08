################################################################################
# Package: VrtSecInclusive
################################################################################

# Declare the package name:
atlas_subdir( VrtSecInclusive )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          Generators/GeneratorObjects
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          Reconstruction/RecoTools/ITrackToVertex
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( BLAS )
find_package( HepMC )
find_package( LAPACK )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( VrtSecInclusiveLib
                   src/*.cxx
                   PUBLIC_HEADERS VrtSecInclusive
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIRS} ${BLAS_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES} AthenaBaseComps GaudiKernel GeneratorObjects Particle ParticleTruth ITrackToVertex TrkSurfaces TrkEventUtils TrkParameters TrkParticleBase TrkTrack TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces StoreGateLib SGtests TrkVKalVrtFitterLib
                   PRIVATE_LINK_LIBRARIES ${HEPMC_LIBRARIES} xAODEventInfo McParticleEvent VxVertex )

atlas_add_component( VrtSecInclusive
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIRS} ${BLAS_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES} ${HEPMC_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GaudiKernel GeneratorObjects Particle ParticleTruth ITrackToVertex TrkSurfaces TrkEventUtils TrkParameters TrkParticleBase TrkTrack TrkExInterfaces TrkToolInterfaces TrkVKalVrtFitterLib TrkVertexFitterInterfaces xAODEventInfo McParticleEvent VxVertex VrtSecInclusiveLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

