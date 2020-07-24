# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMuonCoinHierarchy )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_component( TrigMuonCoinHierarchy
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AnalysisTriggerEvent AthenaBaseComps EventInfo GaudiKernel MuonIdHelpersLib MuonPrepRawData MuonRDO MuonReadoutGeometry MuonTrigCoinData PathResolver TGCcablingInterfaceLib TrigT1Result TrkCompetingRIOsOnTrack TrkEventUtils TrkRIO_OnTrack TrkTrack muonEvent )

# Install files from the package:
atlas_install_runtime( share/mask2/*.txt )
