################################################################################
# Package: TrkExAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrkExAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
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
                          Tracking/TrkExtrapolation/TrkExUtils )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrkExAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps DataModel GeoPrimitives EventPrimitives GaudiKernel MagFieldInterfaces TrkSurfaces TrkParameters StoreGateLib SGtests TrkGeometry TrkVolumes TrkEventPrimitives TrkTrack TrkExInterfaces TrkExUtils )

# Install files from the package:
atlas_install_headers( TrkExAlgs )

