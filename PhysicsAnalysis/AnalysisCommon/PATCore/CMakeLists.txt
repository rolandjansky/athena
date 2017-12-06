# $Id: CMakeLists.txt 744640 2016-05-03 19:08:14Z krasznaa $
################################################################################
# Package: PATCore
################################################################################

# Declare the package name:
atlas_subdir( PATCore )

# Extra dependencies, based on what environment we are in:
if( NOT XAOD_STANDALONE )
   set( extra_deps GaudiKernel )
   set( extra_libs GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthToolSupport/AsgTools
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# In the standalone build create a "CINT dictionary":
if( XAOD_STANDALONE )
   atlas_add_root_dictionary( PATCoreLib PATCoreLibDictSource
      ROOT_HEADERS PATCore/TAccept.h PATCore/TResult.h
      PATCore/TSelectorToolBase.h PATCore/TCalculatorToolBase.h Root/LinkDef.h
      EXTERNAL_PACKAGES ROOT )
endif()

# Component(s) in the package:
atlas_add_library( PATCoreLib
   PATCore/*.h Root/*.cxx ${PATCoreLibDictSource}
   PUBLIC_HEADERS PATCore
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AsgTools ${extra_libs} )

# Only build the "Reflex dictionary" in the offline environment:
atlas_add_dictionary( PATCoreDict
   PATCore/PATCoreDict.h
   PATCore/selection.xml
   LINK_LIBRARIES PATCoreLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
