# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODMissingETCnv )

# Component(s) in the package:
atlas_add_library( xAODMissingETCnvLib
                   xAODMissingETCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODMissingETCnv
                   LINK_LIBRARIES GaudiKernel xAODMissingET )

atlas_add_component( xAODMissingETCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES xAODMissingET GaudiKernel AthenaBaseComps MissingETEvent xAODMissingETCnvLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
