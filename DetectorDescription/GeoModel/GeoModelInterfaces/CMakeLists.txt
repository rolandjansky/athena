# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeoModelInterfaces )

# Component(s) in the package:
atlas_add_library (GeoModelInterfaces
                   GeoModelInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS GeoModelInterfaces
                   LINK_LIBRARIES AthenaKernel GaudiKernel )
