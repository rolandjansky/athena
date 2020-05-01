# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( TrackingAnalysisAlgorithms )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   PRIVATE
   Event/xAOD/xAODTracking )

# Components in the package:
atlas_add_library( TrackingAnalysisAlgorithmsLib
   TrackingAnalysisAlgorithms/*.h Root/*.cxx
   PUBLIC_HEADERS TrackingAnalysisAlgorithms
   LINK_LIBRARIES AnaAlgorithmLib
   PRIVATE_LINK_LIBRARIES xAODTracking )

atlas_add_dictionary( TrackingAnalysisAlgorithmsDict
   TrackingAnalysisAlgorithms/TrackingAnalysisAlgorithmsDict.h
   TrackingAnalysisAlgorithms/selection.xml
   LINK_LIBRARIES TrackingAnalysisAlgorithmsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( TrackingAnalysisAlgorithms
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel TrackingAnalysisAlgorithmsLib )
endif()
