################################################################################
# Package: TrigConf2COOLOnline
################################################################################

# Declare the package name:
atlas_subdir( TrigConf2COOLOnline )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Trigger/TrigConfiguration/TrigConfHLTData
   Trigger/TrigConfiguration/TrigConfL1Data
   Trigger/TrigConfiguration/TrigConfStorage )

# External dependencies:
find_package( Boost COMPONENTS program_options )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( tdaq COMPONENTS daq-core-dal daq-hlt-dal 
              rc_CmdLine rc_ItemCtrl rc_OnlSvc rc_RCCommands omniORB4 omnithread owl is ipc config TTCInfo)
find_package( tdaq-common COMPONENTS ers )
find_package( TBB )

# Component(s) in the package:
atlas_add_library( TrigConf2COOLOnline
   src/*.cxx
   PUBLIC_HEADERS TrigConf2COOLOnline
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${TDAQ_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
   ${TBB_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES}
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${TBB_LIBRARIES}
   TrigConfHLTData TrigConfL1Data TrigConfStorage )

atlas_add_executable( TrigConf2CoolOnline
   src/TrigConf2CoolOnline.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
   ${TDAQ_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} ${TDAQ_LIBRARIES}
   TrigConf2COOLOnline )

