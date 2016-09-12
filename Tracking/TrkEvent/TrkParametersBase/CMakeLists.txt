# $Id: CMakeLists.txt 746560 2016-05-12 12:13:16Z krasznaa $
################################################################################
# Package: TrkParametersBase
################################################################################

# Declare the package name:
atlas_subdir( TrkParametersBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
   DetectorDescription/GeoPrimitives
   Event/EventPrimitives
   GaudiKernel
   Tracking/TrkEvent/TrkEventPrimitives )

# Declare an interface library for the headers of the package:
atlas_add_library( TrkParametersBase
   TrkParametersBase/*.h TrkParametersBase/*.icc
   INTERFACE
   PUBLIC_HEADERS TrkParametersBase
   LINK_LIBRARIES GeoPrimitives EventPrimitives TrkEventPrimitives GaudiKernel )
