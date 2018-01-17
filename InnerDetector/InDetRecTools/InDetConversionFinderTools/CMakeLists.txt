################################################################################
# Package: InDetConversionFinderTools
################################################################################

# Declare the package name:
atlas_subdir( InDetConversionFinderTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Control/AthLinks
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVertexSeedFinderUtils )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( InDetConversionFinderToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS InDetConversionFinderTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel InDetRecToolInterfaces Particle TrkEventPrimitives TrkParameters TrkParticleBase TrkTrack
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthLinks InDetPrepRawData TrkSurfaces TrkMeasurementBase TrkRIO_OnTrack VxVertex TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces )

atlas_add_component( InDetConversionFinderTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel InDetRecToolInterfaces Particle TrkEventPrimitives TrkParameters TrkParticleBase TrkTrack AthLinks InDetPrepRawData TrkSurfaces TrkMeasurementBase TrkRIO_OnTrack VxVertex TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces InDetConversionFinderToolsLib )

