# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigSerializeTP )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( TrigSerializeTPLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigSerializeTP
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_component( TrigSerializeTP
                     src/components/*.cxx
                     LINK_LIBRARIES TrigSerializeTPLib )
