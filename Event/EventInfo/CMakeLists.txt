################################################################################
# Package: EventInfo
################################################################################

# Declare the package name:
atlas_subdir( EventInfo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/SGTools
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthLinks )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

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
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES SGTools GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools AthLinks )

atlas_add_dictionary( EventInfoDict
                      EventInfo/EventInfoDict.h
                      EventInfo/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools GaudiKernel TestTools AthLinks EventInfo
                      DATA_LINKS EventInfo )

atlas_add_test( PileUpEventInfo_test
                SOURCES
                test/PileUpEventInfo_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools GaudiKernel TestTools AthLinks EventInfo )

atlas_add_test( MergedEventInfo_test
                SOURCES
                test/MergedEventInfo_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools GaudiKernel TestTools AthLinks EventInfo )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

