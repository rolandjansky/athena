# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkTop )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkTop
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps DerivationFrameworkInterfaces ExpressionEvaluationLib GaudiKernel StoreGateLib xAODBTagging xAODEventInfo xAODJet xAODTracking xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
