# $Id: CMakeLists.txt 797214 2017-02-14 19:51:39Z ssnyder $
#
# Build configuration for the xAODRootAccess package.
#

# The name of the package:
atlas_subdir( xAODRootAccess )

# Extra dependencies, based on the build environment:
set( extra_deps )
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/SGTools )
   set( extra_libs SGTools )
endif()

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/CxxUtils
   Control/AthContainersInterfaces
   Control/AthContainers
   Event/xAOD/xAODCore
   Event/xAOD/xAODEventFormat
   Control/xAODRootAccessInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree RIO Net MathCore )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( xAODRootAccess xAODRootAccessDictSource
   ROOT_HEADERS xAODRootAccess/tools/TTransTrees.h
   xAODRootAccess/tools/TEventBranch.h xAODRootAccess/tools/TEventTree.h
   xAODRootAccess/tools/TMetaBranch.h xAODRootAccess/tools/TMetaTree.h
   xAODRootAccess/tools/TEventBranch.h xAODRootAccess/tools/TEventTree.h
   xAODRootAccess/tools/TMetaBranch.h xAODRootAccess/tools/TMetaTree.h
   xAODRootAccess/tools/TFileChecker.h xAODRootAccess/tools/TFileMerger.h
   xAODRootAccess/MakeTransientTree.h xAODRootAccess/Init.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( xAODRootAccess
   xAODRootAccess/*.h xAODRootAccess/*.icc
   xAODRootAccess/tools/*.h xAODRootAccess/tools/*.icc
   Root/*.cxx ${xAODRootAccessDictSource}
   PUBLIC_HEADERS xAODRootAccess
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODCore xAODEventFormat
   xAODRootAccessInterfaces ${extra_libs} )

# Build a (Reflex) dictionary library:
atlas_add_dictionary( xAODRootAccessDict
   xAODRootAccess/xAODRootAccessDict.h
   xAODRootAccess/selection.xml
   LINK_LIBRARIES xAODRootAccess )

# Install the Python modules of the package:
atlas_install_python_modules( python/*.py )

# Build the executables of the package:
foreach( exec xAODChecker xAODMerge xAODFileReadTest )
   atlas_add_executable( ${exec} util/${exec}.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess )
endforeach()

# Helper macro setting up the compiled tests of the package:
macro( _add_test name )
   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils AthContainers xAODCore xAODRootAccess
      PROPERTIES TIMEOUT 600 )
endmacro( _add_test )

# Build/run unit tests for the package:
if( NOT SIMULATIONBASE )
   _add_test( ut_xaodrootaccess_metadata_test )
endif()
_add_test( ut_xaodrootaccess_remap_test )
_add_test( ut_xaodrootaccess_slimming_test )
_add_test( ut_xaodrootaccess_stats_test )
_add_test( ut_xaodrootaccess_tauxvector_test )
_add_test( ut_xaodrootaccess_tauxstore_test )
_add_test( ut_xaodrootaccess_tauxstore_insertmove_test )
_add_test( ut_xaodrootaccess_tchain_test )
#_add_test( ut_xaodrootaccess_tevent_test )
_add_test( ut_xaodrootaccess_tfileaccesstracer_test )
_add_test( ut_xaodrootaccess_tfilemerger_test )
#_add_test( ut_xaodrootaccess_transtree_test )
_add_test( ut_xaodrootaccess_tstore_test )

# Run the scripted tests for the package:
#atlas_add_test( ut_xaodrootaccess_tpyevent_test
#   SCRIPT test/ut_xaodrootaccess_tpyevent_test.py PROPERTIES TIMEOUT 600 )
#atlas_add_test( ut_xaodrootaccess_tpystore_test
#   SCRIPT test/ut_xaodrootaccess_tpystore_test.py PROPERTIES TIMEOUT 600 )
#atlas_add_test( ut_xaodrootaccess_transtree2_test
#   SCRIPT test/ut_xaodrootaccess_transtree2_test.py PROPERTIES TIMEOUT 600 )
