# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODMissingETCnv )

# Component(s) in the package:
atlas_add_component( xAODMissingETCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODMissingET GaudiKernel AthenaBaseComps MissingETEvent )

# Install files from the package:
atlas_install_headers( xAODMissingETCnv )
atlas_install_joboptions( share/*.py )

