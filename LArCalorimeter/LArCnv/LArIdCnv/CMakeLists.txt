# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArIdCnv )

# Component(s) in the package:
atlas_add_component( LArIdCnv
                     src/*.cxx
                     LINK_LIBRARIES DetDescrCnvSvcLib CaloIdentifier StoreGateLib IdDictDetDescr Identifier GaudiKernel LArIdentifier )

# Install files from the package:
atlas_install_joboptions( share/*.py )
