# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( StgcRawDataMonitoring )

# Component(s) in the package:
atlas_add_component( StgcRawDataMonitoring
   StgcRawDataMonitoring/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaMonitoringLib MuonIdHelpersLib MuonPrepRawData StoreGateLib ) 

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

