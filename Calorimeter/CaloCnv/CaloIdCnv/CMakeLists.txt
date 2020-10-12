# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloIdCnv )

# Component(s) in the package:
atlas_add_component( CaloIdCnv
                     src/*.cxx
                     LINK_LIBRARIES CaloIdentifier AthenaKernel DetDescrCnvSvcLib StoreGateLib IdDictDetDescr Identifier GaudiKernel )

# Install files from the package:
atlas_install_joboptions( share/*.py )
