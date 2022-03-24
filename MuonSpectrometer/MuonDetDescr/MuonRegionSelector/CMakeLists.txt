# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonRegionSelector )

# External dependencies:
find_package( CLHEP )

atlas_add_library( MuonRegionSelectorLib
                   MuonRegionSelector/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonRegionSelector
                   LINK_LIBRARIES RegSelLUT AthenaBaseComps GaudiKernel GeoPrimitives )

# Component(s) in the package:
atlas_add_component( MuonRegionSelector
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GeoPrimitives 
                     RegSelLUT GaudiKernel Identifier CSCcablingLib StoreGateLib MuonCablingData
                     MuonTGC_CablingLib RPC_CondCablingLib
                     MuonReadoutGeometry MuonAGDDDescription MuonRegionSelectorLib )


