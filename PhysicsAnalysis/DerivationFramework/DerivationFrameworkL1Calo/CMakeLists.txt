# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DerivationFrameworkL1Calo )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkL1Calo
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps DerivationFrameworkInterfaces xAODTrigL1Calo )

# Install files from the package:
atlas_install_joboptions( share/*.py )

