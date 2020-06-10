# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package's name.
atlas_subdir( AthExOnnxRuntime )

# External dependencies.
find_package( onnxruntime )

# Component(s) in the package.
atlas_add_library( AthExOnnxRuntimeLib
   AthExOnnxRuntime/*.h
   Root/*.cxx
   INTERFACE
   PUBLIC_HEADERS AthExOnnxRuntime
   INCLUDE_DIRS ${ONNXRUNTIME_INCLUDE_DIRS}
   LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} AnaAlgorithmLib)

atlas_add_component( AthExOnnxRuntime
   src/components/*.cxx
   INCLUDE_DIRS ${ONNXRUNTIME_INCLUDE_DIRS}
   LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} AthExOnnxRuntimeLib AthenaBaseComps GaudiKernel PathResolver AnaAlgorithmLib)

# Install files from the package.
atlas_install_joboptions( share/*.py )

