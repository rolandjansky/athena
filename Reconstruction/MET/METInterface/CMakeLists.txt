# $Id: CMakeLists.txt 757529 2016-06-24 15:36:43Z krasznaa $
################################################################################
# Package: METInterface
################################################################################

# Declare the package name:
atlas_subdir( METInterface )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODBase
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODJet
   Event/xAOD/xAODMissingET
   PhysicsAnalysis/AnalysisCommon/PATInterfaces )

# Component(s) in the package:
atlas_add_library( METInterface
   METInterface/*.h
   INTERFACE
   PUBLIC_HEADERS METInterface
   LINK_LIBRARIES AsgTools xAODBase xAODEventInfo xAODJet xAODMissingET
   PATInterfaces )

atlas_add_dictionary( METInterfaceDict
   METInterface/METInterfaceDict.h
	METInterface/selection.xml
	LINK_LIBRARIES METInterface )
