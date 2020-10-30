# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name: 
atlas_subdir( TrkSurfaces )

# Component(s) in the package:
atlas_add_library( TrkSurfaces
   TrkSurfaces/*.h src/*.cxx
   PUBLIC_HEADERS TrkSurfaces
   LINK_LIBRARIES AthenaKernel AthContainers CxxUtils GeoPrimitives Identifier EventPrimitives TrkParametersBase
   GaudiKernel TrkDetDescrUtils TrkDetElementBase TrkEventPrimitives )

atlas_add_dictionary( TrkSurfacesDict
   TrkSurfaces/TrkSurfacesDict.h
   TrkSurfaces/selection.xml
   LINK_LIBRARIES TrkSurfaces )

atlas_add_test( CylinderSurface_test
                SOURCES test/CylinderSurface_test.cxx
                LINK_LIBRARIES TrkEventPrimitives TrkSurfaces TestTools )
