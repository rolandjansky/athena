# $Id: CMakeLists.txt 749910 2016-05-26 15:00:09Z krasznaa $
################################################################################
# Package: IRegionSelector
################################################################################

# Declare the package name:
atlas_subdir( IRegionSelector )

# Declare the package's dependencies. But only for an Athena build:
if( NOT XAOD_STANDALONE )
   atlas_depends_on_subdirs( PUBLIC
      Control/AthenaKernel
      DetectorDescription/Identifier
      GaudiKernel )
endif()

# Component(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_library( IRegionSelector
      IRegionSelector/*.h
      INTERFACE
      PUBLIC_HEADERS IRegionSelector )
else()
   atlas_add_library( IRegionSelector
      IRegionSelector/*.h
      INTERFACE
      PUBLIC_HEADERS IRegionSelector
      LINK_LIBRARIES AthenaKernel Identifier GaudiKernel )
endif()
