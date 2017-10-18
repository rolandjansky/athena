################################################################################
# Package: egammaTools
################################################################################

# Declare the package name:
atlas_subdir( egammaTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloConditions
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloRec
                          Calorimeter/CaloUtils
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/SGTools
                          Control/StoreGate
                          Control/AthContainers
                          DetectorDescription/Identifier
                          DetectorDescription/GeoPrimitives
                          Event/EventKernel
                          Event/EventPrimitives
                          Event/FourMom
                          Event/FourMomUtils
                          Event/NavFourMom
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          InnerDetector/InDetRecTools/InDetConversionFinderTools
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArCabling
                          LumiBlock/LumiBlockComps
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonFourMomentumCorrection
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          Reconstruction/egamma/egammaInterfaces
                          Reconstruction/egamma/egammaMVACalib
                          Reconstruction/egamma/egammaRecEvent
                          Reconstruction/egamma/egammaUtils
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( egammaTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloConditions CaloEvent  CaloIdentifier CaloRecLib CaloUtilsLib AthenaBaseComps AthenaKernel CxxUtils AthContainers SGTools StoreGateLib SGtests Identifier EventKernel EventPrimitives FourMom FourMomUtils NavFourMom xAODCaloEvent  xAODEgamma xAODTracking xAODTruth GaudiKernel InDetConversionFinderToolsLib InDetRecToolInterfaces LArRecConditions LArCablingLib LumiBlockCompsLib PATCoreLib ElectronPhotonFourMomentumCorrectionLib ElectronPhotonSelectorToolsLib Particle ParticleTruth egammaMVACalibLib egammaRecEvent egammaUtils TrkEventPrimitives TrkMaterialOnTrack TrkParticleBase TrkPseudoMeasurementOnTrack TrkTrack VxVertex TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( egammaTools )
atlas_install_python_modules( python/*.py )

