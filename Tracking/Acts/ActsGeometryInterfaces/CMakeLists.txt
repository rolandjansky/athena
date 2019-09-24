
# Declare the package name:
atlas_subdir( ActsGeometryInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PRIVATE
                          Control/StoreGate
                          AthenaKernel
                          AthenaBaseComps
                          GaudiKernel
                          ActsInterop )

# External dependencies:
find_package( Eigen )
find_package( Acts QUIET COMPONENTS Core )

# Component(s) in the package:

atlas_add_library( ActsGeometryInterfacesLib
                   ActsGeometryInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS ActsGeometryInterfaces
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES}
                   AthenaKernel
                   ActsInteropLib
                   ActsCore)
