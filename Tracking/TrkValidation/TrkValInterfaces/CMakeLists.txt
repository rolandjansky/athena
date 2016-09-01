# $Id: CMakeLists.txt 771258 2016-09-01 12:03:17Z aalonso $
################################################################################
# Package: TrkValInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkValInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   DetectorDescription/GeoPrimitives
   GaudiKernel
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/TrkParameters
   Tracking/TrkEvent/TrkTrack)
 #  Tracking/TrkEvent/TrkTrack
 #  Tracking/TrkFitter/TrkFitterUtils )

# Component(s) in the package:
atlas_add_library( TrkValInterfaces
   TrkValInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS TrkValInterfaces
   LINK_LIBRARIES GeoPrimitives GaudiKernel TrkEventPrimitives TrkParameters
   TrkTrack )
