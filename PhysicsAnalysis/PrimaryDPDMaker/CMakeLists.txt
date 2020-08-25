################################################################################
# Package: PrimaryDPDMaker
################################################################################

# Declare the package name:
atlas_subdir( PrimaryDPDMaker )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( PrimaryDPDMakerLib
                   src/*.cxx
                   PUBLIC_HEADERS PrimaryDPDMaker
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent AthenaBaseComps AthenaKernel AthLinks AthContainers IRegionSelector Identifier EventKernel GaudiKernel InDetIdentifier InDetReadoutGeometry InDetPrepRawData Particle TrkTrack TrkToolInterfaces StoreGateLib SGtests DerivationFrameworkInterfaces
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} RoiDescriptor FourMom FourMomUtils NavFourMom xAODEventInfo xAODMuon xAODTracking InDetRIO_OnTrack LArRecEvent AnalysisTriggerEvent MuonSelectorToolsLib JetEvent muonEvent egammaEvent tauEvent TileEvent TileIdentifier TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack TrkTrackSummary VxVertex )

atlas_add_component( PrimaryDPDMaker
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaBaseComps AthenaKernel AthLinks AthContainers StoreGateLib SGtests IRegionSelector Identifier EventKernel GaudiKernel InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry InDetPrepRawData Particle TrkTrack TrkToolInterfaces RoiDescriptor FourMom FourMomUtils NavFourMom xAODEventInfo xAODMuon xAODTracking InDetRIO_OnTrack LArRecEvent AnalysisTriggerEvent MuonSelectorToolsLib JetEvent muonEvent egammaEvent tauEvent TileEvent TileIdentifier TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack TrkTrackSummary VxVertex PrimaryDPDMakerLib )

atlas_add_dictionary( PrimaryDPDMakerDict
                      PrimaryDPDMaker/PrimaryDPDMakerDict.h
                      PrimaryDPDMaker/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaBaseComps AthenaKernel AthLinks AthContainers StoreGateLib SGtests IRegionSelector Identifier EventKernel GaudiKernel InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry InDetPrepRawData Particle TrkTrack TrkToolInterfaces RoiDescriptor FourMom FourMomUtils NavFourMom xAODEventInfo xAODMuon xAODTracking InDetRIO_OnTrack LArRecEvent AnalysisTriggerEvent MuonSelectorToolsLib JetEvent muonEvent egammaEvent tauEvent TileEvent TileIdentifier TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack TrkTrackSummary VxVertex PrimaryDPDMakerLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/PrimaryDPDMaker_TestConfiguration.xml )

