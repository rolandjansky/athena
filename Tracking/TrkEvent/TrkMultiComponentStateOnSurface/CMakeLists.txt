# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkMultiComponentStateOnSurface )

# Component(s) in the package:
atlas_add_library( TrkMultiComponentStateOnSurface
                   src/*.cxx
                   PUBLIC_HEADERS TrkMultiComponentStateOnSurface
                   LINK_LIBRARIES TrkEventPrimitives TrkParameters TrkTrack AthContainers
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrkSurfaces TrkMaterialOnTrack TrkMeasurementBase )

atlas_add_dictionary( TrkMultiComponentStateOnSurfaceDict
                      TrkMultiComponentStateOnSurface/TrkMultiComponentStateOnSurfaceDict.h
                      TrkMultiComponentStateOnSurface/selection.xml
                      LINK_LIBRARIES TrkMultiComponentStateOnSurface)

