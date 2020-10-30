# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetEventCnvTools )

# Component(s) in the package:
atlas_add_component( InDetEventCnvTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AtlasDetDescr IdDictDetDescr Identifier InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry InDetPrepRawData InDetRIO_OnTrack StoreGateLib TrkPrepRawData TrkRIO_OnTrack TrkEventCnvToolsLib )
