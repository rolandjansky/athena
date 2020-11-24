# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetAssociationTools )

# Component(s) in the package:
atlas_add_component( InDetAssociationTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel InDetPrepRawData TrkPrepRawData TrkToolInterfaces AtlasDetDescr InDetIdentifier InDetRIO_OnTrack InDetReadoutGeometry StoreGateLib TrkDetElementBase TrkRIO_OnTrack TrkTrack TrkEventUtils )
