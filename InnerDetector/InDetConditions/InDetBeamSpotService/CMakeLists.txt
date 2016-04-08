################################################################################
# Package: InDetBeamSpotService
################################################################################

# Declare the package name:
atlas_subdir( InDetBeamSpotService )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventPrimitives )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetBeamSpotService
                     src/BeamCondSvc.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel VxVertex AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities EventPrimitives )

atlas_add_dictionary( InDetBeamSpotServiceDict
                      InDetBeamSpotService/IBeamCondSvc.h
                      InDetBeamSpotService/selection.xml
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel VxVertex AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities EventPrimitives )

# Install files from the package:
atlas_install_headers( InDetBeamSpotService )
atlas_install_joboptions( share/*.py )

