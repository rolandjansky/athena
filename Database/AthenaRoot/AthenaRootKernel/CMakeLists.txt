# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaRootKernel )

# Component(s) in the package:
atlas_add_library( AthenaRootKernel
                   src/*.cxx
                   PUBLIC_HEADERS AthenaRootKernel
                   LINK_LIBRARIES DataModelRoot GaudiKernel )
