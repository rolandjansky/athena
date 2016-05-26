################################################################################
# Package: ZdcEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( ZdcEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          ForwardDetectors/ZDC/ZDC_SimEvent
                          ForwardDetectors/ZDC/ZdcCnv/ZdcEventTPCnv
                          ForwardDetectors/ZDC/ZdcEvent
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( ZdcEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ZdcEvent/ZdcDigitsCollection.h ZdcEvent/ZdcRawChannelCollection.h ZDC_SimEvent/ZDC_SimStripHit_Collection.h ZDC_SimEvent/ZDC_SimPixelHit_Collection.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities ZDC_SimEvent ZdcEventTPCnv ZdcEvent GaudiKernel )

# Install files from the package:
atlas_install_headers( ZdcEventAthenaPool )
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities )
   set( AthenaPoolUtilitiesTest_DIR
      ${CMAKE_SOURCE_DIR}/Database/AthenaPOOL/AthenaPoolUtilities/cmake )
endif()
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  run_tpcnv_test( ZdcEventTPCnv_15.6.7   ESD-15.6.7-data )
else()
   message( WARNING "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()   
