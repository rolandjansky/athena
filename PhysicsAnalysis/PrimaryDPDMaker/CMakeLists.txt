################################################################################
# Package: PrimaryDPDMaker
################################################################################

# Declare the package name:
atlas_subdir( PrimaryDPDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthLinks
                          Control/AthContainers
                          Control/StoreGate
			  PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          Event/EventKernel
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/SGTools
                          DetectorDescription/RoiDescriptor
                          Event/FourMom
                          Event/FourMomUtils
                          Event/NavFourMom
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          LArCalorimeter/LArRecEvent
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/egamma/egammaEvent
                          Reconstruction/tauEvent
                          TestPolicy
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileIdentifier
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex )

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
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent AthenaBaseComps AthenaKernel AthLinks AthContainers IRegionSelector Identifier EventKernel GaudiKernel InDetIdentifier InDetReadoutGeometry InDetPrepRawData Particle TrkTrack TrkToolInterfaces StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools RoiDescriptor FourMom FourMomUtils NavFourMom xAODEventInfo xAODMuon xAODTracking InDetRIO_OnTrack LArRecEvent AnalysisTriggerEvent JetEvent muonEvent egammaEvent tauEvent TileEvent TileIdentifier TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack TrkTrackSummary VxVertex )

atlas_add_component( PrimaryDPDMaker
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaBaseComps AthenaKernel AthLinks AthContainers StoreGateLib SGtests IRegionSelector Identifier EventKernel GaudiKernel InDetIdentifier InDetReadoutGeometry InDetPrepRawData Particle TrkTrack TrkToolInterfaces SGTools RoiDescriptor FourMom FourMomUtils NavFourMom xAODEventInfo xAODMuon xAODTracking InDetRIO_OnTrack LArRecEvent AnalysisTriggerEvent JetEvent muonEvent egammaEvent tauEvent TileEvent TileIdentifier TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack TrkTrackSummary VxVertex PrimaryDPDMakerLib )

atlas_add_dictionary( PrimaryDPDMakerDict
                      PrimaryDPDMaker/PrimaryDPDMakerDict.h
                      PrimaryDPDMaker/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaBaseComps AthenaKernel AthLinks AthContainers StoreGateLib SGtests IRegionSelector Identifier EventKernel GaudiKernel InDetIdentifier InDetReadoutGeometry InDetPrepRawData Particle TrkTrack TrkToolInterfaces SGTools RoiDescriptor FourMom FourMomUtils NavFourMom xAODEventInfo xAODMuon xAODTracking InDetRIO_OnTrack LArRecEvent AnalysisTriggerEvent JetEvent muonEvent egammaEvent tauEvent TileEvent TileIdentifier TrkParameters TrkParticleBase TrkPseudoMeasurementOnTrack TrkTrackSummary VxVertex PrimaryDPDMakerLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/PrimaryDPDMaker_TestConfiguration.xml )

