# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package's name.
atlas_subdir( AthOnnxRuntimeBJT )

# External dependencies.
#find_package( onnxruntime )
find_package( onnxruntime REQUIRED )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
#   Tools/PathResolver
   Control/xAODRootAccess
   Event/xAOD/xAODJet
)  

find_package( ROOT COMPONENTS Core RIO Hist Tree )
# Build a dictionary for the library:
atlas_add_root_dictionary( AthOnnxRuntimeBJTLib _dictionarySource
   ROOT_HEADERS Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Component(s) in the package.
atlas_add_library( AthOnnxRuntimeBJTLib
   AthOnnxRuntimeBJT/*.h Root/*.cxx ${_dictionarySource}
   PUBLIC_HEADERS AthOnnxRuntimeBJT
   INCLUDE_DIRS PRIVATE ${ONNXRUNTIME_INCLUDE_DIRS} PathResolver
   LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} AnaAlgorithmLib PathResolver ${ROOT_LIBRARIES} xAODRootAccess xAODJet)

if (XAOD_STANDALONE)
 # Add the dictionary (for AnalysisBase only):
 atlas_add_dictionary (AthOnnxRuntimeBJTDict
  AthOnnxRuntimeBJT/JSSMLToolDict.h
  AthOnnxRuntimeBJT/selection.xml
  LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} AthOnnxRuntimeBJTLib)
endif ()

if (NOT XAOD_STANDALONE)
  # Add a component library for AthAnalysis only:
  atlas_add_component (AthOnnxRuntimeBJT
    src/components/*.cxx
    LINK_LIBRARIES AthOnnxRuntimeBJTLib)
endif ()

# Install files from the package.
atlas_install_joboptions( share/*.py )
atlas_install_generic( data/*
   DESTINATION data PKGNAME_SUBDIR )
