# $Id: CMakeLists.txt 777177 2016-10-07 16:09:43Z krasznaa $
################################################################################
# Package: PileupReweighting
################################################################################

# Declare the package name:
atlas_subdir( PileupReweighting )

# Extra dependencies based on the environment:
set( extra_dep )
if( XAOD_STANDALONE )
   set( extra_dep PhysicsAnalysis/AnalysisCommon/ReweightUtils )
else()
   set( extra_Dep GaudiKernel Control/AthenaBaseComps )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
   Trigger/TrigEvent/TrigDecisionInterface
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   DataQuality/GoodRunsLists
   PRIVATE
   Event/xAOD/xAODEventInfo
   Tools/PathResolver
   ${extra_dep} )

# External dependencies:
find_package( ROOT COMPONENTS MathCore RIO Core Tree Hist )

# Libraries in the package:
atlas_add_root_dictionary( PileupReweightingLib PileupReweightingDictSource
   ROOT_HEADERS PileupReweighting/TPileupReweighting.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_libs ReweightUtilsLib )
endif()

atlas_add_library( PileupReweightingLib
   PileupReweighting/*.h Root/*.cxx ${PileupReweightingDictSource}
   PUBLIC_HEADERS PileupReweighting
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools PATInterfaces AsgAnalysisInterfaces
   TrigDecisionInterface GoodRunsListsLib
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver ${extra_libs} )

if( NOT XAOD_STANDALONE )
   atlas_add_component( PileupReweighting
      src/components/*.cxx
      LINK_LIBRARIES AsgAnalysisInterfaces AthenaBaseComps xAODEventInfo
      PATInterfaces GaudiKernel PileupReweightingLib )
endif()

atlas_add_dictionary( PileupReweightingDict
   PileupReweighting/PileupReweightingDict.h
   PileupReweighting/selection.xml
   LINK_LIBRARIES PileupReweightingLib )

# Executable(s) in the package:
macro( _add_exec name )
   atlas_add_executable( ${name}
      src/${name}.C
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} PileupReweightingLib )
endmacro( _add_exec )

_add_exec( testPRW )
_add_exec( testPRWTool )
_add_exec( checkPRWConfigFile )

# Test(s) in the package:
atlas_add_test( ut_PRWExample_test
   SOURCES test/ut_PRWExample_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PileupReweightingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.root )
