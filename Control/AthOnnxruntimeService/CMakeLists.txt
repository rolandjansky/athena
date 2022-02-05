# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package's name.
atlas_subdir( AthOnnxruntimeService )

# External dependencies.
find_package( onnxruntime )

# Component(s) in the package.
atlas_add_library( AthOnnxruntimeServiceLib
   AthOnnxruntimeService/*.h Root/*.cxx
   PUBLIC_HEADERS AthOnnxruntimeService
   INCLUDE_DIRS ${ONNXRUNTIME_INCLUDE_DIRS}
   LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} AsgServicesLib)

if (XAOD_STANDALONE)
atlas_add_dictionary( AthOnnxruntimeServiceDict
   AthOnnxruntimeService/AthOnnxruntimeServiceDict.h
   AthOnnxruntimeService/selection.xml
   LINK_LIBRARIES AthOnnxruntimeServiceLib )
endif ()

if (NOT XAOD_STANDALONE)
  atlas_add_component( AthOnnxruntimeService
     src/*.h src/*.cxx src/components/*.cxx
     INCLUDE_DIRS ${ONNXRUNTIME_INCLUDE_DIRS}
     LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} AthOnnxruntimeServiceLib AthenaBaseComps GaudiKernel AsgServicesLib)
endif ()


