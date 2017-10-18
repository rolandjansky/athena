################################################################################
# Package: TrigBtagEmulationTool
################################################################################

# Declare the package name:
atlas_subdir( TrigBtagEmulationTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
			  AtlasPolicy
                          Control/AthContainers
                          Control/StoreGate
                          Control/AthToolSupport/AsgTools
			  Event/xAOD/xAODBTagging
			  Event/xAOD/xAODEventInfo
			  Event/xAOD/xAODJet
			  Event/xAOD/xAODTracking
			  Event/xAOD/xAODTrigger
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/AnalysisCommon/ParticleJetTools
			  Trigger/TrigAnalysis/TrigDecisionTool
			  Trigger/TrigConfiguration/TrigConfHLTData
			  Trigger/TrigEvent/TrigSteeringEvent
			  PhysicsAnalysis/JetTagging/JetTagAlgs/BTagging
			  PRIVATE
                          Control/AthenaBaseComps
			  Event/EventPrimitives
			  GaudiKernel )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigBtagEmulationToolLib
		   TrigBtagEmulationTool/ITrigBtagEmulationTool.h
		   INTERFACE
		   PUBLIC_HEADERS TrigBtagEmulationTool
		   LINK_LIBRARIES AsgTools )		   

atlas_add_component( TrigBtagEmulationTool
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthContainers StoreGateLib AsgTools xAODBTagging xAODEventInfo xAODJet xAODTracking xAODTrigger PATCoreLib ParticleJetToolsLib TrigDecisionToolLib TrigConfHLTData TrigSteeringEvent BTaggingLib AthenaBaseComps EventPrimitives GaudiKernel)

# Install files from the package:
atlas_install_python_modules( python/TrigBtag*.py )
atlas_install_joboptions( share/test*.py )
atlas_install_data( data/*.txt )


