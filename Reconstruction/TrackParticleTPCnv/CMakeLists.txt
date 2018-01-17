################################################################################
# Package: TrackParticleTPCnv
################################################################################

# Declare the package name:
atlas_subdir( TrackParticleTPCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/DataModelAthenaPool
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkEventCnv/TrkEventTPCnv
                          PRIVATE
                          Control/AthenaKernel
                          Control/CxxUtils
                          DetectorDescription/GeoPrimitives )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_tpcnv_library( TrackParticleTPCnv
                         src/*.cxx
                         src/Particle/*.cxx
                         PUBLIC_HEADERS TrackParticleTPCnv
                         PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                         LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities Particle TrkParameters TrkParticleBase TrkTrack VxVertex TrkEventTPCnv AthenaKernel CxxUtils GeoPrimitives )

atlas_add_dictionary( TrackParticleTPCnvDict
                      TrackParticleTPCnv/TrackParticleTPCnvDict.h
                      TrackParticleTPCnv/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthLinks DataModelAthenaPoolLib AthenaPoolCnvSvcLib AthenaPoolUtilities Particle TrkParameters TrkParticleBase TrkTrack VxVertex TrkEventTPCnv AthenaKernel CxxUtils GeoPrimitives TrackParticleTPCnv )

