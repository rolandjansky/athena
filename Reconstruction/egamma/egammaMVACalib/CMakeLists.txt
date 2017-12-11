# $Id: CMakeLists.txt 786399 2016-11-25 08:24:32Z turra $
################################################################################
# Package: egammaMVACalib
################################################################################

# Declare the package name:
atlas_subdir( egammaMVACalib )

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
   PRIVATE
   Reconstruction/MVAUtils
   Control/CxxUtils
   Tools/PathResolver
   ${extra_dep} )

# External dependencies:
find_package( ROOT COMPONENTS Tree TreePlayer TMVA XMLIO Core MathCore Hist
   RIO )

# Component(s) in the package:
atlas_add_library( egammaMVACalibLib
   egammaMVACalib/*.h Root/*.cxx
   PUBLIC_HEADERS egammaMVACalib
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma
   xAODTracking
   PRIVATE_LINK_LIBRARIES CxxUtils PathResolver MVAUtils )

if( NOT XAOD_STANDALONE )
   atlas_add_component( egammaMVACalib
      src/components/*.cxx
      LINK_LIBRARIES GaudiKernel egammaMVACalibLib )
endif()

atlas_add_dictionary( egammaMVACalibDict
   egammaMVACalib/egammaMVACalibDict.h
   egammaMVACalib/selection.xml
   LINK_LIBRARIES egammaMVACalibLib )

atlas_add_executable( egammaMVACalib_mem_test_initialize
   util/mem_test_initialize.cxx
   LINK_LIBRARIES egammaMVACalibLib )

if( XAOD_STANDALONE )
   atlas_add_executable( testClusterMVACalib
      util/testClusterMVACalib.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEventInfo xAODEgamma xAODCaloEvent
      xAODCore AsgTools egammaMVACalibLib )
endif()

# Test(s) in the package:
atlas_add_test( mem_test SCRIPT test/mem_test.py
                EXTRA_PATTERNS " GB$" )
if( XAOD_STANDALONE )
# Disable until we have reasonable input/to check 
# against 
#  atlas_add_test( ut_test SCRIPT test/ut_test.py)
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
