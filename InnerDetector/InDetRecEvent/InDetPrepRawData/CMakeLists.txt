# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetPrepRawData )

# Component(s) in the package:
atlas_add_library( InDetPrepRawData
                   src/*.cxx
                   PUBLIC_HEADERS InDetPrepRawData
                   LINK_LIBRARIES AthLinks AthenaKernel GeoPrimitives Identifier EventContainers EventPrimitives GaudiKernel InDetReadoutGeometry TRT_ReadoutGeometry TrkSurfaces TrkEventPrimitives TrkPrepRawData InDetRawData )

atlas_add_dictionary( InDetPrepRawDataDict
                      InDetPrepRawData/InDetPrepRawDataDict.h
                      InDetPrepRawData/selection.xml
                      LINK_LIBRARIES AthLinks Identifier InDetPrepRawData )
