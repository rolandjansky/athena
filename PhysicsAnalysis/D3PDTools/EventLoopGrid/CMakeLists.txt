# $Id: CMakeLists.txt 781044 2016-10-28 15:35:33Z krasznaa $

# The name of the package:
atlas_subdir( EventLoopGrid )

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/EventLoop
   PhysicsAnalysis/D3PDTools/SampleHandler
   PRIVATE
   PhysicsAnalysis/D3PDTools/RootCoreUtils )

# External dependencies of the package:
find_package( ROOT COMPONENTS Core RIO PyROOT Tree )

# Library in the package:
atlas_add_root_dictionary( EventLoopGrid EventLoopGridCintDict
   ROOT_HEADERS EventLoopGrid/GridWorker.h EventLoopGrid/GridDriver.h
   EventLoopGrid/PrunDriver.h EventLoopGrid/GridJobLoader.h
   EventLoopGrid/PandaRootTools.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( EventLoopGrid
   EventLoopGrid/*.h Root/*.h Root/*.cxx ${EventLoopGridCintDict}
   PUBLIC_HEADERS EventLoopGrid
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} EventLoop SampleHandler
   PRIVATE_LINK_LIBRARIES RootCoreUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/* )
atlas_install_data( data/* )
