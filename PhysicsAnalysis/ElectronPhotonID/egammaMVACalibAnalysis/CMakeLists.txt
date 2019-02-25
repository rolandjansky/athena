################################################################################
# Package: egammaMVACalibAnalysis
################################################################################

# Declare the package name:
atlas_subdir( egammaMVACalibAnalysis )

# Extra dependencies for Athena capable builds:
set( extra_dep )
if( XAOD_STANDALONE )
   set( extra_dep Control/xAODRootAccess
      Event/xAOD/xAODEventInfo
      Event/xAOD/xAODCore )
else()
   set( extra_dep GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODTracking
   Reconstruction/egamma/egammaMVACalib
   PRIVATE
   Reconstruction/MVAUtils
   Control/CxxUtils
   Tools/PathResolver
   ${extra_dep} )

# External dependencies:
find_package( ROOT COMPONENTS Tree TreePlayer TMVA XMLIO Core MathCore Hist RIO )

# Component(s) in the package:
atlas_add_library( egammaMVACalibAnalysisLib
   egammaMVACalibAnalysis/*.h Root/*.cxx
   PUBLIC_HEADERS egammaMVACalibAnalysis
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma
   xAODTracking egammaMVACalibLib
   PRIVATE_LINK_LIBRARIES CxxUtils PathResolver MVAUtils )

if( NOT XAOD_STANDALONE )
   atlas_add_component( egammaMVACalibAnalysis
      src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel egammaMVACalibAnalysisLib)
endif()

atlas_add_dictionary( egammaMVACalibAnalysisDict
	egammaMVACalibAnalysis/egammaMVACalibAnalysisDict.h
	egammaMVACalibAnalysis/selection.xml
	LINK_LIBRARIES egammaMVACalibAnalysisLib )

atlas_add_executable( egammaMVACalibAnalysis_mem_test_initialize
   util/mem_test_initialize.cxx
   LINK_LIBRARIES egammaMVACalibAnalysisLib )

if( XAOD_STANDALONE )
   atlas_add_executable( testClusterMVACalib
      util/testClusterMVACalib.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEventInfo xAODEgamma xAODCaloEvent
      xAODCore AsgTools egammaMVACalibAnalysisLib )
endif()

# Test(s) in the package:
# Not working now because egammaMVACalibDict was removed.
#atlas_add_test( mem_test SCRIPT test/mem_test.py
#                PROPERTIES TIMEOUT 300
#                EXTRA_PATTERNS " GB$" )

# Install files from the package:
atlas_install_python_modules( python/*.py )
