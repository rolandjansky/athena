# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeometryDBSvc )

# Component(s) in the package:
atlas_add_library( GeometryDBSvcLib
                   INTERFACE
                   PUBLIC_HEADERS GeometryDBSvc
                   LINK_LIBRARIES RDBAccessSvcLib GaudiKernel )

atlas_add_component( GeometryDBSvc
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps CxxUtils GeometryDBSvcLib )
