# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DBReplicaSvc )

atlas_add_library( DBReplicaSvcLib
                   INTERFACE
                   PUBLIC_HEADERS DBReplicaSvc
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( DBReplicaSvc
                     src/DBReplicaSvc.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps DBReplicaSvcLib PathResolver )
