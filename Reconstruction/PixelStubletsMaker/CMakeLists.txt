################################################################################
# Package: PixelStubletsMaker
################################################################################

# Declare the package name:
atlas_subdir( PixelStubletsMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/xAOD/xAODTracking
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          PRIVATE
                          External/GaudiInterface
                          Database/AthenaPOOL/AthenaPoolUtilities
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tools/PathResolver
                          External/AtlasCLHEP
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTruth
                          Tracking/TrkEvent/TrkSpacePoint)


# External dependencies:
find_package( BLAS )
find_package( LAPACK )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( PixelStubletsMakerLib
                   src/*.cxx
                   PUBLIC_HEADERS PixelStubletsMaker
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIRS} ${BLAS_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES} AthenaBaseComps AthenaPoolUtilities InDetIdentifier PathResolver InDetRIO_OnTrack TrkMeasurementBase TrkParameters TrkSurfaces TrkTrack TrkToolInterfaces xAODEventInfo xAODMuon xAODTruth TrkSpacePoint StoreGateLib)
#                   PRIVATE_LINK_LIBRARIES GaudiInterface )

atlas_add_component( PixelStubletsMaker
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIRS} ${BLAS_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES} 
                     AthenaBaseComps AthenaPoolUtilities InDetIdentifier PathResolver InDetRIO_OnTrack TrkMeasurementBase TrkParameters TrkSurfaces TrkTrack TrkToolInterfaces xAODEventInfo xAODMuon xAODTruth TrkSpacePoint StoreGateLib )


# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

