################################################################################
# Package: SCT_ReadoutGeometry
################################################################################

# Declare the package name:
atlas_subdir( SCT_ReadoutGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondTools
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/IdDictDetDescr)

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:

atlas_add_library(SCT_ReadoutGeometry
  src/*.c*
  PUBLIC_HEADERS SCT_ReadoutGeometry
  INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
  DEFINITIONS ${CLHEP_DEFINITIONS}
  LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} InDetReadoutGeometry AthenaKernel SGTools AtlasDetDescr GeoPrimitives Identifier GaudiKernel InDetIdentifier TrkDetElementBase TrkSurfaces TrkEventPrimitives StoreGateLib SGtests
  PRIVATE_LINK_LIBRARIES AthenaPoolUtilities DetDescrConditions IdDictDetDescr)	 
		 
