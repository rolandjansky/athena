################################################################################
# Package: CaloTrackUtils
################################################################################

# Declare the package name:
atlas_subdir( CaloTrackUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloRec
                          GaudiKernel
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODCaloEvent
                          Reconstruction/Particle
                          TestPolicy
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( CaloTrackUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS CaloTrackUtils
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel CaloRecLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives xAODCaloEvent Particle TrkSurfaces TrkParameters TrkParticleBase VxVertex TrkExInterfaces )

atlas_add_component( CaloTrackUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} CaloRecLib GaudiKernel GeoPrimitives xAODCaloEvent Particle TrkSurfaces TrkParameters TrkParticleBase VxVertex TrkExInterfaces CaloTrackUtilsLib )

