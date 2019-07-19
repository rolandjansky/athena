################################################################################
# Package: FakeBkgTools
################################################################################

atlas_subdir( FakeBkgTools )

set( extra_dep )
if( NOT XAOD_STANDALONE )
  set( extra_dep GaudiKernel PhysicsAnalysis/POOLRootAccess )
endif()

atlas_depends_on_subdirs(
  PUBLIC
  PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
  Control/AthToolSupport/AsgTools
  PhysicsAnalysis/AnalysisCommon/PATInterfaces
  Event/xAOD/xAODBase
  PRIVATE
  Event/xAOD/xAODEgamma
  Event/xAOD/xAODMuon
  Event/xAOD/xAODEventInfo
  Tools/PathResolver
  PhysicsAnalysis/Algorithms/SelectionHelpers
  Control/CxxUtils
  Event/xAOD/xAODCore
  ${extra_dep} )

find_package( Boost )
find_package( ROOT COMPONENTS Core Tree RIO MathCore Gpad Hist MathMore Minuit Matrix )

atlas_add_root_dictionary( FakeBkgToolsLib FakeBkgToolsDictSource
   ROOT_HEADERS FakeBkgTools/TMinuit_LHMM.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( FakeBkgToolsLib
  FakeBkgTools/*.h Root/*.h Root/*.cxx  ${FakeBkgToolsDictSource}
  PUBLIC_HEADERS FakeBkgTools
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES AsgAnalysisInterfaces AsgTools xAODBase PATInterfaces
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils xAODEgamma xAODMuon PathResolver SelectionHelpersLib
  xAODCore xAODEventInfo )

if( NOT XAOD_STANDALONE )
  atlas_add_component( FakeBkgTools
    src/components/*.cxx
    LINK_LIBRARIES GaudiKernel FakeBkgToolsLib )
endif()

atlas_add_dictionary( FakeBkgToolsDict
   FakeBkgTools/FakeBkgToolsDict.h
   FakeBkgTools/selection.xml
   LINK_LIBRARIES FakeBkgToolsLib
)

set( pool_lib )
if( NOT XAOD_STANDALONE )
  set( pool_lib POOLRootAccessLib )
endif()

foreach( exec fbtNtupleTester fbtTestBasics fbtTestToyMC )
  atlas_add_executable( ${exec} util/${exec}.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
      LINK_LIBRARIES ${ROOT_LIBRARIES} FakeBkgToolsLib xAODEgamma xAODMuon ${pool_lib})
endforeach()

atlas_add_test( ut_fbtTestBasics
   SCRIPT fbtTestBasics
)

atlas_install_data( data/* )

