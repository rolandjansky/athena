################################################################################
# Package: InDetIncSecVxFinderTool
################################################################################

# Declare the package name:
atlas_subdir( InDetIncSecVxFinderTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
			  Control/AthContainers
			  Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PRIVATE
			  DetectorDescription/GeoPrimitives
                          Tracking/TrkEvent/TrkTrack
			  Tracking/TrkEvent/TrkParticleBase
 			  Tracking/TrkEvent/TrkParameters
			  Tracking/TrkEvent/TrkEventPrimitives
			  Tracking/TrkEvent/VxVertex
			  Tracking/TrkEvent/TrkLinks
			  Tracking/TrkEvent/TrkTrackLink
			  Tracking/TrkEvent/TrkTrack
			  Tracking/TrkDetDescr/TrkSurfaces
			  Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
			  Tracking/TrkVertexFitter/TrkVxEdmCnv
			  Event/EventPrimitives
			  InnerDetector/InDetConditions/InDetBeamSpotService
			  InnerDetector/InDetRecTools/InDetTrackSelectionTool )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetIncSecVxFinderTool
                     src/*.cxx
                     src/components/*.cxx
		     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthContainers xAODTracking GaudiKernel InDetRecToolInterfaces GeoPrimitives TrkTrack TrkParticleBase TrkParameters TrkEventPrimitives VxVertex TrkLinks TrkTrack TrkSurfaces TrkVertexFitterInterfaces TrkVxEdmCnvLib EventPrimitives InDetTrackSelectionToolLib)

# Install files from the package:
atlas_install_headers( InDetIncSecVxFinderTool )
