# $Id: CMakeLists.txt 777928 2016-10-11 22:23:08Z krasznaa $

# Set the name of the package:
atlas_subdir( MultiDraw )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/EventLoop
   PRIVATE
   PhysicsAnalysis/D3PDTools/RootCoreUtils
   PhysicsAnalysis/D3PDTools/SampleHandler )

# External(s):
find_package( ROOT COMPONENTS Core Tree TreePlayer Hist )

# Libraries in the package:
atlas_add_root_dictionary( MultiDraw MultiDrawDictSource
   ROOT_HEADERS MultiDraw/AlgCFlow.h MultiDraw/AlgHist.h
   MultiDraw/Formula.h MultiDraw/FormulaSvc.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( MultiDraw
   MultiDraw/*.h Root/*.cxx ${MultiDrawDictSource}
   PUBLIC_HEADERS MultiDraw
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} EventLoop
   PRIVATE_LINK_LIBRARIES RootCoreUtils )

# Test(s) in the package:
macro( _add_test name )
   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} EventLoop RootCoreUtils SampleHandler
      MultiDraw )
endmacro( _add_test )

_add_test( ut_alg_cflow )
_add_test( ut_alg_hist )

#disabling this test for now
#_add_test( ut_condor_large )
