################################################################################
# Package: TrigGlobalEfficiencyCorrection
################################################################################

atlas_subdir( TrigGlobalEfficiencyCorrection )

set( athena_subdirs )
if( NOT XAOD_STANDALONE )
  set( athena_subdirs GaudiKernel PhysicsAnalysis/POOLRootAccess Control/AthAnalysisBaseComps )
endif()

atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  PhysicsAnalysis/AnalysisCommon/PATInterfaces
  PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
  PhysicsAnalysis/Interfaces/MuonAnalysisInterfaces
  PhysicsAnalysis/Interfaces/TriggerAnalysisInterfaces
  Event/xAOD/xAODEgamma
  Event/xAOD/xAODMuon
  PRIVATE
  Event/xAOD/xAODEventInfo
  PhysicsAnalysis/AnalysisCommon/PATCore
  Tools/PathResolver
  ${athena_subdirs}
)

find_package( Boost )
find_package( ROOT COMPONENTS RIO )

atlas_add_library( TrigGlobalEfficiencyCorrectionLib
  TrigGlobalEfficiencyCorrection/*.h Root/*.cxx
  #PUBLIC_HEADERS TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionTool.h
  PUBLIC_HEADERS TrigGlobalEfficiencyCorrection
  PRIVATE_INCLUDE_DIRS ${BOOST_INCLUDE_DIRS}
  LINK_LIBRARIES AsgTools xAODEgamma xAODMuon EgammaAnalysisInterfacesLib MuonAnalysisInterfacesLib TriggerAnalysisInterfacesLib xAODEventInfo PathResolver
  PRIVATE_LINK_LIBRARIES ${BOOST_LIBRARIES}
)

atlas_add_dictionary( TrigGlobalEfficiencyCorrectionDict
  TrigGlobalEfficiencyCorrection/TrigGlobalEfficiencyCorrectionDict.h
  TrigGlobalEfficiencyCorrection/selection.xml
  LINK_LIBRARIES TrigGlobalEfficiencyCorrectionLib
)

foreach(example TrigGlobEffCorrExample0 TrigGlobEffCorrExample1
TrigGlobEffCorrExample2 TrigGlobEffCorrExample3a TrigGlobEffCorrExample3b
TrigGlobEffCorrExample3c TrigGlobEffCorrExample3d TrigGlobEffCorrExample3e
TrigGlobEffCorrExample4)
    atlas_add_executable( ${example}
        SOURCES examples/${example}.cxx
	    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
	    LINK_LIBRARIES TrigGlobalEfficiencyCorrectionLib 
    )
endforeach()

atlas_add_executable( TrigGlobEffCorrValidation
        SOURCES util/TrigGlobEffCorrValidation.cxx
            INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
            LINK_LIBRARIES TrigGlobalEfficiencyCorrectionLib
    )

atlas_add_test(UnitTests SCRIPT util/unit_tests.sh)

atlas_install_data( data/*.cfg )
