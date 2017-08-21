################################################################################
# Package: DerivationFrameworkBPhys
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkBPhys )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODMuon			
                          Control/DataModel
                          Control/AthenaBaseComps
			  PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PhysicsAnalysis/JpsiUpsilonTools
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODBPhys
                          Control/AthenaKernel
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Event/EventPrimitives
		          PhysicsAnalysis/BPhys/BPhysTools
                          PRIVATE
                          Event/xAOD/xAODBase			
                          Tracking/TrkVertexFitter/TrkVertexAnalysisUtils
                          Event/xAOD/xAODEventInfo
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODPrimitives
                          Calorimeter/CaloEvent
                          Reconstruction/RecoTools/TrackToCalo
                          Reconstruction/RecoTools/TrackToVertex
                          Tracking/TrkEvent/TrkCaloExtension
                          InnerDetector/InDetConditions/InDetBeamSpotService
			  InnerDetector/InDetRecTools/InDetTrackSelectionTool
			  Trigger/TrigAnalysis/TrigDecisionTool
			  )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkBPhysLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkBPhys
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES xAODMuon DataModel AthenaBaseComps
		   JpsiUpsilonTools
		   xAODTracking xAODBPhysLib
		   AthenaKernel RecoToolInterfaces EventPrimitives
		   BPhysToolsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} xAODBase
		   TrkVertexAnalysisUtils xAODEventInfo AthenaPoolUtilities
		   xAODPrimitives CaloEvent TrackToCalo TrackToVertex
		   TrkCaloExtension InDetBeamSpotServiceLib
		   InDetTrackSelectionToolLib TrigDecisionTool)

atlas_add_component( DerivationFrameworkBPhys
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} xAODMuon xAODBase
		     DataModel AthenaBaseComps 
		     JpsiUpsilonTools xAODTracking xAODBPhysLib
		     AthenaKernel RecoToolInterfaces TrkVertexAnalysisUtils
		     xAODEventInfo AthenaPoolUtilities xAODPrimitives CaloEvent
		     TrackToCalo TrackToVertex TrkCaloExtension
		     DerivationFrameworkBPhysLib EventPrimitives
		     InDetBeamSpotServiceLib InDetTrackSelectionToolLib TrigDecisionTool)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
