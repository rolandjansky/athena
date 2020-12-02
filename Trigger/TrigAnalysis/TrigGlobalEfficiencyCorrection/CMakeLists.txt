################################################################################
# Package: TrigGlobalEfficiencyCorrection
################################################################################

atlas_subdir( TrigGlobalEfficiencyCorrection )

set( athena_subdirs )
if( NOT XAOD_STANDALONE )
  set( athena_subdirs GaudiKernel PhysicsAnalysis/POOLRootAccess Control/AthAnalysisBaseComps )
endif()

find_package( Boost )
find_package( ROOT COMPONENTS RIO )

atlas_add_library( TrigGlobalEfficiencyCorrectionLib
   TrigGlobalEfficiencyCorrection/*.h Root/*.cxx
   #PUBLIC_HEADERS TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h
   PUBLIC_HEADERS TrigGlobalEfficiencyCorrection
   PRIVATE_INCLUDE_DIRS ${BOOST_INCLUDE_DIRS} TriggerAnalysisInterfaces
   LINK_LIBRARIES AsgTools xAODEgamma xAODMuon TriggerAnalysisInterfaces EgammaAnalysisInterfacesLib MuonAnalysisInterfacesLib xAODEventInfo PathResolver TriggerMatchingToolLib
   PRIVATE_LINK_LIBRARIES ${BOOST_LIBRARIES}
)

if( NOT XAOD_STANDALONE )
   atlas_add_component( TrigGlobalEfficiencyCorrection
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES xAODEgamma xAODMuon EgammaAnalysisInterfacesLib MuonAnalysisInterfacesLib xAODEventInfo GaudiKernel TrigGlobalEfficiencyCorrectionLib
   )
endif()

atlas_add_dictionary( TrigGlobalEfficiencyCorrectionDict
   TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionDict.h
   TrigGlobalEfficiencyCorrection/selection.xml
   LINK_LIBRARIES TrigGlobalEfficiencyCorrectionLib
)

set( pool_lib )
if( NOT XAOD_STANDALONE )
  set( pool_lib POOLRootAccessLib AthAnalysisBaseCompsLib )
endif()

foreach(example TrigGlobEffCorrExample0 TrigGlobEffCorrExample1
TrigGlobEffCorrExample3a TrigGlobEffCorrExample3b TrigGlobEffCorrExample3c 
TrigGlobEffCorrExample3d TrigGlobEffCorrExample3e
TrigGlobEffCorrExample4 TrigGlobEffCorrExample5a TrigGlobEffCorrExample5b)
   atlas_add_executable( ${example}
      SOURCES examples/${example}.cxx
	  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
	  LINK_LIBRARIES TrigGlobalEfficiencyCorrectionLib ${pool_lib}
   )
endforeach()

atlas_add_executable( TrigGlobEffCorrExample6
   SOURCES examples/TrigGlobEffCorrExample6.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES TrigGlobalEfficiencyCorrectionLib ${pool_lib}
)

atlas_add_executable( TrigGlobEffCorrValidation
   SOURCES util/TrigGlobEffCorrValidation.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES TrigGlobalEfficiencyCorrectionLib xAODRootAccess
)

atlas_add_test(UnitTests SCRIPT util/unit_tests.sh)

atlas_install_data( data/*.cfg )
