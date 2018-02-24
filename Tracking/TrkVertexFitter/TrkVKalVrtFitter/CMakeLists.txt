################################################################################
# Package: TrkVKalVrtFitter
################################################################################

# Declare the package name:
atlas_subdir( TrkVKalVrtFitter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxSecVertex
                          Tracking/TrkVertexFitter/TrkVKalVrtCore
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          Tracking/TrkDetDescr/TrkSurfaces )
                          #Tracking/TrkEvent/VxVertex

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkVKalVrtFitterLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkVKalVrtFitter
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives xAODTracking GaudiKernel MagFieldInterfaces TrkNeutralParameters TrkParameters TrkParticleBase TrkTrack VxSecVertex VxVertex TrkExInterfaces TrkVKalVrtCore TrkVertexFitterInterfaces
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} TrkSurfaces )

atlas_add_component( TrkVKalVrtFitter
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives xAODTracking GaudiKernel MagFieldInterfaces TrkNeutralParameters TrkParameters TrkParticleBase TrkTrack VxSecVertex VxVertex TrkExInterfaces TrkVKalVrtCore TrkVertexFitterInterfaces TrkSurfaces TrkVKalVrtFitterLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

