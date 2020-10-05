# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloDmDetDescr )

# Component(s) in the package:
atlas_add_library( CaloDmDetDescr
                   src/*.cxx
                   PUBLIC_HEADERS CaloDmDetDescr
                   LINK_LIBRARIES CaloIdentifier StoreGateLib
                   PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel PathResolver )

# Install files from the package:
atlas_install_runtime( share/DeadMaterialRegionDescription_1.10c.txt )

