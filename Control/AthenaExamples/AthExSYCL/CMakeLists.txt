# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( AthExSYCL )

# If SYCL is not available, don't do anything for this package.
find_package( SYCL )
if( NOT SYCL_FOUND )
   message( STATUS "SYCL compilation is not available, AthExSYCL is not built" )
   return()
endif()

# Add a component library that has some SYCL code in it.
atlas_add_component( AthExSYCL
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel )
atlas_setup_sycl_target( AthExSYCL )

# Install extra files from the package.
atlas_install_joboptions( share/*.py )
