# $Id: CMakeLists.txt 782349 2016-11-04 11:47:49Z krasznaa $
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
  Control/AthContainers
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

# Install files from the package:
atlas_install_python_modules( python/*.py )
