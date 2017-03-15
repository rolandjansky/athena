################################################################################
# Package: egammaAlgs
################################################################################

# Declare the package name:
atlas_subdir( egammaAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/EventKernel
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTruth
                          GaudiKernel
                          PhysicsAnalysis/MCTruthClassifier
                          PRIVATE
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
                          Tools/PathResolver
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackLink
			  Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )

# tag ROOTMathLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( egammaAlgsLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps EventKernel xAODCaloEvent xAODEgamma xAODTruth GaudiKernel MCTruthClassifierLib CaloUtilsLib StoreGateLib SGtests ElectronPhotonSelectorToolsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloGeoHelpers AthenaKernel CxxUtils xAODTracking egammaRecEvent egammaUtils PathResolver Particle ParticleTruth TrkToolInterfaces InDetRecToolInterfaces FourMomUtils TrkTrack TrkPseudoMeasurementOnTrack InDetConversionFinderTools )

atlas_add_component( egammaAlgs
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps EventKernel xAODCaloEvent xAODEgamma xAODTruth GaudiKernel MCTruthClassifierLib CaloGeoHelpers CaloUtilsLib AthenaKernel CxxUtils StoreGateLib SGtests xAODTracking ElectronPhotonSelectorToolsLib egammaRecEvent egammaUtils PathResolver egammaAlgsLib Particle ParticleTruth TrkToolInterfaces InDetRecToolInterfaces FourMomUtils TrkTrack TrkPseudoMeasurementOnTrack InDetConversionFinderTools )

# Install files from the package:
atlas_install_python_modules( python/*.py )

