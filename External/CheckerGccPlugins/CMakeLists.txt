# $Id: CMakeLists.txt 748977 2016-05-23 12:00:12Z krasznaa $

# The name of the package:
atlas_subdir( CheckerGccPlugins )

# Only do anything for a GCC compiler:
if( NOT CMAKE_COMPILER_IS_GNUCXX )
   return()
endif()

# Since this package doesn't depend on any other ATLAS packages,
# in release recompilation mode don't even go further than this.
if( ATLAS_RELEASE_MODE )
   return()
endif()

# External(s) needed by the compilation:
find_package( gmp )

# Set up the build of the library:
atlas_add_library( checker_gccplugins
   src/*.cxx
   NO_PUBLIC_HEADERS MODULE
   PRIVATE_INCLUDE_DIRS ${GMP_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${GMP_LIBRARIES} )
target_compile_options( checker_gccplugins
   PRIVATE -fno-rtti )
target_compile_options( checker_gccplugins
   PRIVATE -iwithprefix plugin/include )


function( CheckerGccPlugins_test1 testName mode )

  set( _fullName ${testName}_${mode} )
  set( _checkerTestFlags ${CMAKE_CXX_FLAGS_${mode}} )
  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/checker_test.sh.in
                  ${CMAKE_CURRENT_BINARY_DIR}/CheckerGccPlugins_${_fullName}.sh
                  @ONLY )
  atlas_add_test( ${_fullName}
                  SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/CheckerGccPlugins_${_fullName}.sh
                  POST_EXEC_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test/post.sh ${_fullName} '' ${CMAKE_CURRENT_SOURCE_DIR}/share/${testName}_test.ref"
                   )

endfunction( CheckerGccPlugins_test1 )


function( CheckerGccPlugins_test testName )
CheckerGccPlugins_test1( ${testName} RELEASE )
CheckerGccPlugins_test1( ${testName} DEBUG )
endfunction( CheckerGccPlugins_test )


CheckerGccPlugins_test( gaudi_inheritance )
CheckerGccPlugins_test( divcheck )
CheckerGccPlugins_test( naming )
CheckerGccPlugins_test( ists1 )
CheckerGccPlugins_test( thread1 )
CheckerGccPlugins_test( thread2 )
CheckerGccPlugins_test( thread3 )
CheckerGccPlugins_test( thread4 )
CheckerGccPlugins_test( thread5 )
CheckerGccPlugins_test( thread6 )
CheckerGccPlugins_test( thread7 )
#CheckerGccPlugins_test( usingns1 )
#CheckerGccPlugins_test( usingns2 )
#CheckerGccPlugins_test( usingns3 )
#CheckerGccPlugins_test( usingns4 )
#CheckerGccPlugins_test( usingns5 )
#CheckerGccPlugins_test( usingns6 )
#CheckerGccPlugins_test( usingns7 )
