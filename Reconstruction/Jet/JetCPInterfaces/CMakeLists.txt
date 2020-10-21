# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetCPInterfaces )

# Declare an interface library for the package's header(s):
atlas_add_library( JetCPInterfaces
   JetCPInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS JetCPInterfaces
   LINK_LIBRARIES PATInterfaces JetInterface xAODJet )
