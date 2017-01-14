# $Id: CMakeLists.txt 740760 2016-04-16 07:38:37Z krasznaa $
################################################################################
# Package: PFlowUtils
################################################################################

# Declare the package name:
atlas_subdir( PFlowUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Control/AthToolSupport/AsgTools
  Control/AthenaBaseComps
  Event/xAOD/xAODPFlow
  PRIVATE
  Event/xAOD/xAODJet
  GaudiKernel )

# Component(s) in the package:
atlas_add_component( PFlowUtils
  PFlowUtils/*.h src/*.cxx Root/*.cxx src/components/*.cxx
  LINK_LIBRARIES AsgTools AthenaBaseComps xAODPFlow xAODJet GaudiKernel )

atlas_add_library( PFlowUtilsLib
  PFlowUtils/*.h Root/*.cxx
  PUBLIC_HEADERS PFlowUtils
  LINK_LIBRARIES AsgTools xAODPFlow )

# Install files from the package:
atlas_install_headers( PFlowUtils )
atlas_install_python_modules( python/*.py )
