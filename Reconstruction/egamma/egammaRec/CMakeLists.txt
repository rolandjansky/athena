################################################################################
# Package: egammaRec
################################################################################

# Declare the package name:
atlas_subdir( egammaRec )

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
                          Control/AthenaKernel
                          Control/CxxUtils
                          Control/StoreGate
                          Event/xAOD/xAODTracking
                          PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
                          Reconstruction/egamma/egammaInterfaces
                          Reconstruction/egamma/egammaRecEvent
                          Reconstruction/egamma/egammaUtils
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )

# tag ROOTMathLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( egammaRecLib
                   src/*.cxx
                   PUBLIC_HEADERS egammaRec
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps EventKernel xAODCaloEvent xAODEgamma xAODTruth GaudiKernel MCTruthClassifierLib CaloUtilsLib StoreGateLib SGtests EgammaAnalysisInterfacesLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloGeoHelpers AthenaKernel CxxUtils xAODTracking egammaRecEvent egammaUtils PathResolver )

atlas_add_component( egammaRec
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps EventKernel xAODCaloEvent xAODEgamma xAODTruth GaudiKernel MCTruthClassifierLib CaloGeoHelpers CaloUtilsLib 
		     AthenaKernel CxxUtils StoreGateLib SGtests xAODTracking EgammaAnalysisInterfacesLib egammaRecEvent egammaUtils PathResolver egammaRecLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

