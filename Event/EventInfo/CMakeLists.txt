# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( EventInfo )

# External dependencies:
find_package( Boost COMPONENTS unit_test_framework )

# Component(s) in the package:
atlas_add_library( EventInfo
                   src/EventID.cxx
                   src/EventType.cxx
                   src/EventInfo.cxx
                   src/EventStreamInfo.cxx
                   src/PileUpEventInfo.cxx
                   src/PileUpTimeEventIndex.cxx
                   src/TriggerInfo.cxx
                   src/MergedEventInfo.cxx
                   src/TagInfo.cxx
                   PUBLIC_HEADERS EventInfo
                   LINK_LIBRARIES AthLinks AthenaKernel GaudiKernel )

atlas_add_dictionary( EventInfoDict
                      EventInfo/EventInfoDict.h
                      EventInfo/selection.xml
                      LINK_LIBRARIES EventInfo
                      DATA_LINKS EventInfo )

atlas_add_test( PileUpEventInfo_test
                SOURCES
                test/PileUpEventInfo_test.cxx
                LINK_LIBRARIES EventInfo )

atlas_add_test( MergedEventInfo_test
                SOURCES
                test/MergedEventInfo_test.cxx
                LINK_LIBRARIES EventInfo )

atlas_add_test( TagInfo_test                
                SOURCES
                test/TagInfo_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils EventInfo )

set_target_properties( EventInfo_TagInfo_test  PROPERTIES ENABLE_EXPORTS True )
