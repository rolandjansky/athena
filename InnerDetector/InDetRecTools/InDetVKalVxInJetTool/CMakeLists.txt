################################################################################
# Package: InDetVKalVxInJetTool
################################################################################

# Declare the package name:
atlas_subdir( InDetVKalVxInJetTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PRIVATE
			  Control/AthenaKernel
                          Tools/PathResolver
                          DetectorDescription/GeoPrimitives
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          Tracking/TrkEvent/TrkNeutralParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxSecVertex
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
                          Tracking/TrkVertexFitter/TrkVKalVrtFitter
                          Reconstruction/Particle
			  Reconstruction/MVAUtils )
#InnerDetector/InDetRecTools/InDetMaterialRejTool

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread TMVA )

# Library to link against:
#atlas_add_library( InDetVKalVxInJetToolLib 
#                   src/*.cxx  src/components/*.cxx
#		   PUBLIC_HEADERS InDetVKalVxInJetTool InDetTrkInJetType
#                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
#                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps xAODTracking GaudiKernel 
#		        InDetRecToolInterfaces Particle TrkVKalVrtFitterLib GeoPrimitives AnalysisUtilsLib TrkNeutralParameters MVAUtils
#		        TrkParticleBase TrkTrackSummary VxSecVertex VxVertex TrkToolInterfaces TrkVertexFitterInterfaces PathResolver )

# Component(s) in the package:
atlas_add_component( InDetVKalVxInJetTool 
                     src/*.cxx  src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps xAODTracking PathResolver GaudiKernel 
		        InDetRecToolInterfaces Particle TrkVKalVrtFitterLib GeoPrimitives AnalysisUtilsLib TrkNeutralParameters MVAUtils
		        TrkParticleBase TrkTrackSummary VxSecVertex VxVertex TrkToolInterfaces TrkVertexFitterInterfaces PathResolver  )
 
# Install files from the package:
atlas_install_headers( InDetVKalVxInJetTool )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.txt )

#This test is temporarily commented in order not to slow down magnetic field migration to condition data.
#Will be restored later
atlas_add_test( InDetTrkInJetType_test
               SOURCES test/InDetTrkInJetType_test.cxx
               INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
               LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps xAODTracking PathResolver GaudiKernel 
		InDetRecToolInterfaces Particle TrkVKalVrtFitterLib GeoPrimitives AnalysisUtilsLib TrkNeutralParameters MVAUtils
		TrkParticleBase TrkTrackSummary VxSecVertex VxVertex TrkToolInterfaces TrkVertexFitterInterfaces PathResolver AthenaKernel
               LOG_IGNORE_PATTERN "Booking|ClassIDSvc|weight file|create data set info" )
