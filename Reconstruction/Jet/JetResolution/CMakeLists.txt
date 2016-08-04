# $Id: CMakeLists.txt 766594 2016-08-04 23:13:11Z will $
################################################################################
# Package: JetResolution
################################################################################

# Declare the package name:
atlas_subdir( JetResolution )

# Extra dependencies, based on the build environment:
set( extra_deps )
set( extra_libs )
set( extra_srcs )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthAnalysisBaseComps
      Control/AthenaBaseComps GaudiKernel )
   set( extra_libs AthAnalysisBaseCompsLib AthenaBaseComps GaudiKernel )
   set( extra_srcs src/*.h src/*.cxx )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODJet
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PRIVATE
   Event/xAOD/xAODEventInfo
   Tools/PathResolver
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist Physics Tree TreePlayer )

# Component(s) in the package:
atlas_add_library( JetResolutionLib
   JetResolution/*.h Root/*.cxx ${extra_srcs}
   PUBLIC_HEADERS JetResolution
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODJet PATInterfaces
   PRIVATE_LINK_LIBRARIES xAODEventInfo PathResolver ${extra_libs} )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetResolution
      src/components/*.cxx
      LINK_LIBRARIES JetResolutionLib )
endif()

atlas_add_dictionary( JetResolutionDict
   JetResolution/JetResolutionDict.h
   JetResolution/selection.xml
   LINK_LIBRARIES JetResolutionLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
