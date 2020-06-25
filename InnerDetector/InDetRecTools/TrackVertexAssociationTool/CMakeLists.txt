# $Id: CMakeLists.txt 787108 2016-11-29 14:45:15Z krasznaa $
################################################################################
# Package: TrackVertexAssociationTool
################################################################################

# Declare the package name:
atlas_subdir( TrackVertexAssociationTool )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Control/AthToolSupport/AsgTools
   Control/AthToolSupport/AsgDataHandles
   Event/xAOD/xAODCore
   Event/xAOD/xAODTracking
   PRIVATE
   Event/xAOD/xAODEventInfo
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )

# Libraries in the package:
atlas_add_library( TrackVertexAssociationToolLib
   Root/*.cxx
   PUBLIC_HEADERS TrackVertexAssociationTool
   LINK_LIBRARIES AthContainers AthLinks AsgTools xAODCore xAODTracking AsgDataHandlesLib
   PRIVATE_LINK_LIBRARIES xAODEventInfo )

if( NOT XAOD_STANDALONE )
   atlas_add_component( TrackVertexAssociationTool
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODTracking
      TrackVertexAssociationToolLib )
endif()

atlas_add_dictionary( TrackVertexAssociationToolDict
   TrackVertexAssociationTool/TrackVertexAssociationToolDict.h
   TrackVertexAssociationTool/selection.xml
   LINK_LIBRARIES TrackVertexAssociationToolLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( TrackVertexAssoValidator
      util/TrackVertexAssoValidator.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODTracking
      TrackVertexAssociationToolLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
