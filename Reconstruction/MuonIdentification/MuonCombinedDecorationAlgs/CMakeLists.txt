# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonCombinedDecorationAlgs )

# Component(s) in the package:
atlas_add_component( MuonCombinedDecorationAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib xAODCaloEvent xAODMuon xAODMuonCnvLib xAODTruth 
                                    xAODTracking GaudiKernel MuonSegment MuonRecToolInterfaces MuonLayerEvent MuonCombinedEvent MuonRecHelperToolsLib
                                    MuonCombinedToolInterfaces MuonPrepRawData StoreGateLib TrkSegment TrkTrack TrkToolInterfaces MuonCompetingRIOsOnTrack
                                    MuonDetDescrUtils)
