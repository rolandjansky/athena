################################################################################
# Package: InDetTruthVertexValidation
################################################################################

# Declare the package name:
atlas_subdir( InDetTruthVertexValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTruth
                          PRIVATE
                          Control/AthenaBaseComps
			  Event/EventPrimitives
                          Event/xAOD/xAODEventInfo
                          GaudiKernel )

# External dependencies:
find_package( Eigen )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Geometry Tree MathCore Hist RIO pthread TBB )

# Component(s) in the package:
atlas_add_library( InDetTruthVertexValidationLib
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS InDetTruthVertexValidation
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${CLHEP_LIBRARIES} EventPrimitives xAODTracking AthenaBaseComps AsgTools xAODEventInfo xAODTruth )

atlas_add_component( InDetTruthVertexValidation
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} ${CLHEP_LIBRARIES} EventPrimitives xAODTracking 
		     AthenaBaseComps AsgTools xAODEventInfo xAODTruth InDetTruthVertexValidationLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

