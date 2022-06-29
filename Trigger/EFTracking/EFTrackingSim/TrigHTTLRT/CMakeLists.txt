# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHTTLRT )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )

# Component(s) in the package:
atlas_add_library( TrigHTTLRTLib
    src/*.cxx TrigHTTLRT/*.h
    PUBLIC_HEADERS         TrigHTTLRT
    LINK_LIBRARIES         AthenaBaseComps GaudiKernel TrigHTTBanksLib TrigHTTHoughLib TrigHTTObjectsLib
    PRIVATE_INCLUDE_DIRS   ${ROOT_INCLUDE_DIRS}
    PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrigHTTMapsLib
)

atlas_add_component( TrigHTTLRT
    src/components/*.cxx
    LINK_LIBRARIES              TrigHTTLRTLib
)

