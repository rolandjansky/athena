################################################################################
# Package: ByteStreamEmonSvc
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamEmonSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/ByteStreamCnvSvc
                          Event/ByteStreamData
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Database/PersistentDataModel
                          Event/ByteStreamCnvSvcBase
                          Event/EventInfo )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( tdaq COMPONENTS emon ohroot owl is omniORB4 omnithread )

# Component(s) in the package:
atlas_add_library( ByteStreamEmonSvcLib
                   src/ByteStreamEmonInputSvc.cxx
                   PUBLIC_HEADERS ByteStreamEmonSvc
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} ByteStreamData GaudiKernel ByteStreamCnvSvcLib ByteStreamData_test StoreGateLib SGtests ByteStreamCnvSvcBaseLib
                   PRIVATE_LINK_LIBRARIES PersistentDataModel EventInfo )

atlas_add_component( ByteStreamEmonSvc
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} ByteStreamCnvSvcLib ByteStreamData ByteStreamData_test GaudiKernel StoreGateLib SGtests PersistentDataModel ByteStreamCnvSvcBaseLib EventInfo ByteStreamEmonSvcLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

