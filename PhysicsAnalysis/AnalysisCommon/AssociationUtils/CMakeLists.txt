# $Id: CMakeLists.txt 793296 2017-01-21 01:09:39Z sfarrell $

# The name of the package:
atlas_subdir( AssociationUtils )

# Extra dependencies based on the build environment:
set( extra_dep )
if( XAOD_STANDALONE )
   set( extra_dep Control/xAODRootAccess )
else()
   set( extra_dep Control/AthenaBaseComps
      GaudiKernel )
endif()

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODBase
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODJet
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTau
   Event/xAOD/xAODTracking
   PRIVATE
   Control/CxxUtils
   Event/FourMomUtils
   Event/xAOD/xAODEventInfo
   ${extra_dep} )

# External(s):
find_package( ROOT COMPONENTS Core )
find_package( Boost COMPONENTS program_options )
find_package( GTest )

# Libraries in the package:
atlas_add_root_dictionary( AssociationUtilsLib AssociationUtilsLibDict
   ROOT_HEADERS AssociationUtils/OverlapRemovalTool.h
   AssociationUtils/BaseOverlapTool.h AssociationUtils/DeltaROverlapTool.h
   AssociationUtils/EleEleOverlapTool.h
   AssociationUtils/EleMuSharedTrkOverlapTool.h
   AssociationUtils/EleJetOverlapTool.h AssociationUtils/MuJetOverlapTool.h
   AssociationUtils/AltMuJetOverlapTool.h
   AssociationUtils/TauLooseEleOverlapTool.h
   AssociationUtils/TauLooseMuOverlapTool.h AssociationUtils/TauJetOverlapTool.h
   AssociationUtils/TauAntiTauJetOverlapTool.h
   AssociationUtils/ObjLinkOverlapTool.h AssociationUtils/ToolBox.h
   AssociationUtils/BJetHelper.h AssociationUtils/DeltaRMatcher.h
   AssociationUtils/MuJetGhostDRMatcher.h
   AssociationUtils/OverlapDecorationHelper.h
   AssociationUtils/OverlapLinkHelper.h
   AssociationUtils/IOverlapRemovalToolLegacy.h
   AssociationUtils/OverlapRemovalToolLegacy.h
   Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( AssociationUtilsLib
   AssociationUtils/*.h Root/*.cxx ${AssociationUtilsLibDict}
   PUBLIC_HEADERS AssociationUtils
   INCLUDE_DIRS ${ROOT_ICNLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODBase xAODEgamma xAODJet
   xAODMuon xAODTau xAODTracking AthContainers AthLinks
   PRIVATE_LINK_LIBRARIES CxxUtils FourMomUtils )

if( NOT XAOD_STANDALONE )
   atlas_add_component( AssociationUtils
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps xAODBase GaudiKernel AssociationUtilsLib )
endif()

# Executables in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( OverlapRemovalTester
      util/OverlapRemovalTester.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} xAODRootAccess
      xAODEventInfo xAODEgamma xAODMuon xAODJet xAODTau xAODEgamma
      AssociationUtilsLib )

   atlas_add_executable( OverlapRemovalTesterLegacy
      util/OverlapRemovalTesterLegacy.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODEgamma
      xAODMuon xAODJet xAODTau xAODEgamma AssociationUtilsLib )
endif()

# Tests in the package:
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_libs xAODRootAccess )
endif()

# Helper function for defining google tests
macro( _add_gtest name )
   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgTestingLib AssociationUtilsLib
      ${extra_libs} )
endmacro( _add_gtest )

# Define the google unit tests.
# Disabling some which don't work in Athena.
foreach( test gt_AnaToolHandle_test ) # gt_OverlapRemovalInit_test gt_toolbox_test
   _add_gtest( ${test} )
endforeach()

if( XAOD_STANDALONE )
   atlas_add_test( ut_assocUtilsDict SCRIPT test/ut_assocUtilsDict.py )
   atlas_add_test( ut_ort_tester SCRIPT test/ut_ort_tester.sh )
   atlas_add_test( ut_ort_tester_legacy SCRIPT test/ut_ort_tester_legacy.sh )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
