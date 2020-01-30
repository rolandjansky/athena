################################################################################
# Package: DerivationFrameworkExotics
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkExotics )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODJet
			  Event/xAOD/xAODCaloEvent
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          Reconstruction/Jet/JetCalibTools
                          Trigger/TrigAnalysis/TrigDecisionTool
			  PhysicsAnalysis/AnalysisCommon/PMGOverlapRemovalTools/GammaORTools
                          Tracking/TrkExtrapolation/TrkExUtils
                          PRIVATE
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODCutFlow
                          GaudiKernel
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          Tracking/TrkEvent/TrkParameters
                          Event/EventPrimitives
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
			  InDetTrackSelectionTool/InDetTrackSelectionTool
			  Reconstruction/Jet/JetEDM
                         )


# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkExoticsLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkExotics
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AsgTools xAODJet TrkExUtils TrigDecisionToolLib GammaORToolsLib InDetTrackSelectionToolLib 
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} xAODCore xAODEventInfo GaudiKernel xAODCutFlow JetEDM)

atlas_add_component( DerivationFrameworkExotics
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AsgTools xAODJet TrigDecisionToolLib xAODCore xAODCutFlow xAODEventInfo GaudiKernel GammaORToolsLib TrkExUtils InDetTrackSelectionToolLib JetEDM DerivationFrameworkExoticsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( data )
