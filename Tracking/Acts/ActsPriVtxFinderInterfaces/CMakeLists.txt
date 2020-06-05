# Declare the package name:
atlas_subdir( ActsPriVtxFinderInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PRIVATE
                          Control/StoreGate
                          AthenaKernel
                          AthenaBaseComps
                          GaudiKernel
                          ActsInterop
                          Event/xAOD/xAODTracking
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex )

# External dependencies:
find_package( Eigen )
find_package( Acts COMPONENTS Core )

# Component(s) in the package:

atlas_add_library( ActsPriVtxFinderInterfacesLib
                   ActsPriVtxFinderInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS ActsPriVtxFinderInterfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES}
                   AthenaKernel
                   ActsInteropLib
                   ActsCore
                   xAODTracking
                   TrkParameters
                   TrkParticleBase
                   TrkTrack VxVertex)

