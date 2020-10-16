# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_Ntuple )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO HistPainter Gpad )

# Component(s) in the package:
atlas_add_component( ALFA_Ntuple
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib AthenaPoolUtilities ALFA_Geometry ALFA_RawEv ALFA_GloRecEv ALFA_LocRecLib ALFA_LocRecCorrEv ALFA_LocRecEv GaudiKernel GeneratorObjects )
