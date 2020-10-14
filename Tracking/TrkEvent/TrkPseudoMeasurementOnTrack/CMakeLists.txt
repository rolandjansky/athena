# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkPseudoMeasurementOnTrack )

# Component(s) in the package:
atlas_add_library( TrkPseudoMeasurementOnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkPseudoMeasurementOnTrack
                   LINK_LIBRARIES GeoPrimitives EventPrimitives TrkSurfaces TrkMeasurementBase
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( TrkPseudoMeasurementOnTrackDict
                      TrkPseudoMeasurementOnTrack/TrkPseudoMeasurementOnTrackDict.h
                      TrkPseudoMeasurementOnTrack/selection.xml
                      LINK_LIBRARIES TrkPseudoMeasurementOnTrack )
