################################################################################
# Package: ByteStreamEmonSvc
################################################################################

# Declare the package name:
atlas_subdir( ByteStreamEmonSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          GaudiKernel
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamCnvSvc
                          Event/ByteStreamData
                          Event/EventInfo
                          Control/CxxUtils
                          Control/StoreGate
                          Database/PersistentDataModel )


# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( tdaq COMPONENTS emon ohroot owl is omniORB4 omnithread oh )

# Component(s) in the package:
atlas_add_component( ByteStreamEmonSvc
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES} ByteStreamCnvSvcLib ByteStreamData GaudiKernel StoreGateLib PersistentDataModel ByteStreamCnvSvcBaseLib EventInfo )

# Install files from the package:
atlas_install_joboptions( share/*.py )
