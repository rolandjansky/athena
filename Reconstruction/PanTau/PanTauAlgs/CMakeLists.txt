################################################################################ 
# Package: PanTauAlgs
################################################################################
 
# Declare the package name:
atlas_subdir( PanTauAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/NavFourMom
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTau
			  Event/xAOD/xAODParticleEvent
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/AthenaKernel
                          Control/DataModel
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          Event/FourMom
                          Event/FourMomUtils
                          Event/xAOD/xAODTracking
                          Reconstruction/Particle
                          Reconstruction/eflowEvent
			  Reconstruction/MVAUtils
                          Reconstruction/tauEvent
			  Reconstruction/tauRecTools
                          Tools/PathResolver
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS MathCore MathMore Matrix Core Tree Hist RIO pthread Minuit Minuit2 Physics HistPainter Rint )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( PanTauAlgs
                     src/*.cxx
		     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps NavFourMom xAODPFlow xAODTau xAODParticleEvent GaudiKernel CaloEvent AthenaKernel DataModel StoreGateLib SGtests GeoPrimitives FourMom FourMomUtils xAODTracking Particle eflowEvent MVAUtils tauEvent tauRecToolsLib PathResolver VxVertex TrkVertexFitterInterfaces )

# Install files from the package:
atlas_install_headers( PanTauAlgs )
atlas_install_runtime( data/weights/* )
atlas_install_python_modules( python/*.py )

