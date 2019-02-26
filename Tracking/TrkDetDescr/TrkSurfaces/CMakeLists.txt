# $Id: CMakeLists.txt 746161 2016-05-11 12:24:58Z krasznaa $
################################################################################
# Package: TrkSurfaces
################################################################################

# Declare the package name: 
atlas_subdir( TrkSurfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/SGTools
   Control/AthContainers
   DetectorDescription/GeoPrimitives
   DetectorDescription/Identifier
   Event/EventPrimitives
   GaudiKernel
   Tracking/TrkDetDescr/TrkDetDescrUtils
   Tracking/TrkDetDescr/TrkDetElementBase
   Tracking/TrkEvent/TrkEventPrimitives
   Tracking/TrkEvent/TrkParametersBase
   PRIVATE
   AtlasTest/TestTools
   Control/CxxUtils )

# Component(s) in the package:
atlas_add_library( TrkSurfaces
   TrkSurfaces/*.h src/*.cxx
   PUBLIC_HEADERS TrkSurfaces
   LINK_LIBRARIES SGTools AthContainers GeoPrimitives Identifier EventPrimitives
   GaudiKernel TrkDetDescrUtils TrkDetElementBase TrkEventPrimitives
   PRIVATE_LINK_LIBRARIES CxxUtils )

atlas_add_dictionary( TrkSurfacesDict
   TrkSurfaces/TrkSurfacesDict.h
   TrkSurfaces/selection.xml
   LINK_LIBRARIES TrkSurfaces )


atlas_add_test( CylinderSurface_test
                SOURCES test/CylinderSurface_test.cxx
                LINK_LIBRARIES TrkSurfaces TestTools )

 
