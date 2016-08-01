################################################################################
# Package: EventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( EventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Event/EventInfo
                          Event/EventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( EventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES EventInfo/EventInfo.h EventInfo/EventStreamInfo.h EventInfo/MergedEventInfo.h EventInfo/PileUpEventInfo.h
                           LINK_LIBRARIES AthenaPoolUtilities AthenaKernel AthenaPoolCnvSvcLib EventInfo EventTPCnv )

# Install files from the package:
atlas_install_headers( EventAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  run_tpcnv_test( EventTPCnv_15.0.0   AOD-15.0.0-full )
  run_tpcnv_test( EventTPCnv_17.5.0   AOD-17.5.0-full )
  run_tpcnv_test( EventTPCnv_18.0.0   AOD-18.0.0-full )
  run_tpcnv_test( EventTPCnv_20.1.7.2 ESD-20.1.7.2    )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
