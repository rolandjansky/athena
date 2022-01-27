# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArRawUtils )

# Component(s) in the package:
atlas_add_library( LArRawUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS LArRawUtils
                   LINK_LIBRARIES CaloIdentifier CaloTTDetDescr AthenaBaseComps AthenaKernel Identifier GaudiKernel LArCablingLib LArIdentifier LArRawEvent StoreGateLib CaloTriggerToolLib CxxUtils
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr LArRecConditions )
