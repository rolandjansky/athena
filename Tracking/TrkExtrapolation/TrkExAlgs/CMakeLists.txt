################################################################################
# Package: TrkExAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrkExAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthContainers
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Control/StoreGate
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          ActsGeometryInterfaces
                          ActsGeometry
                          ActsInterop)

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( Acts COMPONENTS Core )


# Component(s) in the package:
atlas_add_component( TrkExAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthContainers GeoPrimitives EventPrimitives GaudiKernel MagFieldInterfaces TrkSurfaces TrkParameters StoreGateLib SGtests TrkGeometry TrkVolumes TrkEventPrimitives TrkTrack TrkExInterfaces TrkExUtils ActsGeometryInterfacesLib ActsInteropLib ActsGeometryLib ActsCore)

atlas_install_python_modules( python/*.py )

# Install files from the package:
atlas_install_headers( TrkExAlgs )

