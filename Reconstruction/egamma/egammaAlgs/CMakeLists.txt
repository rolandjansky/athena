################################################################################
# Package: egammaAlgs
################################################################################

# Declare the package name:
atlas_subdir( egammaAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Event/EventKernel
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PhysicsAnalysis/MCTruthClassifier
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloUtils
                          Calorimeter/CaloEvent
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/StoreGate
                          Event/FourMomUtils
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetRecTools/InDetConversionFinderTools
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          Reconstruction/Particle
                          Reconstruction/ParticleTruth
                          Reconstruction/egamma/egammaInterfaces
                          Reconstruction/egamma/egammaRecEvent
                          Reconstruction/egamma/egammaUtils
                          Reconstruction/egamma/egammaMVACalib
                          Tools/PathResolver
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackLink
			  Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )

atlas_add_component( egammaAlgs
		     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps EventKernel xAODCaloEvent xAODEgamma xAODTruth GaudiKernel MCTruthClassifierLib CaloGeoHelpers CaloUtilsLib AthenaKernel CxxUtils StoreGateLib SGtests xAODTracking ElectronPhotonSelectorToolsLib egammaRecEvent egammaUtils egammaMVACalibLib PathResolver Particle ParticleTruth TrkToolInterfaces InDetRecToolInterfaces FourMomUtils TrkTrack TrkPseudoMeasurementOnTrack InDetConversionFinderToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

