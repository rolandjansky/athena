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
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   PRIVATE
   Control/AthToolSupport/AsgTesting
   Control/CxxUtils
   Event/FourMomUtils
   Event/xAOD/xAODEventInfo
   ${extra_dep} )

# External(s):
find_package( ROOT COMPONENTS Core )
find_package( Boost COMPONENTS program_options )
find_package( GTest )

# Libraries in the package:
atlas_add_library( AssociationUtilsLib
   AssociationUtils/*.h Root/*.cxx
   PUBLIC_HEADERS AssociationUtils
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODBase xAODEgamma xAODJet
   xAODMuon xAODTau xAODTracking AthContainers AthLinks AnaAlgorithmLib
   PRIVATE_LINK_LIBRARIES CxxUtils FourMomUtils )

atlas_add_dictionary( AssociationUtilsDict
    AssociationUtils/AssociationUtilsDict.h
    AssociationUtils/selection.xml
    LINK_LIBRARIES AssociationUtilsLib )

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
      LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AssociationUtilsLib
      ${extra_libs} )
endmacro( _add_gtest )

# Define the google unit tests.
# Disabling some which don't work in Athena.
foreach( test gt_AnaToolHandle_test ) # gt_OverlapRemovalInit_test gt_toolbox_test
   _add_gtest( ${test} LINK_LIBRARIES AsgTestingLib )
endforeach()

if( XAOD_STANDALONE )
   atlas_add_test( ut_assocUtilsDict SCRIPT test/ut_assocUtilsDict.py )
   atlas_add_test( ut_ort_tester SCRIPT test/ut_ort_tester.sh )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
