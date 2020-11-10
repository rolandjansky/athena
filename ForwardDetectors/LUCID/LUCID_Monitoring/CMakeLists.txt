# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LUCID_Monitoring )

# External dependencies:
find_package( ROOT COMPONENTS Hist )

# Component(s) in the package:
atlas_add_component( LUCID_Monitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib LUCID_RawEvent )

# Install files from the package:
atlas_install_joboptions( share/*.py )

