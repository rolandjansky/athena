# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonCombinedTimingTools )

# Component(s) in the package:
atlas_add_component( MuonCombinedTimingTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODMuon GaudiKernel MuonCombinedToolInterfaces TrkSegment TrkSpaceTimePoint TrkToolInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

