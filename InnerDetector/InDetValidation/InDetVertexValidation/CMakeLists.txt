################################################################################
# Package: InDetVertexValidation
################################################################################

# Declare the package name:
atlas_subdir( InDetVertexValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
			  Control/AthenaBaseComps
			  Control/AthenaMonitoring
			  Event/EventPrimitives			  
			  InnerDetector/InDetRecTools/TrackVertexAssociationTool
			  Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          PRIVATE
                          InnerDetector/InDetValidation/InDetTruthVertexValidation
			  PhysicsAnalysis/AnalysisCommon/PileupReweighting
			  GaudiKernel)

# External dependencies:
find_package( Eigen )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Geometry Tree MathCore Hist RIO pthread TBB )

# Component(s) in the package:
atlas_add_library( InDetVertexValidationLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS InDetVertexValidation
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${CLHEP_LIBRARIES} AsgTools AthenaBaseComps AthenaMonitoring EventPrimitives TrackVertexAssociationTool xAODEventInfo xAODTracking xAODTruth )

atlas_add_component( InDetVertexValidation
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${CLHEP_LIBRARIES} AsgTools AthenaBaseComps AthenaMonitoring EventPrimitives TrackVertexAssociationTool xAODEventInfo xAODTracking xAODTruth InDetVertexValidationLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
