# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_RawEv )

# Component(s) in the package:
atlas_add_library( ALFA_RawEv
                   src/*.cxx
                   PUBLIC_HEADERS ALFA_RawEv
                   LINK_LIBRARIES AthContainers AthenaKernel HitManagement
                   PRIVATE_LINK_LIBRARIES GaudiKernel EventContainers StoreGateLib )

atlas_add_dictionary( ALFA_RawEvDict
                      ALFA_RawEv/ALFA_RawEvDict.h
                      ALFA_RawEv/selection.xml
                      LINK_LIBRARIES ALFA_RawEv )
