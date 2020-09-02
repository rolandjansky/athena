################################################################################
# Package: NewVrtSecInclusiveTool
################################################################################

# Declare the package name:
atlas_subdir( NewVrtSecInclusiveTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PRIVATE
                          Tools/PathResolver
                          DetectorDescription/GeoPrimitives
                          Tracking/TrkExtrapolation/TrkExInterfaces
			  Reconstruction/MVAUtils
                          Tracking/TrkEvent/VxSecVertex
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
 			  InnerDetector/InDetConditions/InDetBeamSpotService)

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread TMVA )

# Component(s) in the package:
atlas_add_component( NewVrtSecInclusiveTool 
                     src/*.cxx  src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps xAODTracking PathResolver GaudiKernel 
		Particle TrkVKalVrtFitterLib GeoPrimitives MVAUtils AnalysisUtilsLib TrkParticleBase 
		TrkTrackSummary TrkExInterfaces VxSecVertex )
 
# Install files from the package:
atlas_install_headers( NewVrtSecInclusiveTool )
atlas_install_python_modules( python/*.py )
#atlas_install_joboptions( share/*.txt )
#atlas_install_data( share/*.root )

