# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigMissingETCnv )

atlas_add_library( xAODTrigMissingETCnvLib
                   xAODTrigMissingETCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS xAODTrigMissingETCnv
                   LINK_LIBRARIES GaudiKernel xAODTrigMissingET )

# Component(s) in the package:
atlas_add_component( xAODTrigMissingETCnv
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel TrigMissingEtEvent xAODTrigMissingETCnvLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

