# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set up the package name.
atlas_subdir( AthExHIP )

# Check if HIP compilation is available.
if( NOT CMAKE_HIP_COMPILER )
   message( STATUS "HIP not available, not building package" )
   return()
endif()

# Set up the build of the example library.
atlas_add_component( AthExHIP
   src/*.h src/*.cxx src/*.hip src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel )
set_target_properties( AthExHIP PROPERTIES
   LINKER_LANGUAGE HIP )

# Install files from the package.
atlas_install_joboptions( share/*.py )
