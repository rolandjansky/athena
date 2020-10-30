# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArRawUtils )

# Component(s) in the package:
atlas_add_library( LArRawUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS LArRawUtils
                   LINK_LIBRARIES CaloIdentifier CaloTTDetDescr AthenaBaseComps AthenaKernel Identifier GaudiKernel LArCablingLib LArIdentifier LArRawEvent StoreGateLib CaloTriggerToolLib
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr )

atlas_add_component( LArRawUtils
                     src/components/*.cxx
                     LINK_LIBRARIES LArRawUtilsLib )

