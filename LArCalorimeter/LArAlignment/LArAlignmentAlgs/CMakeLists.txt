# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArAlignmentAlgs )

# Component(s) in the package:
atlas_add_component( LArAlignmentAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel DetDescrConditions RegistrationServicesLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

