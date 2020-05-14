################################################################################
# Package: TrkJetVxFitter
################################################################################

# Declare the package name:
atlas_subdir( TrkJetVxFitter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          GaudiKernel
                          MagneticField/MagFieldConditions
                          MagneticField/MagFieldElements
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrackLink
                          PRIVATE
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxJetVertex
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS MathCore Core Tree Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkJetVxFitterLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkJetVxFitter
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives GaudiKernel MagFieldConditions MagFieldElements TrkParameters
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrkNeutralParameters TrkTrack VxJetVertex VxVertex TrkVertexFitterInterfaces )

atlas_add_component( TrkJetVxFitter
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives GaudiKernel MagFieldConditions MagFieldElements TrkParameters TrkNeutralParameters TrkTrack VxJetVertex VxVertex TrkVertexFitterInterfaces TrkJetVxFitterLib )

