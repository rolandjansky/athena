################################################################################
# Package: VrtSecFuzzy
################################################################################

# Declare the package name:
atlas_subdir( VrtSecFuzzy )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODEgamma
                          GaudiKernel
                          Generators/GeneratorObjects
                          Reconstruction/RecoTools/ITrackToVertex
                          DetectorDescription/AtlasDetDescr
                          InnerDetector/InDetConditions/InDetConditionsSummaryService
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex 
                          Reconstruction/MVAUtils 
                          )
 

# External dependencies:
find_package( BLAS )
find_package( LAPACK )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread TMVA  )

# Component(s) in the package:
atlas_add_library( VrtSecFuzzyLib
                   src/*.cxx
                   PUBLIC_HEADERS VrtSecFuzzy
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIRS} ${BLAS_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES} AthenaBaseComps xAODEventInfo xAODTracking xAODTruth xAODMuon xAODEgamma GaudiKernel GeneratorObjects ITrackToVertex TrkDetDescrInterfaces TrkSurfaces TrkExInterfaces TrkToolInterfaces TrkVertexFitterInterfaces StoreGateLib SGtests TrkVKalVrtFitterLib AtlasDetDescr InDetIdentifier MVAUtils PathResolver 
                   PRIVATE_LINK_LIBRARIES xAODEventInfo TrkTrackSummary VxVertex )

atlas_add_component( VrtSecFuzzy
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIRS} ${BLAS_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES} AthenaBaseComps StoreGateLib SGtests xAODEventInfo xAODTracking xAODTruth xAODMuon xAODEgamma GaudiKernel GeneratorObjects ITrackToVertex TrkDetDescrInterfaces TrkSurfaces TrkExInterfaces TrkToolInterfaces TrkVKalVrtFitterLib TrkVertexFitterInterfaces xAODEventInfo TrkTrackSummary VxVertex VrtSecFuzzyLib AtlasDetDescr InDetIdentifier )

# Install files from the package:
atlas_install_python_modules( python/*.py )
#atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

