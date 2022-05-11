# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the name of the package:
atlas_subdir( InDetTrackSystematicsTools )

# External(s):
find_package( ROOT COMPONENTS Core Hist RIO MathCore )

# Libraries in the package:
atlas_add_library( InDetTrackSystematicsToolsLib
   InDetTrackSystematicsTools/*.h Root/*.cxx
   PUBLIC_HEADERS InDetTrackSystematicsTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools PATInterfaces xAODTracking
   xAODTruth xAODJet
   PRIVATE_LINK_LIBRARIES FourMomUtils xAODEventInfo PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( InDetTrackSystematicsTools
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AsgTools xAODCore
      xAODTracking InDetTrackSystematicsToolsLib )
endif()

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( InDetTrackSystsToolTester
      util/InDetTrackSystsToolTester.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODTracking
      xAODCore InDetTrackSystematicsToolsLib )
endif()

atlas_add_executable( graph_to_function
   util/graph_to_function.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# This will allow us to put files into data/ to test new calibration files
# Uncomment this line after putting test files into data/, then run "make" again to install.
# atlas_install_data( data/*.root )
