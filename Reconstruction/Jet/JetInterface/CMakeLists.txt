# $Id: CMakeLists.txt 778355 2016-10-13 19:11:02Z krasznaa $
################################################################################
# Package: JetInterface
################################################################################

# Declare the package name:
atlas_subdir( JetInterface )

find_package( ROOT )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODJet
   Event/xAOD/xAODTracking
   Event/xAOD/xAODBase )

# Component(s) in the package:
atlas_add_library( JetInterface
   JetInterface/*.h Root/*.cxx
   PUBLIC_HEADERS JetInterface
   LINK_LIBRARIES AsgTools AsgDataHandlesLib xAODEventInfo xAODJet xAODTracking xAODBase )

atlas_add_dictionary( JetInterfaceDict
   JetInterface/JetInterfaceDict.h
   JetInterface/selection.xml
   LINK_LIBRARIES JetInterface )
