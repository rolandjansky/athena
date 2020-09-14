################################################################################
# Package: TileD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( TileD3PDMaker )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TileD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${CORAL_LIBRARIES} AtlasHepMCLib CaloEvent CaloGeoHelpers AthContainers AthenaBaseComps 
		     StoreGateLib SGtests GeoModelUtilities Identifier EventInfo xAODCaloEvent xAODEventInfo xAODMissingET xAODMuon xAODPrimitives xAODTracking 
		     xAODTrigger GaudiKernel GeneratorObjects D3PDMakerUtils RecoToolInterfaces ITrackToVertex TileEvent TileIdentifier 
		     TrkParameters TrkParametersIdentificationHelpers VxVertex TrigInDetEvent TrigMuonEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

