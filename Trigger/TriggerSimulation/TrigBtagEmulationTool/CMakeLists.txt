################################################################################
# Package: TrigBtagEmulationTool
################################################################################

# Declare the package name:
atlas_subdir( TrigBtagEmulationTool )

set( extra_include_public_dirs )
set( extra_include_public_libs )
set( extra_include_private_dirs )
set( extra_include_private_libs )

if ( XAOD_STANDALONE )
   set( extra_include_public_dirs Trigger/TrigConfiguration/TrigConfxAOD )
   set( extra_include_public_libs TrigConfxAODLib )

elseif ( XAOD_ANALYSIS )
   set( extra_include_public_dirs Control/AthContainers 	  
   				  Control/StoreGate )
   set( extra_include_public_libs AthContainers 
   				  StoreGateLib )

   set( extra_include_private_dirs Control/AthenaBaseComps )
   set( extra_include_private_libs AthenaBaseComps )

else() 
   set( extra_include_public_dirs Control/AthContainers 
   				  Control/StoreGate 
				  PhysicsAnalysis/JetTagging/JetTagAlgs/BTagging )
   set( extra_include_public_libs AthContainers 
   				  StoreGateLib 
				  BTaggingLib )

   set( extra_include_private_dirs Control/AthenaBaseComps 
   				   GaudiKernel )
   set( extra_include_private_libs AthenaBaseComps 
   				   GaudiKernel )
endif() 


# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
			  AtlasPolicy
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
			  Tools/PathResolver
			  ${extra_include_public_dirs}
			  PRIVATE
			  Event/EventPrimitives
			  ${extra_include_private_dirs} )



# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )


# Component(s) in the package:
if ( XAOD_STANDALONE )
   atlas_add_library( TrigBtagEmulationToolLib
                      TrigBtagEmulationTool/*.h Root/*.cxx
                      PUBLIC_HEADERS TrigBtagEmulationTool
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} PATCoreLib AsgTools xAODBTagging xAODEventInfo xAODJet ParticleJetToolsLib xAODTracking xAODTrigger TrigDecisionToolLib TrigConfHLTData TrigSteeringEvent EventPrimitives PathResolver ${extra_include_public_libs} ${extra_include_private_libs} )

   atlas_install_headers( TrigBtagEmulationTool )

else()
   atlas_add_library( TrigBtagEmulationToolLib
                      TrigBtagEmulationTool/ITrigBtagEmulationTool.h
                      INTERFACE
                      PUBLIC_HEADERS TrigBtagEmulationTool
                      LINK_LIBRARIES AsgTools )

   atlas_add_component( TrigBtagEmulationTool
                        Root/*.cxx
                        src/*.cxx src/components/*.cxx
                        INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                        LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} PATCoreLib AsgTools xAODBTagging xAODEventInfo xAODJet ParticleJetToolsLib xAODTracking xAODTrigger TrigDecisionToolLib TrigConfHLTData TrigSteeringEvent EventPrimitives PathResolver ${extra_include_public_libs} ${extra_include_private_libs} )
endif()


# Install files from the package:
atlas_install_python_modules( python/TrigBtag*.py )
atlas_install_joboptions( share/test*.py )
atlas_install_data( data/*.txt )