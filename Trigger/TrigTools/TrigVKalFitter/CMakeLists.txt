################################################################################
# Package: TrigVKalFitter
################################################################################

# Declare the package name:
atlas_subdir( TrigVKalFitter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkVertexFitter/TrkVKalVrtCore
                          PRIVATE
                          Trigger/TrigEvent/TrigInDetEvent )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrigVKalFitterLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigVKalFitter
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives GaudiKernel MagFieldInterfaces TrkVKalVrtCore
                   PRIVATE_LINK_LIBRARIES TrigInDetEvent )

atlas_add_component( TrigVKalFitter
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives GaudiKernel MagFieldInterfaces TrkVKalVrtCore TrigInDetEvent TrigVKalFitterLib )

