################################################################################
# Package: TrkExTools
################################################################################

# Declare the package name:
atlas_subdir( TrkExTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          PRIVATE
                          Control/AthContainers
                          Control/CxxUtils
                          Control/StoreGate
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrkExTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives EventPrimitives xAODTracking GaudiKernel TrkDetDescrUtils TrkGeometry TrkSurfaces TrkVolumes TrkEventPrimitives TrkNeutralParameters TrkParameters TrkExInterfaces TrkExUtils AthContainers CxxUtils StoreGateLib SGtests TrkDetDescrInterfaces TrkEventUtils TrkMaterialOnTrack TrkParticleBase TrkTrack )

# Install files from the package:
atlas_install_headers( TrkExTools )
atlas_install_python_modules( python/*.py )

