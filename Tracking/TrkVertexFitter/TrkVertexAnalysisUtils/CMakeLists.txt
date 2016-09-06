################################################################################
# Package: TrkVertexAnalysisUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkVertexAnalysisUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Event/EventKernel
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PRIVATE
                          Tracking/TrkEvent/TrkParametersBase
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkVertexAnalysisUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrkVertexAnalysisUtils
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventKernel EventPrimitives xAODTracking GaudiKernel
                   PRIVATE_LINK_LIBRARIES TrkParametersBase TrkParticleBase VxVertex TrkExInterfaces )

atlas_add_component( TrkVertexAnalysisUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventKernel EventPrimitives xAODTracking GaudiKernel TrkParametersBase TrkParticleBase VxVertex TrkExInterfaces TrkVertexAnalysisUtilsLib )

