# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CommissionUtils )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( CommissionUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS CommissionUtils
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CommissionEvent LArSimEvent TrackRecordLib )

atlas_add_component( CommissionUtils
                     src/components/*.cxx
                     LINK_LIBRARIES CommissionUtilsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

