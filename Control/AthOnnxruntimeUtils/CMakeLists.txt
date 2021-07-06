# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package's name.
atlas_subdir( AthOnnxruntimeUtils )

# Component(s) in the package.
atlas_add_library( AthOnnxruntimeUtilsLib
   INTERFACE
   PUBLIC_HEADERS AthOnnxruntimeUtils
   LINK_LIBRARIES AthenaKernel GaudiKernel )

