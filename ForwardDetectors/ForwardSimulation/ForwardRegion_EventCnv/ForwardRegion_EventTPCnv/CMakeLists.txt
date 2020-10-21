# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ForwardRegion_EventTPCnv )

# Component(s) in the package:
atlas_add_library( ForwardRegion_EventTPCnv
                   src/*.cxx
                   PUBLIC_HEADERS ForwardRegion_EventTPCnv
                   LINK_LIBRARIES ForwardRegion_SimEv AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES Identifier )

atlas_add_dictionary( ForwardRegion_EventTPCnvDict
                      ForwardRegion_EventTPCnv/SimulationHitTPCnvDict.h
                      ForwardRegion_EventTPCnv/selection.xml
                      LINK_LIBRARIES ForwardRegion_EventTPCnv )

atlas_add_test( SimulationHitCnv_p1_test
                SOURCES
                test/SimulationHitCnv_p1_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ForwardRegion_EventTPCnv GaudiKernel TestTools )
