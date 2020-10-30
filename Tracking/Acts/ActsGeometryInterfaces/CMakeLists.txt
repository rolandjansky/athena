# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ActsGeometryInterfaces )

# External dependencies:
find_package( Acts COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( ActsGeometryInterfacesLib
                   ActsGeometryInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS ActsGeometryInterfaces
                   LINK_LIBRARIES
                   AthenaBaseComps
                   ActsCore
                   ActsGeometryLib
                   GaudiKernel
                   TrkGeometry )
