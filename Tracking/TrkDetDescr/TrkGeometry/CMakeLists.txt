# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkGeometry )

# Component(s) in the package:
atlas_add_library( TrkGeometry
                   src/*.cxx
                   PUBLIC_HEADERS TrkGeometry
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel AthContainers CxxUtils GeoPrimitives GaudiKernel TrkDetDescrUtils TrkGeometrySurfaces TrkSurfaces TrkVolumes TrkEventPrimitives TrkNeutralParameters TrkParameters
                   PRIVATE_LINK_LIBRARIES EventPrimitives )

atlas_add_dictionary( TrkGeometryDict
                      TrkGeometry/TrkGeometryDict.h
                      TrkGeometry/selection.xml
                      LINK_LIBRARIES AthContainers TrkGeometry )
