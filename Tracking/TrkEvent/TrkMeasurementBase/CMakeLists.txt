# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkMeasurementBase )

# Component(s) in the package:
atlas_add_library( TrkMeasurementBase
                   PUBLIC_HEADERS TrkMeasurementBase
                   LINK_LIBRARIES AthContainers GeoPrimitives EventPrimitives GaudiKernel TrkEventPrimitives )

atlas_add_dictionary( TrkMeasurementBaseDict
                      TrkMeasurementBase/TrkMeasurementBaseDict.h
                      TrkMeasurementBase/selection.xml
                      LINK_LIBRARIES TrkMeasurementBase )
