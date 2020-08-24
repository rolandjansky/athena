# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package's name.
atlas_subdir( AthExOnnxRuntime )

# External dependencies.
find_package( onnxruntime )

# Component(s) in the package.
atlas_add_library( AthExOnnxRuntimeLib
   AthExOnnxRuntime/*.h
   INTERFACE
   PUBLIC_HEADERS AthExOnnxRuntime
   INCLUDE_DIRS ${ONNXRUNTIME_INCLUDE_DIRS}
   LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} GaudiKernel )

atlas_add_component( AthExOnnxRuntime
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ONNXRUNTIME_INCLUDE_DIRS}
   LINK_LIBRARIES ${ONNXRUNTIME_LIBRARIES} AthExOnnxRuntimeLib AthenaBaseComps GaudiKernel PathResolver )

# Install files from the package.
atlas_install_joboptions( share/*.py )

# Set up tests for the package.
atlas_add_test( AthExOnnxRuntimeJob_serial
   SCRIPT athena.py AthExOnnxRuntime/AthExOnnxRuntime_jobOptions.py )

atlas_add_test( AthExOnnxRuntimeJob_mt
   SCRIPT athena.py --threads=2
          AthExOnnxRuntime/AthExOnnxRuntime_jobOptions.py )



