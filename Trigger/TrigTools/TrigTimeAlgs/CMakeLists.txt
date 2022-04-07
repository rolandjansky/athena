# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTimeAlgs )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( TrigTimeAlgsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigTimeAlgs
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES}
                   LINK_LIBRARIES AthenaKernel GaudiKernel )
