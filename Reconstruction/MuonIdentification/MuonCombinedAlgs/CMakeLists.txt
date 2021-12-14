# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonCombinedAlgs )

# Component(s) in the package:
atlas_add_component( MuonCombinedAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib xAODCaloEvent xAODMuon xAODMuonCnvLib xAODTruth 
                                    xAODTracking GaudiKernel MuonSegment MuonRecToolInterfaces MuonLayerEvent MuonCombinedEvent 
                                    MuonCombinedToolInterfaces MuonPrepRawData StoreGateLib TrkSegment TrkTrack TrkToolInterfaces 
                                    MuonDetDescrUtils MuonRecHelperToolsLib)

atlas_install_python_modules( python/MuonCombinedAlgsMonitoring.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
