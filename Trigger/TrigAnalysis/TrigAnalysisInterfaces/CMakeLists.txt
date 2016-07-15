# $Id: CMakeLists.txt 762065 2016-07-15 07:15:23Z krasznaa $
################################################################################
# Package: TrigAnalysisInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrigAnalysisInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools )

# Component(s) in the package:
atlas_add_library( TrigAnalysisInterfaces
   TrigAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS TrigAnalysisInterfaces
   LINK_LIBRARIES AsgTools )

atlas_add_dictionary( TrigAnalysisInterfacesDict
   TrigAnalysisInterfaces/TrigAnalysisInterfacesDict.h
   TrigAnalysisInterfaces/selection.xml
   LINK_LIBRARIES TrigAnalysisInterfaces )
