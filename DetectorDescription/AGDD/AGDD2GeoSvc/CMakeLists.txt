# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AGDD2GeoSvc )

atlas_add_library ( AGDD2GeoSvcLib
                    INTERFACE
                    PUBLIC_HEADERS AGDD2GeoSvc
                    LINK_LIBRARIES AGDDControl AGDDKernel AthenaBaseComps GaudiKernel )

# Component(s) in the package:
atlas_add_component( AGDD2GeoSvc
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AGDD2GeoSvcLib AGDDHandlers )
