################################################################################
# Package: DerivationFrameworkSM
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkSM )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
			  Event/xAOD/xAODBase
                          PRIVATE
                          Event/xAOD/xAODEgamma
			  Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
			  Trigger/TrigAnalysis/TriggerMatchingTool
                          GaudiKernel
			  PhysicsAnalysis/ElectronPhotonID/PhotonVertexSelection 
			  Reconstruction/VKalVrt/TrkToLeptonPVTool
                         )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkSMLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkSM
                   LINK_LIBRARIES AthenaBaseComps
                   PRIVATE_LINK_LIBRARIES xAODEgamma xAODTracking xAODBase xAODMuon xAODTracking TriggerMatchingToolLib PhotonVertexSelectionLib GaudiKernel )

atlas_add_component( DerivationFrameworkSM
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODBase xAODEgamma xAODMuon xAODTracking GaudiKernel TriggerMatchingToolLib PhotonVertexSelectionLib DerivationFrameworkSMLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

