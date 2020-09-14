################################################################################
# Package: CoraCool
################################################################################

# Declare the package name:
atlas_subdir( CoraCool )

# External dependencies:
find_package( Boost COMPONENTS  unit_test_framework filesystem thread system)
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CoraCool
                   src/CoraCoolDatabaseSvcFactory.cxx
                   src/CoraCoolDatabaseSvc.cxx
                   src/CoraCoolDatabase.cxx
                   src/CoraCoolSequence.cxx
                   src/CoraCoolFolder.cxx
                   src/CoraCoolObject.cxx
                   src/CoraCoolObjectIter.cxx
                   PUBLIC_HEADERS CoraCool
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES CxxUtils ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} )
                   
atlas_add_test  ( CoraCoolDatabaseSvcFactory_test
                  SOURCES
                  test/CoraCoolDatabaseSvcFactory_test.cxx 
                  INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}  ${CORAL_INCLUDE_DIRS}
                  LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} CoraCool
                  POST_EXEC_SCRIPT ""
                  )
                  
atlas_add_test  ( CoraCoolDatabaseSvc_test
                  SOURCES
                  test/CoraCoolDatabaseSvc_test.cxx 
                  INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}  ${CORAL_INCLUDE_DIRS}
                  LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} CoraCool
                  POST_EXEC_SCRIPT ""
                  )                

atlas_add_test  ( CoraCoolDatabase_test
                  SOURCES
                  test/CoraCoolDatabase_test.cxx 
                  INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}  ${CORAL_INCLUDE_DIRS}
                  LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} CoraCool
                  POST_EXEC_SCRIPT ""
                  )     
                  
atlas_add_test  ( CoraCoolFolder_test
                  SOURCES
                  test/CoraCoolFolder_test.cxx 
                  INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}  ${CORAL_INCLUDE_DIRS}
                  LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} CoraCool
                  POST_EXEC_SCRIPT ""
                  )   
                  
atlas_add_test  ( CoraCoolObjectIter_test
                  SOURCES
                  test/CoraCoolObjectIter_test.cxx 
                  INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}  ${CORAL_INCLUDE_DIRS}
                  LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} CoraCool
                  POST_EXEC_SCRIPT ""
                  )     
                      
