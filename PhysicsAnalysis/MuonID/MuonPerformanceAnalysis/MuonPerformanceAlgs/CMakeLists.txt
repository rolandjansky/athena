# $Id: CMakeLists.txt 780323 2016-10-25 15:06:22Z nkoehler $
################################################################################
# Package: MuonPerformanceAlgs
################################################################################
 
# Declare the package name:
atlas_subdir( MuonPerformanceAlgs )
 
# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Reconstruction/RecoTools/RecoToolInterfaces
   PRIVATE
   Control/AthenaBaseComps
   Control/AthAnalysisBaseComps
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODMuon
   Event/xAOD/xAODPrimitives
   Event/xAOD/xAODTracking
   Event/xAOD/xAODCutFlow
   Event/xAOD/xAODMetaData
   Event/EventInfo/EventInfo
   Event/EventBookkeeperMetaData
   GaudiKernel
   PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceHistUtils
   PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonResonanceTools
   PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonTPTools
   PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
   )
 
# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
 
# Component(s) in the package:
atlas_add_component( MuonPerformanceAlgs
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} RecoToolInterfaces AthenaBaseComps
   AthAnalysisBaseCompsLib xAODCutFlow xAODBase xAODCore xAODEventInfo xAODMetaData
   xAODMuon xAODPrimitives xAODTracking EventBookkeeperMetaData GaudiKernel
   MuonPerformanceHistUtils MuonResonanceToolsLib DerivationFrameworkInterfaces )
 
# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
