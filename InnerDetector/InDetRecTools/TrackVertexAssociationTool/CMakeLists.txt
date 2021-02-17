# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrackVertexAssociationTool )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )
find_package( lwtnn REQUIRED )

# Libraries in the package:
atlas_add_library( TrackVertexAssociationToolLib
   Root/*.cxx
   PUBLIC_HEADERS TrackVertexAssociationTool
   INCLUDE_DIRS ${LWTNN_INCLUDE_DIRS}
   LINK_LIBRARIES AthContainers AthLinks AsgTools xAODCore xAODEventInfo xAODTracking AsgDataHandlesLib ${LWTNN_LIBRARIES}
   PRIVATE_LINK_LIBRARIES PathResolver )

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
