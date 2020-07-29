# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkHiggs )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkHiggs
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     DEFINITIONS ${CLHEP_DEFINITIONS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps DerivationFrameworkInterfaces TrigDecisionToolLib xAODEgamma xAODEventInfo xAODJet xAODMuon xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

