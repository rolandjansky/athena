# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelRTT )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist RIO Gpad )

# Component(s) in the package:
atlas_add_library( PixelValidation
   PixelRTT/*.h src/*.cxx
   PUBLIC_HEADERS PixelRTT
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils PixelCalibAlgsLib
   PRIVATE_LINK_LIBRARIES PixelConditionsData PathResolver )

atlas_add_executable( doPixelValidation
   Application/doPixelValidation.cxx
   LINK_LIBRARIES CxxUtils PixelValidation )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/*.* )
