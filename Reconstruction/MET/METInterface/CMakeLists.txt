################################################################################
# Package: METInterface
################################################################################

# Declare the package name:
atlas_subdir( METInterface )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMissingET
                          PhysicsAnalysis/AnalysisCommon/PATInterfaces )

# Component(s) in the package:
atlas_add_library( METInterface
                   PUBLIC_HEADERS METInterface
                   LINK_LIBRARIES AsgTools xAODBase xAODEventInfo xAODJet xAODMissingET PATInterfaces )

atlas_add_dictionary( METInterfaceDict
       		      METInterface/METInterfaceDict.h
	              METInterface/selection.xml
	              INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
	              LINK_LIBRARIES ${ROOT_LIBRARIES} xAODMissingET AthContainers EventPrimitives )
