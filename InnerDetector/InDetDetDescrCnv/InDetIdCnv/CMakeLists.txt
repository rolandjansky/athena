# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetIdCnv )

# Component(s) in the package:
atlas_add_component( InDetIdCnv
                     src/*.cxx
                     LINK_LIBRARIES StoreGateLib DetDescrCnvSvcLib IdDictDetDescr GaudiKernel InDetIdentifier )

# Install files from the package:
atlas_install_joboptions( share/*.py )

