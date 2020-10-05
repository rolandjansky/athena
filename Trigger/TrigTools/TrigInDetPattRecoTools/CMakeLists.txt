# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetPattRecoTools )

# Component(s) in the package:
atlas_add_library( TrigInDetPattRecoTools
                   src/*.cxx
                   PUBLIC_HEADERS TrigInDetPattRecoTools
                   LINK_LIBRARIES TrigInDetPattRecoEvent
                   PRIVATE_LINK_LIBRARIES IRegionSelector TrigInDetEvent )

find_package(Boost)

atlas_add_test( mockSeeds
                SOURCES test/mockSeeds.cxx
                LINK_LIBRARIES PathResolver TrigInDetEvent TrigInDetPattRecoEvent TrigInDetPattRecoTools TrigSteeringEvent IRegionSelector
                PROPERTIES TIMEOUT 300 )

