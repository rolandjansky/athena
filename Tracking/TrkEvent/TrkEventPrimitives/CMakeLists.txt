# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkEventPrimitives )

# Component(s) in the package:
atlas_add_library( TrkEventPrimitives
                   src/*.cxx
                   PUBLIC_HEADERS TrkEventPrimitives
                   LINK_LIBRARIES GeoPrimitives EventPrimitives GaudiKernel )

atlas_add_dictionary( TrkEventPrimitivesDict
                      TrkEventPrimitives/TrkEventPrimitivesDict.h
                      TrkEventPrimitives/selection.xml
                      LINK_LIBRARIES TrkEventPrimitives )
