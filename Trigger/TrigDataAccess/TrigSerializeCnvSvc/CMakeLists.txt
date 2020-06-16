# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigSerializeCnvSvc )

# Component(s) in the package:
atlas_add_library( TrigSerializeCnvSvcLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigSerializeCnvSvc
                   LINK_LIBRARIES AthContainers AthenaBaseComps AthenaKernel CxxUtils GaudiKernel StoreGateLib
                   PRIVATE_LINK_LIBRARIES DataModelRoot PersistentDataModel StorageSvc TrigSerializeResultLib TrigSerializeTPLib )

atlas_add_component( TrigSerializeCnvSvc
                     src/components/*.cxx
                     LINK_LIBRARIES TrigSerializeCnvSvcLib )
