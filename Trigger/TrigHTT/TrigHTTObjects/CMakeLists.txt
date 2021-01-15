################################################################################
# Package: TrigHTTObjects
################################################################################

# Declare the package name:
atlas_subdir( TrigHTTObjects )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
    PUBLIC
        GaudiKernel
        Control/AthenaKernel
        Control/SGTools
        Trigger/TrigHTT/TrigHTTUtils
)

# External dependencies:

find_package ( ROOT )
find_package( Boost COMPONENTS iostreams program_options filesystem thread system )
find_package ( Eigen )

# Component(s) in the package:
atlas_add_root_dictionary( TrigHTTObjectsLib
                           TrigHTTObjectsLibDictSource
                           ROOT_HEADERS
			   TrigHTTObjects/HTTEventInfo.h
			   TrigHTTObjects/HTTOptionalEventInfo.h
			   TrigHTTObjects/HTTTowerInputHeader.h
			   TrigHTTObjects/HTTEventInputHeader.h
			   TrigHTTObjects/HTTLogicalEventInputHeader.h
			   TrigHTTObjects/HTTLogicalEventOutputHeader.h
			   TrigHTTObjects/HTTHit.h
			   TrigHTTObjects/HTTCluster.h
			   TrigHTTObjects/HTTRoad.h
			   TrigHTTObjects/HTTMultiTruth.h
			   TrigHTTObjects/HTTMatchInfo.h
			   TrigHTTObjects/HTTTrack.h
			   TrigHTTObjects/HTTTruthTrack.h
			   TrigHTTObjects/HTTTrackStream.h
			   TrigHTTObjects/TrigHTTObjectsLinkDef.h
			   EXTERNAL_PACKAGES ROOT Boost Eigen
)

# Component(s) in the package:
atlas_add_library(
    TrigHTTObjectsLib src/*.cxx TrigHTTObjects/*.h ${TrigHTTObjectsLibDictSource}
    PUBLIC_HEADERS TrigHTTObjects
    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
    LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaKernel ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} TrigHTTUtilsLib
)

# Define units tests
atlas_add_test(
    TrigHTTPattern_test
    SOURCES test/HTTPattern_test.cxx
    LINK_LIBRARIES TrigHTTObjectsLib
)

atlas_add_test(
    TrigHTT_SSID_test
    SOURCES test/HTT_SSID_test.cxx
    LINK_LIBRARIES TrigHTTObjectsLib
)



