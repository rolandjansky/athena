################################################################################
# Package: CaloTrkMuIdTools
################################################################################

# Declare the package name:
atlas_subdir( CaloTrkMuIdTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( CaloTrkMuIdTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaBaseComps StoreGateLib SGtests xAODTracking GaudiKernel ICaloTrkMuIdTools RecoToolInterfaces TrkExInterfaces CaloDetDescrLib CaloGeoHelpers CaloIdentifier CaloUtilsLib xAODCaloEvent ParticleCaloExtension TileDetDescr PathResolver TrkSurfaces TrkCaloExtension TrkEventPrimitives CaloTrackingGeometryLib AthOnnxruntimeServiceLib FourMomUtils)

# Install files from the package:
atlas_install_headers( CaloTrkMuIdTools )
atlas_install_joboptions( share/*.py )
