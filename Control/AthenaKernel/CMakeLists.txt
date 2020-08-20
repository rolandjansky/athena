# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaKernel )

# External dependencies:
find_package( Boost COMPONENTS program_options regex filesystem thread )
find_package( UUID )
find_package( CLHEP )
find_package( TBB )
# Only link agains the RT library if it's available.
find_library( RT_LIBRARY rt )
if( NOT RT_LIBRARY )
   unset( RT_LIBRARY )
endif()

# Libraries in the package:
atlas_add_library( AthenaKernel
   AthenaKernel/*.h AthenaKernel/*.icc src/*.cxx
   PUBLIC_HEADERS AthenaKernel
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${UUID_INCLUDE_DIRS} 
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${UUID_LIBRARIES} ${RT_LIBRARY} CxxUtils DataModelRoot GaudiKernel
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${CMAKE_DL_LIBS} RootUtils )

atlas_add_dictionary( AthenaKernelDict
   AthenaKernel/AthenaKernelDict.h
   AthenaKernel/selection.xml
   LINK_LIBRARIES GaudiKernel AthenaKernel )

atlas_install_python_modules( python/*.py 
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.txt )

# Test(s) in the package:
atlas_add_test( getMessageSvc_test
   SOURCES test/getMessageSvc_test.cxx
   LOG_IGNORE_PATTERN "^Wall clock time"
   LINK_LIBRARIES GaudiKernel TestTools AthenaKernel )

atlas_add_test( MsgStreamMember_test
   SOURCES test/MsgStreamMember_test.cxx
   LOG_IGNORE_PATTERN "^Wall clock time |ref count"
   LINK_LIBRARIES TestTools AthenaKernel )

atlas_add_test( AthenaPackageInfo_test
   SOURCES test/AthenaPackageInfo_test.cxx
   LINK_LIBRARIES AthenaKernel )

atlas_add_test( DirSearchPath_test
   SOURCES test/DirSearchPath_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel )

atlas_add_test( Chrono_test
   SOURCES test/Chrono_test.cxx
   LINK_LIBRARIES GaudiKernel AthenaKernel )

atlas_add_test( errorcheck_test
   SOURCES test/errorcheck_test.cxx
   LINK_LIBRARIES GaudiKernel TestTools AthenaKernel )

atlas_add_test( type_tools_test
   SOURCES test/type_tools_test.cxx
   LINK_LIBRARIES AthenaKernel )

atlas_add_test( Units_test
   SOURCES test/Units_test.cxx
   LINK_LIBRARIES GaudiKernel TestTools AthenaKernel )

atlas_add_test( DataObjectSharedPtr_test
   SOURCES test/DataObjectSharedPtr_test.cxx
   LINK_LIBRARIES AthenaKernel )

atlas_add_test( IRCUSvc_test
   SOURCES test/IRCUSvc_test.cxx
   LINK_LIBRARIES AthenaKernel )

atlas_add_test( RCUObject_test
   SOURCES test/RCUObject_test.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaKernel )

atlas_add_test( CondCont_test
   SOURCES test/CondCont_test.cxx
   LINK_LIBRARIES AthenaKernel TestTools )

atlas_add_test( CLIDRegistry_test
   SOURCES test/CLIDRegistry_test.cxx
   LINK_LIBRARIES AthenaKernel )

atlas_add_test( ClassName_test
   SOURCES test/ClassName_test.cxx
   LINK_LIBRARIES AthenaKernel )

atlas_add_test( safe_clid_test
   SOURCES test/safe_clid_test.cxx
   LINK_LIBRARIES AthenaKernel )

atlas_add_test( BaseInfo_test
   SOURCES test/BaseInfo_test.cxx
   LINK_LIBRARIES AthenaKernel )

atlas_add_test( SlotSpecificObj_test
   SOURCES test/SlotSpecificObj_test.cxx
   LINK_LIBRARIES AthenaKernel TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( DataBucket_test
   SOURCES
   test/DataBucket_test.cxx
   LINK_LIBRARIES AthenaKernel TestTools )

atlas_add_test( RCUUpdater_test
   SOURCES test/RCUUpdater_test.cxx
   LINK_LIBRARIES AthenaKernel TestTools )

atlas_add_test( MetaContDataBucket_test
   SOURCES test/MetaContDataBucket_test.cxx
   LINK_LIBRARIES AthenaKernel TestTools )

atlas_add_test( TopBase_test
   SOURCES test/TopBase_test.cxx
   LINK_LIBRARIES AthenaKernel TestTools )

atlas_add_test( RecyclableDataObject_test
  SOURCES test/RecyclableDataObject_test.cxx
  INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
  LINK_LIBRARIES AthenaKernel TestTools ${TBB_LIBRARIES} )

atlas_add_test( ExtendedEventContext_test
  SOURCES test/ExtendedEventContext_test.cxx
  LINK_LIBRARIES AthenaKernel TestTools )

atlas_add_test( ThinningDecisionBase_test
  SOURCES test/ThinningDecisionBase_test.cxx
  LINK_LIBRARIES AthenaKernel TestTools )

atlas_add_test( ThinningCache_test
  SOURCES test/ThinningCache_test.cxx
  LINK_LIBRARIES AthenaKernel TestTools )

atlas_add_test( ThinningInfo_test
  SOURCES test/ThinningInfo_test.cxx
  LINK_LIBRARIES AthenaKernel TestTools )

atlas_add_test( getThinningCache_test
  SOURCES test/getThinningCache_test.cxx
  LINK_LIBRARIES AthenaKernel TestTools )

atlas_add_test( AlgorithmTimer_test
  SOURCES test/AlgorithmTimer_test.cxx
  LINK_LIBRARIES AthenaKernel )
