# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCombinedEvent )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_library( TrigCombinedEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigCombinedEvent
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers AthenaKernel GaudiKernel TrigNavigationLib TrigSerializeCnvSvcLib )

atlas_add_sercnv_library( TrigCombinedEventSerCnv
                          FILES TrigCombinedEvent/TrigComposite.h TrigCombinedEvent/TrigCompositeContainer.h
                          LINK_LIBRARIES TrigCombinedEvent )

atlas_add_dictionary( TrigCombinedEventDict
                      TrigCombinedEvent/TrigCombinedEventDict.h
                      TrigCombinedEvent/selection.xml
                      LINK_LIBRARIES TrigCombinedEvent
                      DATA_LINKS TrigComposite
                      ELEMENT_LINKS TrigCompositeContainer )

atlas_add_test( TrigComposite_test
                SOURCES
                test/TrigComposite_test.cxx
                LINK_LIBRARIES TrigCombinedEvent
                POST_EXEC_SCRIPT nopost.sh )
