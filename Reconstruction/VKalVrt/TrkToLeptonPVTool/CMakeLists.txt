################################################################################
# Package: TrkToLeptonPVTool
################################################################################

# Declare the package name:
atlas_subdir( TrkToLeptonPVTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          PRIVATE
                          Tracking/TrkEvent/VxSecVertex
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          InnerDetector/InDetConditions/InDetBeamSpotService)

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )


# Component(s) in the package:
atlas_add_component( TrkToLeptonPVTool 
                     src/*.cxx  src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps xAODTracking xAODEventInfo GaudiKernel 
		     VxSecVertex TrkVertexFitterInterfaces InDetBeamSpotServiceLib)
 
# Install files from the package:
atlas_install_headers( TrkToLeptonPVTool )
atlas_install_python_modules( python/*.py )

