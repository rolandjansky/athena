################################################################################
# Package: PFlowUtils
################################################################################

# Declare the package name:
atlas_subdir( PFlowUtils )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODPFlow
  PRIVATE
  Event/xAOD/xAODJet
  ${extra_deps} )

# Component(s) in the package:
atlas_add_library( PFlowUtilsLib
   PFlowUtils/*.h Root/*.cxx
   PUBLIC_HEADERS PFlowUtils
   LINK_LIBRARIES AsgTools xAODPFlow )

if( NOT XAOD_STANDALONE )
   atlas_add_component( PFlowUtils
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps xAODJet xAODPFlow GaudiKernel
      PFlowUtilsLib )
endif()

atlas_add_dictionary( PFlowUtilsDict
   PFlowUtils/PFlowUtilsDict.h PFlowUtils/selection.xml
   LINK_LIBRARIES PFlowUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Test(s) in the package:
atlas_add_test( toolhandle_test
   SOURCES test/toolhandle_test.cxx
   LINK_LIBRARIES AsgTools PFlowUtilsLib )
