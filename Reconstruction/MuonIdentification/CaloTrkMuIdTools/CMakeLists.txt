################################################################################
# Package: CaloTrkMuIdTools
################################################################################

# Declare the package name:
atlas_subdir( CaloTrkMuIdTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloInterface
                          Calorimeter/CaloTrackingGeometry
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          Reconstruction/MuonIdentification/ICaloTrkMuIdTools
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloUtils
                          Event/xAOD/xAODCaloEvent
                          Reconstruction/RecoEvent/ParticleCaloExtension
                          TileCalorimeter/TileDetDescr
                          Tools/PathResolver
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCaloExtension
                          Tracking/TrkEvent/TrkEventPrimitives )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( onnxruntime )

# Component(s) in the package:
atlas_add_component( CaloTrkMuIdTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${ONNXRUNTIME_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${ONNXRUNTIME_LIBRARIES} CaloEvent AthenaBaseComps StoreGateLib SGtests xAODTracking GaudiKernel ICaloTrkMuIdTools RecoToolInterfaces TrkExInterfaces CaloDetDescrLib CaloGeoHelpers CaloIdentifier CaloUtilsLib xAODCaloEvent ParticleCaloExtension TileDetDescr PathResolver TrkSurfaces TrkCaloExtension TrkEventPrimitives CaloTrackingGeometryLib )

# Install files from the package:
atlas_install_headers( CaloTrkMuIdTools )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/CaloMuonLikelihood.PDF.A0.root share/CaloMuonLikelihood.PDF.A1.root share/CaloMuonLikelihood.PDF.A2.root share/CaloMuonLikelihood.PDF.B0.root share/CaloMuonLikelihood.PDF.B1.root share/CaloMuonLikelihood.PDF.B2.root share/CaloMuonLikelihood.PDF.C0.root share/CaloMuonLikelihood.PDF.C1.root share/CaloMuonLikelihood.PDF.C2.root share/CaloTag.CutConfig.root )
atlas_install_runtime( share/CaloMuonScoreModels/*.onnx )
