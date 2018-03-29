# $Id: CMakeLists.txt 804307 2017-05-04 14:57:53Z oda $
################################################################################
# Package: DerivationFrameworkHiggs
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkHiggs )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/StoreGate
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODJet
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTracking
   PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
   Tracking/TrkVertexFitter/TrkVKalVrtFitter
   Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
   Trigger/TrigAnalysis/TrigDecisionTool
   PRIVATE
   GaudiKernel
   Tracking/TrkEvent/VxVertex
   Event/xAOD/xAODTrigger
   Control/AthContainers
   Generators/GenInterfaces
   Generators/TruthRivetTools
   Generators/TruthUtils
   Tools/PathResolver
   PhysicsAnalysis/ElectronPhotonID/PhotonVertexSelection
   PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Physics )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkHiggs
   DerivationFrameworkHiggs/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps
   xAODEgamma xAODJet xAODMuon xAODTracking TrkVKalVrtFitterLib
   TrkVertexFitterInterfaces TrigDecisionToolLib xAODEventInfo
   GaudiKernel VxVertex xAODTrigger PhotonVertexSelection AthContainers
   TruthConvertersLib TruthRivetToolsLib TruthUtils PathResolver EgammaAnalysisInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_generic( share/*.cfg
   DESTINATION share/DerivationFrameworkHiggs )
