# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTruthAlgs )

# Component(s) in the package:
atlas_add_component( InDetTruthAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib GaudiKernel InDetSimData InDetPrepRawData InDetReadoutGeometry TrkTruthData TrkToolInterfaces TrkTrack InDetTruthInterfaces )
