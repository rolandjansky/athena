################################################################################
# Package: VP1AODSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1AODSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTracking
   GaudiKernel
   graphics/VP1/VP1Base
   PRIVATE
   Control/StoreGate
   DetectorDescription/GeoPrimitives
   Event/EventPrimitives
   Event/xAOD/xAODBase
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODJet
   Event/xAOD/xAODMissingET
   Tools/PathResolver
   Tracking/TrkExtrapolation/TrkExInterfaces
   graphics/VP1/VP1HEPVis
   graphics/VP1/VP1Utils )

# External dependencies:
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets )

# Generate UI files automatically:
# Note: add the "Widgets" component to "find_package( Qt5 ...)" if you have UI
# files, otherwise UIC, even if CMAKE_AUTOUIC is set to ON, is not run
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VP1AODSystems
   VP1AODSystems/*.h src/*.h src/*.cxx
   PUBLIC_HEADERS VP1AODSystems
   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES xAODMuon xAODTracking GaudiKernel VP1Base StoreGateLib
   Qt5::Core Qt5::Gui
   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} GeoPrimitives EventPrimitives
   xAODBase xAODCaloEvent xAODJet xAODMissingET PathResolver TrkExInterfaces
   VP1HEPVis VP1Utils Qt5::Gui )

# Install files from the package:
atlas_install_runtime( textures/*.png )
