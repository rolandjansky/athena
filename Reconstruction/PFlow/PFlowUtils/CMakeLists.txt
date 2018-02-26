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

find_package( ROOT COMPONENTS Core RIO Hist Tree PyROOT )

# Generate a dictionary:
atlas_add_root_dictionary( PFlowUtilsLib PFlowUtilsDictSource
                           ROOT_HEADERS PFlowUtils/*.h Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT 
			   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODPFlow )

# Component(s) in the package:
atlas_add_library( PFlowUtilsLib
   PFlowUtils/*.h Root/*.h Root/*.cxx ${PFlowUtilsDictSource}
   PUBLIC_HEADERS PFlowUtils
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODPFlow )

if( NOT XAOD_STANDALONE )
   atlas_add_component( PFlowUtils
      PFlowUtils/*.h Root/*.cxx src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps xAODJet xAODPFlow GaudiKernel
      PFlowUtilsLib )
endif()

atlas_add_dictionary( PFlowUtilsDict
   PFlowUtils/PFlowUtilsDict.h PFlowUtils/selection.xml
   LINK_LIBRARIES PFlowUtilsLib )

# Install files from the package:
atlas_install_headers( PFlowUtils )
atlas_install_python_modules( python/*.py )

# Test(s) in the package:
atlas_add_test( toolhandle_test
   SOURCES test/toolhandle_test.cxx
   LINK_LIBRARIES AsgTools PFlowUtilsLib )
