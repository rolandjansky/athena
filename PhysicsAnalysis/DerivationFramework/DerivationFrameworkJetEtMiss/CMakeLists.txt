################################################################################
# Package: DerivationFrameworkJetEtMiss
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkJetEtMiss )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODCaloEvent
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/xAODBTaggingEfficiency
			  PhysicsAnalysis/AnalysisCommon/AssociationUtils
                          Reconstruction/Jet/JetInterface
			  Reconstruction/Jet/JetJvtEfficiency
			  Reconstruction/PFlow/PFlowUtils
                          Tools/PathResolver )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkJetEtMissLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps xAODCore xAODJet xAODPFlow xAODTracking xAODTrigger xAODTruth JetInterface JetJvtEfficiency PathResolver PFlowUtils AssociationUtilsLib )

atlas_add_component( DerivationFrameworkJetEtMiss
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps xAODCore xAODJet xAODPFlow xAODTracking xAODTrigger JetInterface JetJvtEfficiency PathResolver PFlowUtils DerivationFrameworkJetEtMissLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.root )

