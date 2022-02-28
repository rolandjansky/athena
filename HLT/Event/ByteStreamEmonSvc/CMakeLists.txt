################################################################################
# Package: ByteStreamEmonSvc
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamEmonSvc )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system unit_test_framework )
find_package( tdaq COMPONENTS emon ohroot owl is omniORB4 omnithread oh )

# Component(s) in the package:
atlas_add_component( ByteStreamEmonSvc
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} ByteStreamCnvSvcLib ByteStreamData GaudiKernel StoreGateLib PersistentDataModel ByteStreamCnvSvcBaseLib xAODEventInfo TrigConfData )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )

atlas_add_test(
    test_extract_histogram_tag
    SOURCES test/test_extract_histogram_tag.cxx src/extract_histogram_tag.cxx
    POST_EXEC_SCRIPT nopost.sh
    INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
    LINK_LIBRARIES Boost::unit_test_framework
)
