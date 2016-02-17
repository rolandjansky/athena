################################################################################
# Package: VP1TrkAuxAlgs
################################################################################
# Author: <abandoned>
################################################################################

# Declare the package name:
atlas_subdir( VP1TrkAuxAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   PRIVATE
   GaudiKernel
   Tracking/TrkExtrapolation/TrkExInterfaces
   Tracking/TrkFitter/TrkFitterInterfaces
   graphics/VP1/VP1Base
   graphics/VP1/VP1Utils )

# External dependencies:
find_package( Qt4 COMPONENTS QtCore )

# Component(s) in the package:
atlas_add_component( VP1TrkAuxAlgs src/*.cxx src/components/*.cxx
   LINK_LIBRARIES Qt4::QtCore AthenaBaseComps GaudiKernel TrkExInterfaces
   TrkFitterInterfaces VP1Base VP1Utils )

# Install files from the package:
atlas_install_headers( VP1TrkAuxAlgs )
