################################################################################
# Package: TRT_ReadoutGeometry
################################################################################

# Declare the package name:
atlas_subdir( TRT_ReadoutGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoModel/GeoModelKernel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          DetectorDescription/ReadoutGeometryBase
                          GaudiKernel
                          InnerDetector/InDetConditions/InDetCondServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          DetectorDescription/IdDictDetDescr
                          InnerDetector/InDetConditions/TRT_ConditionsData )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:

atlas_add_library( TRT_ReadoutGeometry
  src/*.c*
  PUBLIC_HEADERS TRT_ReadoutGeometry
  INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
  DEFINITIONS ${CLHEP_DEFINITIONS}
  LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} InDetReadoutGeometry AthenaKernel SGTools AtlasDetDescr GeoModelKernel GeoPrimitives Identifier ReadoutGeometryBase GaudiKernel InDetCondServices InDetIdentifier TrkDetElementBase TrkSurfaces TrkEventPrimitives StoreGateLib SGtests
  PRIVATE_LINK_LIBRARIES AthenaPoolUtilities DetDescrConditions IdDictDetDescr TRT_ConditionsData )
		 
