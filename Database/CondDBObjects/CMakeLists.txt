# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CondDBObjects )

# Component(s) in the package:
atlas_add_library( CondDBObjects
                   src/*.cxx
                   PUBLIC_HEADERS CondDBObjects
                   LINK_LIBRARIES AthenaKernel GaudiKernel )
