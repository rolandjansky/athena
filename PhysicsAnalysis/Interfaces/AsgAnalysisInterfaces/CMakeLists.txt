# $Id: CMakeLists.txt 782748 2016-11-07 13:28:06Z will $

# The name of the package:
atlas_subdir( AsgAnalysisInterfaces )

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEventInfo
   PhysicsAnalysis/AnalysisCommon/PATInterfaces )

# External(s):
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( AsgAnalysisInterfaces
   AsgAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS AsgAnalysisInterfaces
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo PATInterfaces )

atlas_add_dictionary( AsgAnalysisInterfacesDict
   AsgAnalysisInterfaces/AsgAnalysisInterfacesDict.h
   AsgAnalysisInterfaces/selection.xml
   LINK_LIBRARIES AsgAnalysisInterfaces )

