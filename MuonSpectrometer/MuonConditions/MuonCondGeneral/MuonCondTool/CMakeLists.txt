################################################################################
# Package: MuonCondTool
################################################################################

# Declare the package name:
atlas_subdir( MuonCondTool )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MuonCondTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps DetDescrConditions GeoPrimitives GaudiKernel MuonCondData MuonCondInterface SGTools StoreGateLib SGtests AthenaPoolUtilities Identifier MuonAlignmentData MuonCondSvcLib MuonReadoutGeometry MuonIdHelpersLib PathResolver RDBAccessSvcLib )

# Install files from the package:
atlas_install_headers( MuonCondTool )
atlas_install_joboptions( share/*.py )

