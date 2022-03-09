# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TGCTriggerCondSvc )

# Component(s) in the package:
atlas_add_library( TGCTriggerCondSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS TGCTriggerCondSvc
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities StoreGateLib )

atlas_add_component( TGCTriggerCondSvc
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaPoolUtilities StoreGateLib TGCTriggerCondSvcLib )
