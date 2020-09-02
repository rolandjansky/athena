# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkSUSY )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkSUSY
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     DEFINITIONS ${CLHEP_DEFINITIONS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel DerivationFrameworkInterfaces ExpressionEvaluationLib GaudiKernel MCTruthClassifierLib RecoToolInterfaces xAODAssociations xAODEventInfo xAODJet xAODTracking xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
