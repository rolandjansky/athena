# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloSimEvent )

# Component(s) in the package:
atlas_add_library( CaloSimEvent
                   src/*.cxx
                   PUBLIC_HEADERS CaloSimEvent
                   LINK_LIBRARIES AthenaKernel Identifier HitManagement
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( CaloSimEventDict
                      CaloSimEvent/CaloSimEventDict.h
                      CaloSimEvent/selection.xml
                      LINK_LIBRARIES CaloSimEvent )
