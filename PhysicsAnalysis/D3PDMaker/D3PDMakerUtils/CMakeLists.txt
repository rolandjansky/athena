# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( D3PDMakerUtils )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( D3PDMakerUtils
                   src/*.cxx
                   PUBLIC_HEADERS D3PDMakerUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel AthLinks CxxUtils GaudiKernel StoreGateLib D3PDMakerInterfaces
                   PRIVATE_LINK_LIBRARIES AthContainersInterfaces ${ROOT_LIBRARIES} SGTools )
