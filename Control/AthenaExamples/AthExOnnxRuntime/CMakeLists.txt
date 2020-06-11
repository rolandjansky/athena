# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package's name.
atlas_subdir( AthExOnnxRuntime )

# External dependencies.
find_package( onnxruntime )

# Component(s) in the package.
atlas_add_library( AthExOnnxRuntimeLib
   AthExOnnxRuntime/*.h
   Root/*.cxx
   PUBLIC_HEADERS AthExOnnxRuntime
   INCLUDE_DIRS ${ONNXRUNTIME_INCLUDE_DIRS} PathResolver
   LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} AnaAlgorithmLib PathResolver)

if (XAOD_STANDALONE)
 # Add the dictionary (for AnalysisBase only):
 atlas_add_dictionary (AthExOnnxRuntimeDict
  AthExOnnxRuntime/AthExOnnxRuntimeDict.h
  AthExOnnxRuntime/selection.xml
  LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} AthExOnnxRuntimeLib)
endif ()

if (NOT XAOD_STANDALONE)
  # Add a component library for AthAnalysis only:
  atlas_add_component (AthExOnnxRuntime
    src/components/*.cxx
    LINK_LIBRARIES AthExOnnxRuntimeLib)
endif ()

# Install files from the package.
atlas_install_joboptions( share/*.py )

