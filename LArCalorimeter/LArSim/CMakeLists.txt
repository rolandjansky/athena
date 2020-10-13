# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArSim )

# Component(s) in the package:
atlas_add_component( LArSim
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel LArSimEvent CaloIdentifier StoreGateLib )

# Install files from the package:
atlas_install_joboptions( share/*.txt share/*.py )
