# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package's name.
atlas_subdir( AthOnnxruntimeUtils )

# Component(s) in the package.
atlas_add_library( AthOnnxruntimeUtilsLib
   INTERFACE
   PUBLIC_HEADERS AthOnnxruntimeUtils
   INCLUDE_DIRS ${ONNXRUNTIME_INCLUDE_DIRS}
   LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} AthOnnxruntimeServiceLib AthenaKernel GaudiKernel AsgServicesLib )

